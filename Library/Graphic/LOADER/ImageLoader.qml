import QtQuick 2.0
import Grapher 1.0
import fbsfplugins 1.0

Node
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    property var parameters : {"image":"RelativePath_APP_HOME","width":30,"height":30,"z":-1,"move":true}

    Component.onCompleted:
    {
            source = APP_HOME+"/"+parameters.image
            sourceSize.width = parameters.width
            sourceSize.height= parameters.height
            z = parameters.z
            disabelDrag(!parameters.move)
    }
    onParameterModified :
    {
        if(name==="image")
        {
            source = APP_HOME+"/"+parameters.image
            parameters.width = sourceSize.width
            parameters.height = sourceSize.height
        }
        if(name==="width")
            sourceSize.width = parameters.width
        if(name==="height")
            sourceSize.height = parameters.height
        if(name==="z")
            z = parameters.z
        if( name ==="move")
            disabelDrag(!parameters.move)
    }
}
