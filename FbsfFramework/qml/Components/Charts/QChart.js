
var ChartType = { LINE: 1 };

var Chart = function(canvas, context) {

    var chart = this;

// /////////////////////////////////////////////////////////////////
// Line helper
// /////////////////////////////////////////////////////////////////

    this.Line = function(data,options) {

        chart.Line.defaults = {
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
                 gradient.addColorStop(0, Qt.lighter("teal", 1.2));
                 gradient.addColorStop(1, Qt.lighter("teal", 0.8));
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
            this.xAxisInfo = function () {
                return {width:xAxisLength,xStart:yAxisPosX};
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

                        if (config.steppedLine) {
                            var previousX=yAxisPosX + (valueHop * (j-1));
                            var tmp1=(dataset.data[j-1] - calculatedScale.graphMin)/calculatedScale.stepValue
                            var previousY=xAxisPosY - (scaleHop*tmp1)
                            var midpoint = (previousX + x) / 2.0
                            ctx.lineTo(midpoint, previousY)
                            ctx.lineTo(midpoint, y);
                            ctx.lineTo(x,y);
                        }
                        else if (config.bezierCurve) {
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
                        if ( value > upperValue) { upperValue = value };
                        if ( value < lowerValue) { lowerValue = value };
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
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Helper functions
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    function mergeChartConfig(defaults,userDefined) {
        var returnObj = {};
        for (var attrname in defaults) { returnObj[attrname] = defaults[attrname]; }
        for (var attrname in userDefined) { returnObj[attrname] = userDefined[attrname]; }
        return returnObj;
    }
}

