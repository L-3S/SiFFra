import QtQuick 2.0
import Grapher 1.0
import Logic 1.0
Document{
	componentsType:"Logic"
	Page{pageId : "Page#1"
		FirstOrder{
			nodeId :"FirstOrder#1";nodeName :"FirstOrder#1";
			nodeType :"FirstOrder";componentCategory:"CONTINUOUS";
			x:670;y:340;rotation:0;flipped:0
			nodeParams : {'k' : 1,'tDer' : 0.1,}}
		RealInput{
			nodeId :"RealInput#1";nodeName :"Pump.SetSpeed";
			nodeType :"RealInput";componentCategory:"INTERFACES";
			x:410;y:370;rotation:0;flipped:0}
		RealOutput{
			nodeId :"RealOutput#1";nodeName :"Pump.Speed";
			nodeType :"RealOutput";componentCategory:"INTERFACES";
			x:1000;y:370;rotation:0;flipped:0}
		BooleanOutput{
			nodeId :"BooleanOutput#1";nodeName :"Pump.State";
			nodeType :"BooleanOutput";componentCategory:"INTERFACES";
			x:1000;y:200;rotation:0;flipped:0}
		BooleanInput{
			nodeId :"BooleanInput#1";nodeName :"Pump.Switch";
			nodeType :"BooleanInput";componentCategory:"INTERFACES";
			x:410;y:200;rotation:0;flipped:0}
		Link{tailNodeId :"FirstOrder#1";tailSocketId :"FirstOrder#1.y";
			headNodeId :"RealOutput#1";headSocketId :"RealOutput#1.u";
			 listPoint:[Qt.point(874.5,390),Qt.point(874.5,392)]}
		Link{tailNodeId :"RealInput#1";tailSocketId :"RealInput#1.y";
			headNodeId :"FirstOrder#1";headSocketId :"FirstOrder#1.u";
			 listPoint:[Qt.point(651,392),Qt.point(651,390)]}
		Link{tailNodeId :"BooleanInput#1";tailSocketId :"BooleanInput#1.y";
			headNodeId :"BooleanOutput#1";headSocketId :"BooleanOutput#1.u";
			 listPoint:[Qt.point(719,222),Qt.point(719,222)]}
	}
}
