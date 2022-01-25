import QtQuick 2.0
import Grapher 1.0
import Graphic 1.0
Document{
	componentsType:"Graphic"
	Page{pageId : "Page#1"
		DigitalSensor{
			nodeId :"DigitalSensor#1";nodeName :"Counter";
			nodeType :"DigitalSensor";componentCategory:"SENSORS";
			x:214;y:349;rotation:0;flipped:0
			nodeParams : {'varName' : "VectorReal_0",'unit' : "Bar",'width' : 100,'height' : 30,'ndigit' : 2,'textColor' : "black",'bgColor' : "lightblue",}}
		BarGraph{
			nodeId :"BarGraph#1";nodeName :"BarGraph#1";
			nodeType :"BarGraph";componentCategory:"SENSORS";
			x:60;y:225;rotation:0;flipped:0
			nodeParams : {'title' : "Counter",'varName' : "VectorReal_0",'nbdigit' : 1,'saturation' : 5,'threshold' : 5,'relativeDisplay' : false,'negColor' : "red",'posColor' : "green",'bgColor' : "gray",'txtColor' : "white",'txtSize' : 10,'horizontal' : true,'width' : 400,'height' : 60,}}
	}
}
