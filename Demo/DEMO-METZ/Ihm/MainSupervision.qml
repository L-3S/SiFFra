import QtQuick 2.0
import Grapher 1.0
import Graphic 1.0
Document{
	componentsType:"Graphic"
	Page{pageId : "Page_1"
		ImageLoader{
			nodeId :"ImageLoader#2";nodeName :"I";
			nodeType :"ImageLoader";componentCategory:"LOADER";
			x:6;y:11;rotation:0;flipped:0
			nodeParams : {'image' : "Ihm/Img/Ech.jpg",'width' : 300,'height' : 300,'z' : -1,'move' : false,}}
		SwitchOnOff{
			nodeId :"SwitchOnOff#1";nodeName :"Switch";
			nodeType :"SwitchOnOff";componentCategory:"ACTUATORS";
			x:114;y:245;rotation:0;flipped:0
			nodeParams : {'ValName' : "Pump.Switch",}}
		DigitalSensor{
			nodeId :"DigitalSensor#2";nodeName :"Ds";
			nodeType :"DigitalSensor";componentCategory:"SENSORS";
			x:176;y:361;rotation:0;flipped:0
			nodeParams : {'varName' : "Pump.Speed",'unit' : "rpm",'width' : 100,'height' : 30,'ndigit' : 1,'textColor' : "white",'bgColor' : "green",}}
		BinnarySensor{
			nodeId :"BinnarySensor#1";nodeName :"";
			nodeType :"BinnarySensor";componentCategory:"SENSORS";
			x:209;y:258;rotation:0;flipped:0
			nodeParams : {'varName' : "Pump.State",}}
		Slider{
			nodeId :"Slider#1";nodeName :" set";
			nodeType :"Slider";componentCategory:"ACTUATORS";
			x:31;y:356;rotation:0;flipped:0
			nodeParams : {'varName' : "Pump.SetSpeed",'unit' : "rpm",'description' : "PumpSpeedControl",'min' : 1,'max' : 100,}}
		BarGraph{
			nodeId :"BarGraph#1";nodeName :"BarGraph#1";
			nodeType :"BarGraph";componentCategory:"SENSORS";
			x:472;y:189;rotation:0;flipped:0
			nodeParams : {'title' : "Bargraph Pump Speed",'varName' : "Pump.Speed",'nbdigit' : 2,'txtSize' : 10,'saturation' : 10,'threshold' : 12,'relativeDisplay' : false,'negColor' : "red",'posColor' : "green",'bgColor' : "gray",'txtColor' : "white",'horizontal' : true,'width' : 400,'height' : 60,}}
	}
}
