import QtQuick 2.0
import Grapher 1.0
import Logic 1.0
Document{
	componentsType:"Logic"
	Page{pageId : "Page_1"
		Constant{
			nodeId :"Constant#1";nodeName :"Constant#1";
			nodeType :"Constant";componentCategory:"NUMERIC";
			x:360;y:480;rotation:0;flipped:0
			nodeParams : {'k' : 1,}}
		Add{
			nodeId :"ADD#1";nodeName :"ADD#1";
			nodeType :"Add";componentCategory:"";
			x:367;y:312;rotation:0;flipped:0
			nodeParams : {'k1' : 1,'k2' : 1,}}
		RealInputTV{
			nodeId :"IMPTV#2";nodeName :"VectorReal_1";
			nodeType :"RealInputTV";componentCategory:"";
			x:111;y:405;rotation:0;flipped:0}
		RealInputTV{
			nodeId :"IMPTV#1";nodeName :"VectorReal_0";
			nodeType :"RealInputTV";componentCategory:"";
			x:111;y:183;rotation:0;flipped:0}
		RealOutputTV{
			nodeId :"EXPTV#1";nodeName :"EXPORT_TV";
			nodeType :"RealOutputTV";componentCategory:"";
			x:670;y:240;rotation:0;flipped:0}
		Link{tailNodeId :"IMPTV#1";tailSocketId :"IMPTV#1.Out";
			headNodeId :"ADD#1";headSocketId :"ADD#1.u1";
			 listPoint:[Qt.point(282,215),Qt.point(282,331)]}
		Link{tailNodeId :"IMPTV#2";tailSocketId :"IMPTV#2.Out";
			headNodeId :"ADD#1";headSocketId :"ADD#1.u2";
			 listPoint:[Qt.point(295,437),Qt.point(295,363)]}
		Link{tailNodeId :"ADD#1";tailSocketId :"ADD#1.y";
			headNodeId :"EXPTV#1";headSocketId :"EXPTV#1.In";
			 listPoint:[Qt.point(540.5,347),Qt.point(540.5,272)]}
	}
}
