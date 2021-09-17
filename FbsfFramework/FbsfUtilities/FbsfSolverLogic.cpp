#include "FbsfSolverLogic.h"
#include <QString>
#include <QDebug>
#include <QList>
#include <QQuickItem>

const bool optTrace=false;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSolverLogic::FbsfSolverLogic(QQuickItem *parent):
    QQuickItem(parent)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FbsfSolverLogic::~FbsfSolverLogic()
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfSolverLogic::solve()
{
    if(optTrace)
        qDebug() << "~~~~~~~~~~~ FbsfSolverLogic solve ~~~~~~~~~~~~";
    return solve(1);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static bool itemXOrder_sort(QQuickItem *lhs, QQuickItem *rhs)
{
    return lhs->x() < rhs->x();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Topological sorting algorithm
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// nodeSequence ← Empty list that will contain the sorted elements
// nodeResolved ← Set of all nodes with resolved nodes
// nodeUnresolved ← Set of all nodes with unresolved nodes
//
//       for each node n of the Graph
//           reset marked inputs of node n
//           if all inputs resolved (no input connected or delayed)
//                   insert node n in nodeResolved
//           end if
//       end for
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//       while nodeResolved is non-empty do
//           remove a node n from nodeResolved
//           add n to tail of nodeSequence
//           for each node m with a link e from n to m do
//               if ( the input socket of m already marked)
//                   then cycle detected
//               else flag the input socket of m as marked
//                   if m has no other connected/unmarked input socket
//                              then insert m into nodeResolved
//           end for
//       end while
//
//       End of pass 2 ====> RETURN
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//       for each node of the Graph
//           if NOT all inputs resolved (no input connected or delayed)
//                   insert node in nodeUnresolved
//           end if
//       end for
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//       for each node of nodeUnresolved
//             for each input connected and not marked
//                   set input as delayed
//             end for
//       end for
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//       solve(pass = 2)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//       sort nodeUnresolved list with x coordonate
//       while nodeUnresolved is non-empty do
//               remove a node n from nodeResolved
//               reduce delay from node n (recursive traversal)
//       end while
//
//       return nodeSequence (topologically sorted order)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool FbsfSolverLogic::solve(int pass)
{
    QList<QQuickItem *> nodeResolved;
    QVariant ret;

    nodeResolved.clear();// clear the list
    mNodeSequence.clear();
    int nodeCount=0;

    for (int p = Pages().count()-1  ; p >= 0 ; p--)
    {   // loop on pages list
        if(optTrace) qDebug() << Pages()[p]->property("pageId").toString();

        for (int n = 0 ; n < Nodes(p).count() ; n++)
        {   // loop on node list
            if (Node(p,n)->objectName()=="LINK")continue;

            QMetaObject::invokeMethod(Node(p,n), "resetAllMark",
                                      Q_ARG(QVariant, pass));
            nodeCount++;

            QMetaObject::invokeMethod(Node(p,n), "allInputsResolved",
                                      Q_RETURN_ARG(QVariant, ret));

            if (ret.toBool())
            {
                nodeResolved.append(Node(p,n));
                if(optTrace) qDebug() << "\tpass" << pass
                                      << "nodeResolved <-"
                                      << Node(p,n)->property("nodeId").toString();
            }
        }
    }
    if (nodeCount==0)return false;// empty document
    if(optTrace) qDebug() << "pass" << pass << " resolved" << nodeResolved.size();

    while(nodeResolved.size()>0)
    {
        QQuickItem* node = nodeResolved.takeFirst();
        QVariant nodeV;
        nodeV.setValue(node);
        mNodeSequence.append(nodeV);
        // mark all connected inputs
        QMetaObject::invokeMethod(node, "markSuccessorsInputs",
                                  Q_RETURN_ARG(QVariant, ret));
        if(ret.toBool())//false if cycle
        {
            QMetaObject::invokeMethod(node, "successors",
                                      Q_RETURN_ARG(QVariant, ret));

            QVariantList successors=ret.toList();// successor list
            for(QVariantList::const_iterator it = successors.begin();
                it!=successors.end();++it)
            {     // loop on successors list
                QQuickItem * successor = qvariant_cast<QQuickItem *>(*it);
                QMetaObject::invokeMethod(successor, "allConnectedInputsMarked",
                                          Q_RETURN_ARG(QVariant, ret));

                if (ret.toBool())
                {   //push if not already inserted
                    if (    mNodeSequence.indexOf(*it) == -1
                       &&   nodeResolved.indexOf(successor)  == -1)
                    {
                        nodeResolved.append(successor);
                        if(optTrace)qDebug() << "\tpass" << pass
                                             << "nodeResolved successor <-"
                                             << successor->property("nodeId").toString();
                    }
                }
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~// end of pass 2 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (pass == 2)
    {
        if (optTrace) listNodes();
        return false;
    }
    // Check for cycle during pass 1
    QList<QQuickItem *> nodeUnresolved;
    for (int p = Pages().count()-1  ; p >= 0 ; p--)
    {   // loop on pages list
        for (int n = 0 ; n < Nodes(p).count() ; n++)
        {
            if (Node(p,n)->objectName()=="LINK")continue;
            // loop on node list
            QMetaObject::invokeMethod(Node(p,n), "allConnectedInputsMarked",
                                      Q_RETURN_ARG(QVariant, ret));

            if (!ret.toBool())nodeUnresolved.append(Node(p,n));
        }
    }
    if(optTrace) qDebug() << "pass"<< pass << "unresolved" << nodeUnresolved.size();
    if (nodeUnresolved.size() > 0)
    {
        //console.log("Loop in computation sequence ",nodeUnresolved.length)
        for (int l=0 ;l< nodeUnresolved.size();l++)
        {
            QMetaObject::invokeMethod(nodeUnresolved[l], "setDelayedInputs");
        }
        // second pass
        if(optTrace) qDebug() << "Second pass ";
        solve(2);
    }
    else
    {
        if (optTrace) listNodes();
    }
    // Check if empty sequence
    if (mNodeSequence.size() == 0)
    {
        qDebug() << "ERROR : Empty computation sequence";
    }
    //~~~~~~~~~~~~~ Sort the list and  Reduce delay loop ~~~~~~~~~~~~~~~~~~
    std::stable_sort(nodeUnresolved.begin(), nodeUnresolved.end(), itemXOrder_sort);

    while(nodeUnresolved.size()>0)
    {

        QQuickItem* node = nodeUnresolved.takeFirst();
        QMetaObject::invokeMethod(node, "reduceLoopDelay");
    }
    return false;// sequence is solved

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FbsfSolverLogic::listNodes()
{
    qDebug() << "Solver End " ;
    for(QVariantList::const_iterator it = mNodeSequence.begin();
        it!=mNodeSequence.end();++it)
    {     // loop on successors list
        QQuickItem * node = qvariant_cast<QQuickItem *>(*it);
        qDebug() << "\t"
                 << node->property("nodeType").toString()
                 << node->property("nodeId").toString();
    }
}
