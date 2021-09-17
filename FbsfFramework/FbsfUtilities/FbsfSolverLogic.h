#ifndef SOLVER_H
#define SOLVER_H
#include <QQuickItem>
#include <QList>

class FbsfSolverLogic : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(FbsfSolverLogic)

public:

    FbsfSolverLogic(QQuickItem *parent = 0);
    ~FbsfSolverLogic();

    Q_PROPERTY(QQuickItem * pageList
               READ pageList
               WRITE setPageList)
    Q_PROPERTY(QVariantList nodeSequence
               READ nodeSequence
               WRITE setNodeSequence)

    Q_INVOKABLE bool solve();

    QQuickItem * pageList(){return mPageList;}
    void setPageList(QQuickItem * aPageList) { mPageList = aPageList;}

    QVariantList nodeSequence(){return mNodeSequence;}
    void setNodeSequence(QVariantList aNodeSequence) { mNodeSequence = aNodeSequence;}

private:

    bool solve(int pass);
    QList<QQuickItem *> Pages(){return mPageList->childItems();}
    QQuickItem *         Page(int iPage){return Pages()[iPage];}
    QList<QQuickItem *> Nodes(int iPage){return Pages()[iPage]->childItems();}
    QQuickItem *         Node(int iPage,int iNode){return Nodes(iPage)[iNode];}

    void                listNodes();

    QQuickItem * mPageList;
    QVariantList mNodeSequence;
};

#endif // SOLVER_H
