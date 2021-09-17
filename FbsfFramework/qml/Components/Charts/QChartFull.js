// QChart.js ---
//
// Author: Julien Wintz
// Created: Thu Feb 13 14:37:26 2014 (+0100)
// Version:
// Last-Updated:
//           By:
//     Update #: 94
//
// https://github.com/chartjs/Chart.js/blob/v1.1.1/docs/01-Line-Chart.md
// Change Log:
//
//

var ChartType = {
         BAR: 1,
		DOUGHNUT: 2,
        LINE: 3,
         PIE: 4,
       POLAR: 5,
       RADAR: 6
};

var Chart = function(canvas, context) {

    var chart = this;

// /////////////////////////////////////////////////////////////////
// Line helper
// /////////////////////////////////////////////////////////////////

    this.Line = function(data,options) {

        chart.Line.defaults = {
            backgroundColor : "teal",
            scaleOverlay: false,
            scaleOverride: false,
            scaleSteps: null,
            scaleStepWidth: null,
            scaleStartValue: null,
            scaleLineColor: "rgba(250,250,250,.8)",
            scaleLineWidth: 2,
            scaleShowLabels: true,
            scaleLabel: "<%=value%>",
            scaleFontFamily: "'Arial'",
            scaleFontSize: 14,
            scaleFontStyle: "normal",
            scaleFontColor: "#666",
            scaleShowGridLines: true,
            scaleGridLineColor: "rgba(250,250,250,.2)",
            scaleGridLineWidth: 1,
            scaleXMargin:40,
            scaleYMargin:40,
            bezierCurve: true,
            pointDot: true,
            pointDotRadius: 4,
            pointDotStrokeWidth: 2,
            datasetStroke: true,
            datasetStrokeWidth: 2,
            datasetFill: true,
            xmarker: false,
            xTimeLine: false
        };

        var config = (options) ? mergeChartConfig(chart.Line.defaults,options)
                               : chart.Line.defaults;

        return new Line(data,config,context);
    }
// /////////////////////////////////////////////////////////////////
// Polar area helper
// /////////////////////////////////////////////////////////////////

    this.PolarArea = function(data, options) {

        chart.PolarArea.defaults = {
            scaleOverlay: true,
            scaleOverride: false,
            scaleSteps: null,
            scaleStepWidth: null,
            scaleStartValue: null,
            scaleShowLine: true,
            scaleLineColor: "rgba(0,0,0,.1)",
            scaleLineWidth: 1,
            scaleShowLabels: true,
            scaleLabel: "<%=value%>",
            scaleFontFamily: "'Arial'",
            scaleFontSize: 12,
            scaleFontStyle: "normal",
            scaleFontColor: "#666",
            scaleShowLabelBackdrop: true,
            scaleBackdropColor: "rgba(255,255,255,0.75)",
            scaleBackdropPaddingY: 2,
            scaleBackdropPaddingX: 2,
            segmentShowStroke: true,
            segmentStrokeColor: "#fff",
            segmentStrokeWidth: 2,
            animation: true,
            animationSteps: 100,
            animationEasing: "easeOutBounce",
            animateRotate: true,
            animateScale: false,
            onAnimationComplete: null
        };

        var config = (options) ? mergeChartConfig(chart.PolarArea.defaults,options) : chart.PolarArea.defaults;

        return new PolarArea(data,config,context);
    };

// /////////////////////////////////////////////////////////////////
// Radar helper
// /////////////////////////////////////////////////////////////////

    this.Radar = function(data,options) {

        chart.Radar.defaults = {
            scaleOverlay: false,
            scaleOverride: false,
            scaleSteps: null,
            scaleStepWidth: null,
            scaleStartValue: null,
            scaleShowLine: true,
            scaleLineColor: "rgba(0,0,0,.1)",
            scaleLineWidth: 1,
            scaleShowLabels: false,
            scaleLabel: "<%=value%>",
            scaleFontFamily: "'Arial'",
            scaleFontSize: 12,
            scaleFontStyle: "normal",
            scaleFontColor: "#666",
            scaleShowLabelBackdrop: true,
            scaleBackdropColor: "rgba(255,255,255,0.75)",
            scaleBackdropPaddingY: 2,
            scaleBackdropPaddingX: 2,
            angleShowLineOut: true,
            angleLineColor: "rgba(0,0,0,.1)",
            angleLineWidth: 1,
            pointLabelFontFamily: "'Arial'",
            pointLabelFontStyle: "normal",
            pointLabelFontSize: 12,
            pointLabelFontColor: "#666",
            pointDot: true,
            pointDotRadius: 3,
            pointDotStrokeWidth: 1,
            datasetStroke: true,
            datasetStrokeWidth: 2,
            datasetFill: true,
            animation: true,
            animationSteps: 60,
            animationEasing: "easeOutQuart",
            onAnimationComplete: null
        };

        var config = (options)? mergeChartConfig(chart.Radar.defaults,options) : chart.Radar.defaults;

        return new Radar(data,config,context);
    };

// /////////////////////////////////////////////////////////////////
// Pie helper
// /////////////////////////////////////////////////////////////////

    this.Pie = function(data,options) {

        chart.Pie.defaults = {
            segmentShowStroke: true,
            segmentStrokeColor: "#fff",
            segmentStrokeWidth: 2,
            animation: true,
            animationSteps: 100,
            animationEasing: "easeOutBounce",
            animateRotate: true,
            animateScale: false,
            onAnimationComplete: null
        };

        var config = (options)? mergeChartConfig(chart.Pie.defaults,options) : chart.Pie.defaults;

        return new Pie(data,config,context);
    };

// /////////////////////////////////////////////////////////////////
// Doughnut helper
// /////////////////////////////////////////////////////////////////

    this.Doughnut = function(data,options) {

        chart.Doughnut.defaults = {
            segmentShowStroke: true,
            segmentStrokeColor: "#fff",
            segmentStrokeWidth: 2,
            percentageInnerCutout: 50,
            animation: true,
            animationSteps: 100,
            animationEasing: "easeOutBounce",
            animateRotate: true,
            animateScale: false,
            onAnimationComplete: null
        };

        var config = (options)? mergeChartConfig(chart.Doughnut.defaults,options) : chart.Doughnut.defaults;

        return new Doughnut(data,config,context);

    };

// /////////////////////////////////////////////////////////////////
// Bar helper
// /////////////////////////////////////////////////////////////////

    this.Bar = function(data,options) {

        chart.Bar.defaults = {
            scaleOverlay: false,
            scaleOverride: false,
            scaleSteps: null,
            scaleStepWidth: null,
            scaleStartValue: null,
            scaleLineColor: "rgba(0,0,0,.1)",
            scaleLineWidth: 1,
            scaleShowLabels: true,
            scaleLabel: "<%=value%>",
            scaleFontFamily: "'Arial'",
            scaleFontSize: 12,
            scaleFontStyle: "normal",
            scaleFontColor: "#666",
            scaleShowGridLines: true,
            scaleGridLineColor: "rgba(0,0,0,.05)",
            scaleGridLineWidth: 1,
            barShowStroke: true,
            barStrokeWidth: 2,
            barValueSpacing: 5,
            barDatasetSpacing: 1,
            animation: true,
            animationSteps: 60,
            animationEasing: "easeOutQuart",
            onAnimationComplete: null
        };

        var config = (options) ? mergeChartConfig(chart.Bar.defaults,options) : chart.Bar.defaults;

        return new Bar(data,config,context);
    }

    // /////////////////////////////////////////////////////////////////
    // Line implementation
    // /////////////////////////////////////////////////////////////////

        var Line = function(data,config,ctx) {

            var maxSize;
            var scaleHop;
            var calculatedScale;
            var labelHeight;
            var scaleHeight;
            var valueBounds;
            var labelTemplateString;
            var valueHop;
            var widestXLabel;
            var xAxisLength;
            var yAxisPosX;
            var xAxisPosY;
            var rotateLabels = 0;

            var once=1;
            var rateX=1;

            var chartWidth = width-2*config.scaleXMargin
            var chartHeight= height-2*config.scaleYMargin

            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // initialisation
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            this.init = function () {
                if (once)
                {
                    ctx.font = config.scaleFontStyle + " "
                            + config.scaleFontSize+"px "
                            + config.scaleFontFamily;
                    once=0
                }

                // GRO ratio to compute number of X labels
                rateX=Math.floor(data.timeSet.length/data.labelsNumber);// GRO added

                calculateDrawingSizes();

                valueBounds = getValueBounds();
                labelTemplateString = (config.scaleShowLabels)? config.scaleLabel : "";

                if (!config.scaleOverride) {
                    calculatedScale = calculateScale(scaleHeight,valueBounds.maxSteps,
                                                     valueBounds.minSteps,valueBounds.maxValue,
                                                     valueBounds.minValue,labelTemplateString);
                } else {
                    calculatedScale = {
                        steps: config.scaleSteps,
                        stepValue: config.scaleStepWidth,
                        graphMin: config.scaleStartValue,
                        labels: []
                    }
                    populateLabels(labelTemplateString, calculatedScale.labels,
                                   calculatedScale.steps,config.scaleStartValue,
                                   config.scaleStepWidth);
                }
                scaleHop = Math.floor(scaleHeight/calculatedScale.steps);
                calculateXAxisSize();
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // drawing
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            this.draw = function (progress) {
                if (once) this.init();

                clear(ctx);
                var gradient = ctx.createRadialGradient(0, 100, 100, 200, 0, 600);
                 gradient.addColorStop(0, Qt.lighter(config.backgroundColor, 1.2));
                 gradient.addColorStop(1, Qt.lighter(config.backgroundColor, 0.8));
                ctx.fillStyle = gradient;
                ctx.fillRect(6, 6, width-12, height-12);

                if(config.scaleOverlay) {
                    drawLines();
                    drawScale();
                } else {
                    drawScale();
                    drawLines();
                }
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            this.bindMouseEvent = function (evt) {
                var mouseX, mouseY;
                mouseX = evt.clientX - evt.left;
                mouseY = evt.clientY - evt.top;
                var pointHitDetectionRadius = 4
                var radius=4
                var hitDetectionRange = pointHitDetectionRadius + radius;
                for (var i=0; i<data.datasets.length; i++) {
                    var dataset=data.datasets[i]
                    for (var j=0; j<dataset.data.length; j++) {
                        var x=yAxisPosX + (valueHop * j);
                        var y=xAxisPosY - (calculateOffset(dataset.data[j],calculatedScale,scaleHop));
                        if ((Math.pow(mouseX-x, 2)+Math.pow(mouseY-y, 2)) < Math.pow(hitDetectionRange,2))
                            return{
                                x : x,
                                y : y,
                                name : dataset.label,
                                valx : getDateStr(data.timeSet[j]),
                                valy : dataset.data[j]
                            }
                      }
                 }
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            this.getPointFromMouseX = function (evt) {
                var mouseX, mouseY;
                mouseX = evt.clientX
                mouseY = evt.clientY
                var index=Math.round((mouseX-yAxisPosX)/valueHop)
                return{index:index,value:data.timeSet[index]}
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function drawLines() {
                var labelsWidth=100 /// GRO added legend
                //~~~~~~~~~~ GRO added display unit ~~~~~~~~~~
                var displayUnit
                if(data.datasets.displayUnit!==undefined)
                {
                    displayUnit=data.datasets.displayUnit.display
                    ctx.font = "bold 16px sans-serif";
                    ctx.fillText(displayUnit, yAxisPosX , 15);
                }

                for (var i=0; i<data.datasets.length; i++)
                {
                    var dataset=data.datasets[i]
                    //~~~~~~~~~~ GRO added legend ~~~~~~~~~~
                    if(dataset.label!=="")
                    {
                        ctx.fillStyle=dataset.strokeColor
                        ctx.font = "bold 16px sans-serif";
                        ctx.textAlign = "left";

                        var label=dataset.label
                        if (displayUnit===undefined && dataset.unit!=="")
                                label+="("+dataset.unit+")"
                        ctx.fillText(label, yAxisPosX + labelsWidth,15);
                        labelsWidth+=ctx.measureText(label).width+20
                    }

                    //~~~~~~~~~~ end GRO ~~~~~~~~~~

                    ctx.strokeStyle = dataset.strokeColor;
                    ctx.lineWidth = config.datasetStrokeWidth;
                    ctx.beginPath();
                    ctx.moveTo(yAxisPosX, xAxisPosY - calculateOffset(dataset.data[0],calculatedScale,scaleHop))
                    for (var j=1; j<dataset.data.length; j++)
                    {
                        var x=yAxisPosX + (valueHop * j);
                        var tmp=(dataset.data[j] - calculatedScale.graphMin)/calculatedScale.stepValue
                        var y=xAxisPosY - (scaleHop*tmp)

                        if (config.bezierCurve) {
                            var xm05=yAxisPosX + (valueHop * (j-0.5));
                            var tmpm1=(dataset.data[j-1] - calculatedScale.graphMin)/calculatedScale.stepValue
                            var ym1=xAxisPosY - (scaleHop*tmpm1)
                            ctx.bezierCurveTo(xm05,ym1,xm05,y,x,y);
                        } else{
                            ctx.lineTo(x,y);
                        }
                    }

                    ctx.stroke();

                    if (config.datasetFill) {
                        ctx.lineTo(yAxisPosX + (valueHop*(dataset.data.length-1)),xAxisPosY);
                        ctx.lineTo(yAxisPosX,xAxisPosY);
                        ctx.closePath();
                        ctx.fillStyle = dataset.fillColor;
                        ctx.fill();
                    } else {
                        ctx.closePath();
                    }
                    if (config.pointDot) {
                        ctx.fillStyle = dataset.pointColor;
                        ctx.strokeStyle = dataset.pointStrokeColor;
                        ctx.lineWidth = config.pointDotStrokeWidth;
                        for (var k=0; k<dataset.data.length; k++) {
                            ctx.beginPath();
                            ctx.arc(yAxisPosX + (valueHop *k),
                                    xAxisPosY - calculateOffset(dataset.data[k],calculatedScale,scaleHop),
                                    config.pointDotRadius,0,Math.PI*2,true);
                            ctx.fill();
                            ctx.stroke();
                        }
                    }
                }
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function drawScale() {

                // Draw X scale
                ctx.lineWidth = config.scaleLineWidth;
                ctx.strokeStyle = config.scaleLineColor;
                ctx.beginPath();
                ctx.moveTo(chartWidth,xAxisPosY);
                ctx.lineTo(chartWidth-xAxisLength,xAxisPosY);
                ctx.stroke();
                if (rotateLabels > 0) {
                    ctx.save();
                    ctx.textAlign = "right";
                } else{
                    ctx.textAlign = "center";
                }
                ctx.fillStyle = config.scaleFontColor;

                for (var i=0; i<data.timeSet.length; i++)
                {
                    ctx.save();
                    //~~~~~~~~~~ GRO added ~~~~~~~~~~
                    if (i%rateX==0 || i==data.timeSet.length-1 || data.timeSet.length <data.labelsNumber )
                    {   // display X Labels
                        if (rotateLabels > 0) {
                            ctx.translate(yAxisPosX + i*valueHop,
                                          xAxisPosY + config.scaleFontSize);
                            ctx.rotate(-(rotateLabels * (Math.PI/180)));
                            ctx.fillText(getDateStr(data.timeSet[i]), 0,0);
                            ctx.restore();
                        } else {//  manage overlay of last labels
                            if (i<data.timeSet.length-1 &&
                                i*valueHop+widestXLabel>(data.timeSet.length-1)*valueHop)
                                continue
                            ctx.fillText(getDateStr(data.timeSet[i]),
                                         yAxisPosX + i*valueHop,
                                         xAxisPosY + config.scaleFontSize+3);
                        }
                    }
                    ctx.beginPath();

                    ctx.moveTo(yAxisPosX + i * valueHop, xAxisPosY+3);
                    // draw vertical lines
                    if(config.scaleShowGridLines && i>0)
                    {
                        if (i%rateX==0 || i==data.timeSet.length-1)
                            ctx.strokeStyle = "rgba(250,250,250,0.5)";
                        else
                            ctx.strokeStyle = config.scaleGridLineColor;
                        if (i%(rateX/2)==0 || i==data.timeSet.length-1)
                        {
                            ctx.lineWidth = config.scaleGridLineWidth;
                            ctx.lineTo(yAxisPosX + i * valueHop, config.scaleYMargin);
                        }
                    }
                    else
                    {
                        ctx.lineTo(yAxisPosX + i * valueHop, xAxisPosY+3);
                    }
                    ctx.stroke();
                    //~~~~~~~~~~ GRO added a X marker~~~~~~~~~~~~~~~~~~~
                    if (config.xmarker && data.timeSet[i]===data.xmarker)
                    {
                        ctx.lineWidth = config.scaleGridLineWidth*2;
                        ctx.strokeStyle = "rgba(255,0,0,1.0)";
                        ctx.beginPath();
                        ctx.moveTo(yAxisPosX + i * valueHop + valueHop/2, xAxisPosY+3);
                        ctx.lineTo(yAxisPosX + i * valueHop + valueHop/2, config.scaleYMargin);
                        ctx.stroke();
                    }
                    //~~~~~~~~~~ GRO added a X time line~~~~~~~~~~~~~~~~~~~
                    if (config.xTimeLine && data.timeSet[i]===data.xTimeLine)
                    {
                        ctx.lineWidth = config.scaleGridLineWidth*4;
                        ctx.strokeStyle = "rgba(240, 240, 240,1.0)";
                        ctx.beginPath();
                        ctx.moveTo(yAxisPosX + i * valueHop , xAxisPosY+3);
                        ctx.lineTo(yAxisPosX + i * valueHop , config.scaleYMargin);
                        ctx.stroke();
                    }
                    //~~~~~~~~~~ GRO end added ~~~~~~~~~~~
                }
                // Draw Y scale
                ctx.lineWidth = config.scaleLineWidth;
                ctx.strokeStyle = config.scaleLineColor;
                ctx.beginPath();
                ctx.moveTo(yAxisPosX,xAxisPosY+5);
                ctx.lineTo(yAxisPosX,config.scaleYMargin);
                ctx.stroke();
                ctx.textAlign = "right";
                ctx.textBaseline = "middle";
                // Draw horizontal grid lines
                for (var j=0; j<calculatedScale.steps; j++) {
                    ctx.beginPath();
                    ctx.moveTo(yAxisPosX-3,xAxisPosY - ((j+1) * scaleHop));
                    if (config.scaleShowGridLines) {
                        ctx.lineWidth = config.scaleGridLineWidth;
                        ctx.strokeStyle = config.scaleGridLineColor;
                        ctx.lineTo(yAxisPosX + xAxisLength + 5,xAxisPosY - ((j+1) * scaleHop));
                    } else {
                        ctx.lineTo(yAxisPosX-0.5,xAxisPosY - ((j+1) * scaleHop));
                    }
                    ctx.stroke();
                    // Draw horizontal grid lines
                    ctx.fillText(calculatedScale.labels[j],yAxisPosX-8,xAxisPosY - ((j+1) * scaleHop));
                }
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Compute X axis size and  Y position x and X position y
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function calculateXAxisSize()
            {
                xAxisLength = chartWidth - 2*config.scaleXMargin
                valueHop = xAxisLength/(data.timeSet.length-1);

                yAxisPosX = 2*config.scaleXMargin
                xAxisPosY = chartHeight+config.scaleYMargin
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Compute Y size
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function calculateDrawingSizes()
            {
                maxSize = chartHeight;
                widestXLabel = 1;
                for (var i=0; i<data.timeSet.length; i++) {
                    if (i%rateX==0) //~~~~~~~~~~ GRO added ~~~~~~~~~~
                    {
                        var textLength = ctx.measureText(getDateStr(data.timeSet[i])).width;
                        widestXLabel = (textLength > widestXLabel)? textLength : widestXLabel;
                    }
                }
                // Check for labels rotation conditions
                if (chartWidth/data.labelsNumber < widestXLabel)
                {
                    rotateLabels = 45;

                    if (chartWidth/data.labelsNumber < Math.cos(rotateLabels) * widestXLabel)
                    {
                        rotateLabels = 90;
                        maxSize -= widestXLabel;
                    }
                    else
                    {
                        maxSize -= Math.sin(rotateLabels) * widestXLabel;
                    }
                }
                else
                {
                    maxSize -= config.scaleFontSize;
                }
                labelHeight = config.scaleFontSize;

                scaleHeight = maxSize;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            function getValueBounds()
            {
                var upperValue = -Number.MAX_VALUE;// GRO change MIN to -MAX
                var lowerValue = Number.MAX_VALUE;

                for (var i=0; i<data.datasets.length; i++)
                {
                    var dataset=data.datasets[i]
                    for (var j=0; j<dataset.data.length; j++)
                    {
                        var value=data.datasets[i].data[j]
                        // compute display unit if defined
                        if(data.datasets.displayUnit!==undefined)
                            value=value*data.datasets.displayUnit.arg1
                                       +data.datasets.displayUnit.arg2
                        if ( value > upperValue) { upperValue = value };
                        if ( value < lowerValue) { lowerValue = value };

                        data.datasets[i].data[j]=value
                    }
                };
                // GRO limit number of labels on Y scale (max/4 min=2)
                var maxSteps = Math.floor((scaleHeight / (labelHeight*0.66))/4);
                var minSteps = 2    //Math.floor((scaleHeight / labelHeight*0.5));

                return {
                    maxValue: upperValue,
                    minValue: lowerValue,
                    maxSteps: maxSteps,
                    minSteps: minSteps
                };
            }
        }

// /////////////////////////////////////////////////////////////////
// Polar Area implementation
// /////////////////////////////////////////////////////////////////

    var PolarArea = function(data,config,ctx) {

        var maxSize;
        var scaleHop;
        var calculatedScale;
        var labelHeight;
        var scaleHeight;
        var valueBounds;
        var labelTemplateString;

        // /////////////////////////////////////////////////////////////////
        // initialisation
        // /////////////////////////////////////////////////////////////////

        this.init = function() {

            calculateDrawingSizes();

            valueBounds = getValueBounds();

            labelTemplateString = (config.scaleShowLabels)? config.scaleLabel : null;

            if (!config.scaleOverride) {
                calculatedScale = calculateScale(scaleHeight,valueBounds.maxSteps,valueBounds.minSteps,valueBounds.maxValue,valueBounds.minValue,labelTemplateString);
            } else {
                calculatedScale = {
                    steps: config.scaleSteps,
                    stepValue: config.scaleStepWidth,
                    graphMin: config.scaleStartValue,
                    labels: []
                }
                populateLabels(labelTemplateString, calculatedScale.labels,calculatedScale.steps,config.scaleStartValue,config.scaleStepWidth);
            }

            scaleHop = maxSize/(calculatedScale.steps);
        }

        // /////////////////////////////////////////////////////////////////
        // drawing
        // /////////////////////////////////////////////////////////////////

        this.draw = function(progress) {

            clear(ctx);

            if(config.scaleOverlay) {
                drawAllSegments(progress);
                drawScale();
            } else {
                drawScale();
                drawAllSegments(progress);
            }
        }

        // ///////////////////////////////////////////////////////////////

        function calculateDrawingSizes() {

            maxSize = (Min([width,height])/2);
            maxSize -= Max([config.scaleFontSize*0.5,config.scaleLineWidth*0.5]);

            labelHeight = config.scaleFontSize*2;

            if (config.scaleShowLabelBackdrop) {

                labelHeight += (2 * config.scaleBackdropPaddingY);
                maxSize -= config.scaleBackdropPaddingY*1.5;
            }

            scaleHeight = maxSize;
            labelHeight = Default(labelHeight,5);
        }

        function drawScale() {

            for (var i=0; i<calculatedScale.steps; i++) {

                if (config.scaleShowLine) {
                    ctx.beginPath();
                    ctx.arc(width/2, height/2, scaleHop * (i + 1), 0, (Math.PI * 2), true);
                    ctx.strokeStyle = config.scaleLineColor;
                    ctx.lineWidth = config.scaleLineWidth;
                    ctx.stroke();
                }

                if (config.scaleShowLabels) {
                    ctx.textAlign = "center";
                    ctx.font = config.scaleFontStyle + " " + config.scaleFontSize + "px " + config.scaleFontFamily;

                    var label =  calculatedScale.labels[i];

                    if (config.scaleShowLabelBackdrop) {
                        var textWidth = ctx.measureText(label).width;
                        ctx.fillStyle = config.scaleBackdropColor;
                        ctx.beginPath();
                        ctx.rect(
                            Math.round(width/2 - textWidth/2 - config.scaleBackdropPaddingX),
                            Math.round(height/2 - (scaleHop * (i + 1)) - config.scaleFontSize*0.5 - config.scaleBackdropPaddingY),
                            Math.round(textWidth + (config.scaleBackdropPaddingX*2)),
                            Math.round(config.scaleFontSize + (config.scaleBackdropPaddingY*2))
                        );
                        ctx.fill();
                    }
                    ctx.textBaseline = "middle";
                    ctx.fillStyle = config.scaleFontColor;
                    ctx.fillText(label,width/2,height/2 - (scaleHop * (i + 1)));
                }
            }
        }

        function drawAllSegments(animationDecimal) {

            var startAngle = -Math.PI/2,

            angleStep = (Math.PI*2)/data.length,
            scaleAnimation = 1,
            rotateAnimation = 1;

            if (config.animation) {
                if (config.animateScale) {
                    scaleAnimation = animationDecimal;
                }
                if (config.animateRotate) {
                    rotateAnimation = animationDecimal;
                }
            }

            for (var i=0; i<data.length; i++) {

                ctx.beginPath();
                ctx.arc(width/2,height/2,scaleAnimation * calculateOffset(data[i].value,calculatedScale,scaleHop),startAngle, startAngle + rotateAnimation*angleStep, false);
                ctx.lineTo(width/2,height/2);
                ctx.closePath();
                ctx.fillStyle = data[i].color;
                ctx.fill();

                if(config.segmentShowStroke) {
                    ctx.strokeStyle = config.segmentStrokeColor;
                    ctx.lineWidth = config.segmentStrokeWidth;
                    ctx.stroke();
                }
                startAngle += rotateAnimation*angleStep;
            }
        }

        function getValueBounds() {

            var upperValue = Number.MIN_VALUE;
            var lowerValue = Number.MAX_VALUE;

            for (var i=0; i<data.length; i++) {
                if (data[i].value > upperValue) {upperValue = data[i].value;}
                if (data[i].value < lowerValue) {lowerValue = data[i].value;}
            };

            var maxSteps = Math.floor((scaleHeight / (labelHeight*0.66)));
            var minSteps = Math.floor((scaleHeight / labelHeight*0.5));

            return {
                maxValue: upperValue,
                minValue: lowerValue,
                maxSteps: maxSteps,
                minSteps: minSteps
            };
        }
    }

// /////////////////////////////////////////////////////////////////
// Radar implementation
// /////////////////////////////////////////////////////////////////

    var Radar = function (data, config, ctx) {

        var maxSize;
        var scaleHop;
        var calculatedScale;
        var labelHeight;
        var scaleHeight;
        var valueBounds;
        var labelTemplateString;

        // /////////////////////////////////////////////////////////////////
        // initialisation
        // /////////////////////////////////////////////////////////////////

        this.init = function () {

            if (!data.labels) data.labels = [];

            calculateDrawingSizes();

            var valueBounds = getValueBounds();

            labelTemplateString = (config.scaleShowLabels)? config.scaleLabel : null;

            if (!config.scaleOverride) {
                calculatedScale = calculateScale(scaleHeight,valueBounds.maxSteps,valueBounds.minSteps,valueBounds.maxValue,valueBounds.minValue,labelTemplateString);
            } else {
                calculatedScale = {
                    steps: config.scaleSteps,
                    stepValue: config.scaleStepWidth,
                    graphMin: config.scaleStartValue,
                    labels: []
                }
                populateLabels(labelTemplateString, calculatedScale.labels,calculatedScale.steps,config.scaleStartValue,config.scaleStepWidth);
            }

            scaleHop = maxSize/(calculatedScale.steps);
        }

        // /////////////////////////////////////////////////////////////////
        // drawing
        // /////////////////////////////////////////////////////////////////

        this.draw = function(progress) {

            clear(ctx);

            if(config.scaleOverlay) {
                drawAllDataPoints(progress);
                drawScale();
            } else {
                drawScale();
                drawAllDataPoints(progress);
            }
        }

        // ///////////////////////////////////////////////////////////////

        function drawAllDataPoints(animationDecimal) {

            var rotationDegree = (2*Math.PI)/data.datasets[0].data.length;

            ctx.save();
            ctx.translate(width/2,height/2);

            for (var i=0; i<data.datasets.length; i++) {
                ctx.beginPath();
                ctx.moveTo(0,animationDecimal*(-1*calculateOffset(data.datasets[i].data[0],calculatedScale,scaleHop)));
                for (var j=1; j<data.datasets[i].data.length; j++) {
                    ctx.rotate(rotationDegree);
                    ctx.lineTo(0,animationDecimal*(-1*calculateOffset(data.datasets[i].data[j],calculatedScale,scaleHop)));

                }
                ctx.closePath();

                ctx.fillStyle = data.datasets[i].fillColor;
                ctx.strokeStyle = data.datasets[i].strokeColor;
                ctx.lineWidth = config.datasetStrokeWidth;
                ctx.fill();
                ctx.stroke();

                if (config.pointDot) {
                    ctx.fillStyle = data.datasets[i].pointColor;
                    ctx.strokeStyle = data.datasets[i].pointStrokeColor;
                    ctx.lineWidth = config.pointDotStrokeWidth;
                    for (var k=0; k<data.datasets[i].data.length; k++) {
                        ctx.rotate(rotationDegree);
                        ctx.beginPath();
                        ctx.arc(0,animationDecimal*(-1*calculateOffset(data.datasets[i].data[k],calculatedScale,scaleHop)),config.pointDotRadius,2*Math.PI,false);
                        ctx.fill();
                        ctx.stroke();
                    }
                }
                ctx.rotate(rotationDegree);
            }
            ctx.restore();
        }

        function drawScale() {

            var rotationDegree = (2*Math.PI)/data.datasets[0].data.length;

            ctx.save();
            ctx.translate(width/2, height/2);

            if (config.angleShowLineOut) {
                ctx.strokeStyle = config.angleLineColor;
                ctx.lineWidth = config.angleLineWidth;
                for (var h=0; h<data.datasets[0].data.length; h++) {
                    ctx.rotate(rotationDegree);
                    ctx.beginPath();
                    ctx.moveTo(0,0);
                    ctx.lineTo(0,-maxSize);
                    ctx.stroke();
                }
            }

            for (var i=0; i<calculatedScale.steps; i++) {
                ctx.beginPath();
                if(config.scaleShowLine) {
                    ctx.strokeStyle = config.scaleLineColor;
                    ctx.lineWidth = config.scaleLineWidth;
                    ctx.moveTo(0,-scaleHop * (i+1));
                    for (var j=0; j<data.datasets[0].data.length; j++) {
                        ctx.rotate(rotationDegree);
                        ctx.lineTo(0,-scaleHop * (i+1));
                    }
                    ctx.closePath();
                    ctx.stroke();
                }

                if (config.scaleShowLabels) {
                    ctx.textAlign = 'center';
                    ctx.font = config.scaleFontStyle + " " + config.scaleFontSize+"px " + config.scaleFontFamily;
                    ctx.textBaseline = "middle";
                    if (config.scaleShowLabelBackdrop) {
                        var textWidth = ctx.measureText(calculatedScale.labels[i]).width;
                        ctx.fillStyle = config.scaleBackdropColor;
                        ctx.beginPath();
                        ctx.rect(
                            Math.round(- textWidth/2 - config.scaleBackdropPaddingX),     //X
                            Math.round((-scaleHop * (i + 1)) - config.scaleFontSize*0.5 - config.scaleBackdropPaddingY),//Y
                            Math.round(textWidth + (config.scaleBackdropPaddingX*2)), //Width
                            Math.round(config.scaleFontSize + (config.scaleBackdropPaddingY*2)) //Height
                        );
                        ctx.fill();
                    }
                    ctx.fillStyle = config.scaleFontColor;
                    ctx.fillText(calculatedScale.labels[i],0,-scaleHop*(i+1));
                }
            }

            for (var k=0; k<data.labels.length; k++) {

                ctx.font = config.pointLabelFontStyle + " " + config.pointLabelFontSize+"px " + config.pointLabelFontFamily;
                ctx.fillStyle = config.pointLabelFontColor;

                var opposite = Math.sin(rotationDegree*k) * (maxSize + config.pointLabelFontSize);
                var adjacent = Math.cos(rotationDegree*k) * (maxSize + config.pointLabelFontSize);

                if(rotationDegree*k == Math.PI || rotationDegree*k == 0) {
                    ctx.textAlign = "center";
                } else if(rotationDegree*k > Math.PI) {
                    ctx.textAlign = "right";
                } else {
                    ctx.textAlign = "left";
                }
                ctx.textBaseline = "middle";
                ctx.fillText(data.labels[k],opposite,-adjacent);
            }
            ctx.restore();
        };

        function calculateDrawingSizes() {

            maxSize = (Min([width,height])/2);
            labelHeight = config.scaleFontSize*2;

            var labelLength = 0;

            for (var i=0; i<data.labels.length; i++) {
                ctx.font = config.pointLabelFontStyle + " " + config.pointLabelFontSize+"px " + config.pointLabelFontFamily;
                var textMeasurement = ctx.measureText(data.labels[i]).width;
                if(textMeasurement>labelLength) labelLength = textMeasurement;
            }

            maxSize -= Max([labelLength,((config.pointLabelFontSize/2)*1.5)]);
            maxSize -= config.pointLabelFontSize;
            maxSize = CapValue(maxSize, null, 0);

            scaleHeight = maxSize;
            labelHeight = Default(labelHeight,5);
        };

        function getValueBounds() {

            var upperValue = Number.MIN_VALUE;
            var lowerValue = Number.MAX_VALUE;

            for (var i=0; i<data.datasets.length; i++) {
                for (var j=0; j<data.datasets[i].data.length; j++) {
                    if (data.datasets[i].data[j] > upperValue) {upperValue = data.datasets[i].data[j]}
                    if (data.datasets[i].data[j] < lowerValue) {lowerValue = data.datasets[i].data[j]}
                }
            }

            var maxSteps = Math.floor((scaleHeight / (labelHeight*0.66)));
            var minSteps = Math.floor((scaleHeight / labelHeight*0.5));

            return {
                maxValue: upperValue,
                minValue: lowerValue,
                maxSteps: maxSteps,
                minSteps: minSteps
            };
        }
    }

// /////////////////////////////////////////////////////////////////
// Pie implementation
// /////////////////////////////////////////////////////////////////

    var Pie = function(data,config,ctx) {

        var segmentTotal = 0;
        var pieRadius = Min([height/2,width/2]) - 5;

        // /////////////////////////////////////////////////////////////////
        // initialisation
        // /////////////////////////////////////////////////////////////////

        this.init = function () {

            for (var i=0; i<data.length; i++) {
                segmentTotal += data[i].value;
            }
        }

        // /////////////////////////////////////////////////////////////////
        // drawing
        // /////////////////////////////////////////////////////////////////

        this.draw = function (progress) {

            clear(ctx);

            drawPieSegments(progress);
        }

        // ///////////////////////////////////////////////////////////////

        function drawPieSegments (animationDecimal) {

            var cumulativeAngle = -Math.PI/2,

            scaleAnimation = 1,
            rotateAnimation = 1;

            if (config.animation) {
                if (config.animateScale) {
                    scaleAnimation = animationDecimal;
                }
                if (config.animateRotate) {
                    rotateAnimation = animationDecimal;
                }
            }

            for (var i=0; i<data.length; i++) {
                var segmentAngle = rotateAnimation * ((data[i].value/segmentTotal) * (Math.PI*2));
                ctx.beginPath();
                ctx.arc(width/2,height/2,scaleAnimation * pieRadius,cumulativeAngle,cumulativeAngle + segmentAngle);
                ctx.lineTo(width/2,height/2);
                ctx.closePath();
                ctx.fillStyle = data[i].color;
                ctx.fill();

                if(config.segmentShowStroke) {
                    ctx.lineWidth = config.segmentStrokeWidth;
                    ctx.strokeStyle = config.segmentStrokeColor;
                    ctx.stroke();
                }
                cumulativeAngle += segmentAngle;
            }
        }
    }

// /////////////////////////////////////////////////////////////////
// Doughnut implementation
// /////////////////////////////////////////////////////////////////

    var Doughnut = function(data,config,ctx) {

        var segmentTotal = 0;
        var doughnutRadius = Min([height/2,width/2]) - 5;
        var cutoutRadius = doughnutRadius * (config.percentageInnerCutout/100);

        // /////////////////////////////////////////////////////////////////
        // initialisation
        // /////////////////////////////////////////////////////////////////

        this.init = function () {

            for (var i=0; i<data.length; i++) {
                segmentTotal += data[i].value;
            }
        }

        // /////////////////////////////////////////////////////////////////
        // drawing
        // /////////////////////////////////////////////////////////////////

        this.draw = function (progress) {

            clear(ctx);

            drawDoughnutSegments(progress);
        }

        // ///////////////////////////////////////////////////////////////

        function drawDoughnutSegments (animationDecimal) {

            var cumulativeAngle = -Math.PI/2,

            scaleAnimation = 1,
            rotateAnimation = 1;

            if (config.animation) {
                if (config.animateScale) {
                    scaleAnimation = animationDecimal;
                }
                if (config.animateRotate) {
                    rotateAnimation = animationDecimal;
                }
            }

            for (var i=0; i<data.length; i++) {
                var segmentAngle = rotateAnimation * ((data[i].value/segmentTotal) * (Math.PI*2));
                ctx.beginPath();
                ctx.arc(width/2,height/2,scaleAnimation * doughnutRadius,cumulativeAngle,cumulativeAngle + segmentAngle,false);
                ctx.arc(width/2,height/2,scaleAnimation * cutoutRadius,cumulativeAngle + segmentAngle,cumulativeAngle,true);
                ctx.closePath();
                ctx.fillStyle = data[i].color;
                ctx.fill();

                if(config.segmentShowStroke) {
                    ctx.lineWidth = config.segmentStrokeWidth;
                    ctx.strokeStyle = config.segmentStrokeColor;
                    ctx.stroke();
                }
                cumulativeAngle += segmentAngle;
            }
        }
    }


// /////////////////////////////////////////////////////////////////
// Bar implementation
// /////////////////////////////////////////////////////////////////

    var Bar = function(data, config, ctx) {

        var maxSize;
        var scaleHop;
        var calculatedScale;
        var labelHeight;
        var scaleHeight;
        var valueBounds;
        var labelTemplateString;
        var valueHop;
        var widestXLabel;
        var xAxisLength;
        var yAxisPosX;
        var xAxisPosY;
        var barWidth;
        var rotateLabels = 0;

        // /////////////////////////////////////////////////////////////////
        // initialisation
        // /////////////////////////////////////////////////////////////////

        this.init = function () {

            calculateDrawingSizes();

            valueBounds = getValueBounds();

            labelTemplateString = (config.scaleShowLabels)? config.scaleLabel : "";

            if (!config.scaleOverride) {
                calculatedScale = calculateScale(scaleHeight,valueBounds.maxSteps,valueBounds.minSteps,valueBounds.maxValue,valueBounds.minValue,labelTemplateString);
            } else {
                calculatedScale = {
                    steps: config.scaleSteps,
                    stepValue: config.scaleStepWidth,
                    graphMin: config.scaleStartValue,
                    labels: []
                }
                populateLabels(labelTemplateString, calculatedScale.labels,calculatedScale.steps,config.scaleStartValue,config.scaleStepWidth);
            }

            scaleHop = Math.floor(scaleHeight/calculatedScale.steps);
            calculateXAxisSize();
        }

        // /////////////////////////////////////////////////////////////////
        // drawing
        // /////////////////////////////////////////////////////////////////

        this.draw = function (progress) {

            clear(ctx);

            if(config.scaleOverlay) {
                drawBars(progress);
                drawScale();
            } else {
                drawScale();
                drawBars(progress);
            }
        }

        // ///////////////////////////////////////////////////////////////

        function drawBars(animPc) {

            ctx.lineWidth = config.barStrokeWidth;

            for (var i=0; i<data.datasets.length; i++) {
                ctx.fillStyle = data.datasets[i].fillColor;
                ctx.strokeStyle = data.datasets[i].strokeColor;

                for (var j=0; j<data.datasets[i].data.length; j++) {

                    var barOffset = yAxisPosX + config.barValueSpacing + valueHop*j + barWidth*i + config.barDatasetSpacing*i + config.barStrokeWidth*i;

                    ctx.beginPath();
                    ctx.moveTo(barOffset, xAxisPosY);
                    ctx.lineTo(barOffset, xAxisPosY - animPc*calculateOffset(data.datasets[i].data[j],calculatedScale,scaleHop)+(config.barStrokeWidth/2));
                    ctx.lineTo(barOffset + barWidth, xAxisPosY - animPc*calculateOffset(data.datasets[i].data[j],calculatedScale,scaleHop)+(config.barStrokeWidth/2));
                    ctx.lineTo(barOffset + barWidth, xAxisPosY);
                    if(config.barShowStroke) {
                        ctx.stroke();
                    }
                    ctx.closePath();
                    ctx.fill();
                }
            }
        }

        function drawScale() {

            ctx.lineWidth = config.scaleLineWidth;
            ctx.strokeStyle = config.scaleLineColor;
            ctx.beginPath();
            ctx.moveTo(width-widestXLabel/2+5,xAxisPosY);
            ctx.lineTo(width-(widestXLabel/2)-xAxisLength-5,xAxisPosY);
            ctx.stroke();
            if (rotateLabels > 0) {
                ctx.save();
                ctx.textAlign = "right";
            } else{
                ctx.textAlign = "center";
            }

            ctx.fillStyle = config.scaleFontColor;

            for (var i=0; i<data.labels.length; i++) {
                ctx.save();
                if (rotateLabels > 0) {
                    ctx.translate(yAxisPosX + i*valueHop,xAxisPosY + config.scaleFontSize);
                    ctx.rotate(-(rotateLabels * (Math.PI/180)));
                    ctx.fillText(data.labels[i], 0,0);
                    ctx.restore();
                } else {
                    ctx.fillText(data.labels[i], yAxisPosX + i*valueHop + valueHop/2,xAxisPosY + config.scaleFontSize+3);
                }

                ctx.beginPath();
                ctx.moveTo(yAxisPosX + (i+1) * valueHop, xAxisPosY+3);
                ctx.lineWidth = config.scaleGridLineWidth;
                ctx.strokeStyle = config.scaleGridLineColor;
                ctx.lineTo(yAxisPosX + (i+1) * valueHop, 5);
                ctx.stroke();
            }

            ctx.lineWidth = config.scaleLineWidth;
            ctx.strokeStyle = config.scaleLineColor;
            ctx.beginPath();
            ctx.moveTo(yAxisPosX,xAxisPosY+5);
            ctx.lineTo(yAxisPosX,5);
            ctx.stroke();
            ctx.textAlign = "right";
            ctx.textBaseline = "middle";

            for (var j=0; j<calculatedScale.steps; j++) {
                ctx.beginPath();
                ctx.moveTo(yAxisPosX-3,xAxisPosY - ((j+1) * scaleHop));
                if (config.scaleShowGridLines) {
                    ctx.lineWidth = config.scaleGridLineWidth;
                    ctx.strokeStyle = config.scaleGridLineColor;
                    ctx.lineTo(yAxisPosX + xAxisLength + 5,xAxisPosY - ((j+1) * scaleHop));
                } else {
                    ctx.lineTo(yAxisPosX-0.5,xAxisPosY - ((j+1) * scaleHop));
                }
                ctx.stroke();
                if (config.scaleShowLabels) {
                    ctx.fillText(calculatedScale.labels[j],yAxisPosX-8,xAxisPosY - ((j+1) * scaleHop));
                }
            }
        }

        function calculateXAxisSize() {

            var longestText = 1;

            if (config.scaleShowLabels) {

                ctx.font = config.scaleFontStyle + " " + config.scaleFontSize+"px " + config.scaleFontFamily;

                for (var i=0; i<calculatedScale.labels.length; i++) {
                    var measuredText = ctx.measureText(calculatedScale.labels[i]).width;
                    longestText = (measuredText > longestText)? measuredText : longestText;
                }

                longestText +=10;
            }

            xAxisLength = width - longestText - widestXLabel;
            valueHop = Math.floor(xAxisLength/(data.labels.length));

            barWidth = (valueHop - config.scaleGridLineWidth*2 - (config.barValueSpacing*2) - (config.barDatasetSpacing*data.datasets.length-1) - ((config.barStrokeWidth/2)*data.datasets.length-1))/data.datasets.length;

            yAxisPosX = width-widestXLabel/2-xAxisLength;
            xAxisPosY = scaleHeight + config.scaleFontSize/2;
        }

        function calculateDrawingSizes() {

            maxSize = height;
            ctx.font = config.scaleFontStyle + " " + config.scaleFontSize+"px " + config.scaleFontFamily;
            widestXLabel = 1;

            for (var i=0; i<data.labels.length; i++) {
                var textLength = ctx.measureText(data.labels[i]).width;
                widestXLabel = (textLength > widestXLabel)? textLength : widestXLabel;
            }

            if (width/data.labels.length < widestXLabel) {
                rotateLabels = 45;
                if (width/data.labels.length < Math.cos(rotateLabels) * widestXLabel) {
                    rotateLabels = 90;
                    maxSize -= widestXLabel;
                } else{
                    maxSize -= Math.sin(rotateLabels) * widestXLabel;
                }
            } else{
                maxSize -= config.scaleFontSize;
            }

            maxSize -= 5;

            labelHeight = config.scaleFontSize;

            maxSize -= labelHeight;

            scaleHeight = maxSize;
        }

        function getValueBounds() {

            var upperValue = Number.MIN_VALUE;
            var lowerValue = Number.MAX_VALUE;

            for (var i=0; i<data.datasets.length; i++) {
                for (var j=0; j<data.datasets[i].data.length; j++) {
                    if ( data.datasets[i].data[j] > upperValue) { upperValue = data.datasets[i].data[j] };
                    if ( data.datasets[i].data[j] < lowerValue) { lowerValue = data.datasets[i].data[j] };
                }
            };

            var maxSteps = Math.floor((scaleHeight / (labelHeight*0.66)));
            var minSteps = Math.floor((scaleHeight / labelHeight*0.5));

            return {
                maxValue: upperValue,
                minValue: lowerValue,
                maxSteps: maxSteps,
                minSteps: minSteps
            };
        }
    }

// /////////////////////////////////////////////////////////////////
// Helper functions
// /////////////////////////////////////////////////////////////////

    var clear = function(c) {
        c.clearRect(0, 0, width, height);
    };


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function calculateOffset(val,calculatedScale,scaleHop)
    {
        var outerValue = calculatedScale.steps * calculatedScale.stepValue;
        var adjustedValue = val - calculatedScale.graphMin;
        // INLINE : var scalingFactor = CapValue(adjustedValue/outerValue,1,0);
        var tmp=adjustedValue/outerValue
        if( tmp > 1 ) return 1//(scaleHop*calculatedScale.steps)*maxValue;
        if( tmp < 0 ) return 0//(scaleHop*calculatedScale.steps)*minValue;
        // end INLINE
        return  (scaleHop*(val - calculatedScale.graphMin)/calculatedScale.stepValue)
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function calculateScale(drawingHeight,maxSteps,minSteps,maxValue,minValue,labelTemplateString)
    {
        var graphMin,graphMax,graphRange,stepValue,numberOfSteps,valueRange,rangeOrderOfMagnitude,decimalNum;

        if (maxValue === minValue)
        {
            if(maxValue===0)
                {maxValue+=1}
            else
                {maxValue+=maxValue/10;minValue-=minValue/10}
        }// GRO
        valueRange = maxValue - minValue;
        rangeOrderOfMagnitude = Math.floor(Math.log(valueRange) / Math.LN10)
        //calculateOrderOfMagnitude(valueRange);
        stepValue = Math.pow(10, rangeOrderOfMagnitude)
        graphMin = Math.floor(minValue / (1 * stepValue)) * stepValue;
        graphMax = Math.ceil(maxValue / (1 * stepValue)) * stepValue;
        graphRange = graphMax - graphMin;
        numberOfSteps = Math.round(graphRange / stepValue);

        // BUGFIX : issue with resize of plotter Popup
        if( maxSteps <= minSteps ){ maxSteps = minSteps + 1; }
        while(numberOfSteps < minSteps || numberOfSteps > maxSteps)
        {
            if (numberOfSteps < minSteps)
            {
                stepValue /= 2;
                numberOfSteps = Math.round(graphRange/stepValue);
                if (numberOfSteps>maxSteps){numberOfSteps=maxSteps;break;}
            }
            else
            {
                stepValue *=2;
                numberOfSteps = Math.round(graphRange/stepValue);
                if (numberOfSteps<minSteps){numberOfSteps=minSteps;break;}
            }
        };

        var labels = [];

        populateLabels(labelTemplateString, labels, numberOfSteps, graphMin, stepValue);

        return {
            steps: numberOfSteps,
            stepValue: stepValue,
            graphMin: graphMin,
            labels: labels
        }

        function calculateOrderOfMagnitude(val)
        {
            return Math.floor(Math.log(val) / Math.LN10);
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function populateLabels(labelTemplateString, labels, numberOfSteps, graphMin, stepValue)
    {
        if (labelTemplateString) {
            for (var i = 1; i < numberOfSteps + 1; i++)
            {
                var value=graphMin + (stepValue * i)
                if(value < 1e4 && value > -1e4)
                    labels.push(value.toFixed(getDecimalPlaces(stepValue)))
                else
                    labels.push(value.toExponential(2))
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function Max(array) {
        return Math.max.apply(Math, array);
    };
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function Min(array) {
        return Math.min.apply(Math, array);
    };
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function Default(userDeclared,valueIfFalse) {
        if(!userDeclared) {
            return valueIfFalse;
        } else {
            return userDeclared;
        }
    };
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function isNumber(n) {
        return !isNaN(parseFloat(n)) && isFinite(n);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function CapValue(valueToCap, maxValue, minValue) {
        if(isNumber(maxValue)) {
            if( valueToCap > maxValue ) {
                return maxValue;
            }
        }
        if(isNumber(minValue)) {
            if ( valueToCap < minValue ) {
                return minValue;
            }
        }
        return valueToCap;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function getDecimalPlaces (num) {
        var numberOfDecimalPlaces;
        if (num%1!=0 && num.toString().split(".")[1]!==undefined) {
            return num.toString().split(".")[1].length
        } else {
            return 0;
        }
    }

    function mergeChartConfig(defaults,userDefined) {
        var returnObj = {};
        for (var attrname in defaults) { returnObj[attrname] = defaults[attrname]; }
        for (var attrname in userDefined) { returnObj[attrname] = userDefined[attrname]; }
        return returnObj;
    }

    var cache = {};

    function tmpl(str, data) {
        var fn = !/\W/.test(str) ?
            cache[str] = cache[str] ||
            tmpl(document.getElementById(str).innerHTML) :

        new Function("obj",
                     "var p=[],print=function() {p.push.apply(p,arguments);};" +
                     "with(obj) {p.push('" +
                     str
                     .replace(/[\r\t\n]/g, " ")
                     .split("<%").join("\t")
                     .replace(/((^|%>)[^\t]*)'/g, "$1\r")
                     .replace(/\t=(.*?)%>/g, "',$1,'")
                     .split("\t").join("');")
                     .split("%>").join("p.push('")
                     .split("\r").join("\\'")
                     + "');}return p.join('');");

        return data ? fn( data ) : fn;
    };
}

// /////////////////////////////////////////////////////////////////
// Credits
// /////////////////////////////////////////////////////////////////

/*!
 * Chart.js
 * http://chartjs.org/
 *
 * Copyright 2013 Nick Downie
 * Released under the MIT license
 * https://github.com/nnnick/Chart.js/blob/master/LICENSE.md
 */

// Copyright (c) 2013 Nick Downie

// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
