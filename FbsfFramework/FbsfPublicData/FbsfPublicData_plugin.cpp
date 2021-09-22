#include "FbsfPublicData_plugin.h"

#include "FbsfPublicData.h"
#include "FbsfPublicDataModel.h"
#include "FbsfQmlBinding.h"
#ifndef MODE_BATCH

#include <qqml.h>

void FbsfPublicDataPlugin::registerTypes( const char *uri )
{
    // @uri fbsfplugins
    qmlRegisterType<FbsfExporterIntItem>(  uri, 1, 0, "SignalInt" );
    qmlRegisterType<FbsfImporterIntItem>(  uri, 1, 0, "SubscribeInt" );
    qmlRegisterType<FbsfExporterRealItem>( uri, 1, 0, "SignalReal" );
    qmlRegisterType<FbsfImporterRealItem>( uri, 1, 0, "SubscribeReal" );

    qmlRegisterType<FbsfImporterVectorIntItem>( uri, 1, 0, "SubscribeVectorInt" );
    qmlRegisterType<FbsfImporterVectorRealItem>( uri, 1, 0, "SubscribeVectorReal" );
    qmlRegisterType<FbsfExporterVectorIntItem>( uri, 1, 0, "SignalVectorInt" );
    qmlRegisterType<FbsfExporterVectorRealItem>( uri, 1, 0, "SignalVectorReal" );
    qmlRegisterType<FbsfFilterProxyModel>( uri, 1, 0, "FbsfFilterProxyModel" );
}
#endif

