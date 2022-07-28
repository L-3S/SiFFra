import QtQuick 2.0
import Grapher 1.0
import Logic 1.0
Document{
	componentsType:"Logic"
	Page{pageId : "Page#1"
		RealOutput{
			nodeId :"RealOutput#1";nodeName :"CPUHandler_pc";
			nodeType :"RealOutput";componentCategory:"INTERFACES";
			x:970;y:310;rotation:0;flipped:0
			nodeParams : {'unit' : "unit for variable in ZE",'comment' : "comment for variable in ZE",}
			nodeInputs : {'RealOutput#1.u' : 1,}}
	}
}
