import QtQuick 2.0
import Grapher 1.0
import Logic 1.0
Document{
	componentsType:"Logic"
	Page{pageId : "Page#1"
		RealOutput{
			nodeId :"RealOutput#1";nodeName :"titi";
			nodeType :"RealOutput";componentCategory:"INTERFACES";
			x:570;y:250;rotation:0;flipped:0}
		Constant{
			nodeId :"Constant#1";nodeName :"Constant#1";
			nodeType :"Constant";componentCategory:"SOURCES";
			x:179;y:347;rotation:0;flipped:0
            nodeParams : {'k' : 666,'k2' : true,}
			nodeConstants : {'const1' : 12,'const2' : 13,}}
		Link{tailNodeId :"Constant#1";tailSocketId :"Constant#1.y";
			headNodeId :"RealOutput#1";headSocketId :"RealOutput#1.u";
			 listPoint:[Qt.point(385,397),Qt.point(385,272)]}
	}
}
