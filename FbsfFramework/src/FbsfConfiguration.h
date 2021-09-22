#ifndef FBSFCONFIGURATION_H
#define FBSFCONFIGURATION_H
#include "FbsfGlobal.h"
#include "ParamProperties.h"

#include <QtCore/QPointer>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
class QDomElement;
class FbsfConfigNode;

class FBSF_FRAMEWORKSHARED_EXPORT FbsfConfigSequence
{
public:
    FbsfConfigSequence(){}
    //~FbsfConfigSequence(){}
    void Descriptor(QMap<QString,QString>& aDescriptor){ mDescriptor=aDescriptor;}
    QMap<QString,QString>&          Descriptor(){return mDescriptor;}
    QList< QMap<QString,QString> >& Models()    {return mModels;}
    QList<FbsfConfigNode>& Nodes()    {return mNodes;}
private:
    QMap<QString,QString>           mDescriptor;
    QList< QMap<QString,QString> >  mModels;
    QList<FbsfConfigNode>           mNodes;
};

class FBSF_FRAMEWORKSHARED_EXPORT FbsfConfiguration
{

public:
    FbsfConfiguration();

   int                             parseXML(QString aFileName);
   QList< FbsfConfigSequence >&     Sequences() {return mListSequences;}

   QMap<QString,QString>&           Simulation(){return mSimulation;}
   QString&                         Name() { return mConfigName;}
   FbsfConfigSequence&              pluginList() {return mpluginList;}

protected:
   void parseSimulation(const QDomElement& elem);
   void parsePlugins(const QDomElement& elem);
   void parseSequences(const QDomElement& elem);
   void parseModels(FbsfConfigSequence& aSequence,const QDomElement& elem);
   void parseConfigNode(FbsfConfigSequence& aSequence, const QDomElement &elem);

   QMap<QString,QString>            mSimulation;
   QList< FbsfConfigSequence >      mListSequences;
   FbsfConfigSequence               mpluginList;
   QString                          mConfigName;
public :
   ///~~~~~~~~~ moved from ConfigFileVerifier ~~~~~~~~~~~~~~~~~~~
   /// Generic validity checker method for an individual instance
   /// Infos are in the logs
   /// The QString return value corresponds to the name
   static QString CheckValidity(const QMap<QString,QString> &aModuleXml,
                                const QMap<QString,ParamProperties> &aModuleFields
                                );

private:
   static void syst_verif(const QMap<QString, QString> &aXml,
                        const QMap<QString,ParamProperties>::const_iterator iter
                          );

   static void MsgBoxCriticalError();

   static void verif_limite(const QMap<QString,ParamProperties>::const_iterator iter, QVariant value);

};

class FBSF_FRAMEWORKSHARED_EXPORT FbsfConfigNode : public FbsfConfiguration
{
public:
    FbsfConfigNode(){}
    //~FbsfConfigNode(){}
    void parseNode(const QDomElement& elem);
};

#endif // FBSFCONFIGURATION_H
