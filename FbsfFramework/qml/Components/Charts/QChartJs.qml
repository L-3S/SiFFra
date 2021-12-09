import QtQuick 2.0

import "QChart.js" as Charts
import "qrc:/qml/Javascript/Common.js" as Util

Canvas {
  id                : canvas;
  anchors.fill      : parent
  antialiasing      : true
  //renderStrategy    : Canvas.Threaded
  renderTarget      : Canvas.FramebufferObject
  // Use a large amount of memory on Windows 10 but Threaded is slow
  renderStrategy    : Canvas.Cooperative
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  property   var    chart;
  property   var    chartData;
  property   int    chartType              : 0;

  property   var    chartOptions: ({})

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Callbacks
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  onPaint: {
      var ctx = canvas.getContext("2d");
      /* Reset the canvas context to allow resize events to properly redraw
         the surface with an updated window size */
      ctx.reset()

      switch(chartType) {
      case Charts.ChartType.LINE:
          chart = new Charts.Chart(canvas, ctx).Line(chartData, chartOptions);
          break;

//      case Charts.ChartType.BAR:
//          chart = new Charts.Chart(canvas, ctx).Bar(chartData, chartOptions);
//          break;
//      case Charts.ChartType.DOUGHNUT:
//          chart = new Charts.Chart(canvas, ctx).Doughnut(chartData, chartOptions);
//          break;
//      case Charts.ChartType.PIE:
//          chart = new Charts.Chart(canvas, ctx).Pie(chartData, chartOptions);
//          break;
//      case Charts.ChartType.POLAR:
//          chart = new Charts.Chart(canvas, ctx).PolarArea(chartData, chartOptions);
//          break;
//      case Charts.ChartType.RADAR:
//          chart = new Charts.Chart(canvas, ctx).Radar(chartData, chartOptions);
//          break;
      default:
          console.log('Chart type should be specified.');
      }
      chart.draw();
      if (rectWidth && rectHeight)
      {
            context.strokeStyle = 'black';
            context.strokeRect(rectStartx,rectStarty,rectWidth,rectHeight);
      }
  }

  onChartDataChanged: {      requestPaint();  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Function for redraw
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  function repaint()
  {
      requestPaint();
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Function for ghost rectangle
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  property int   rate : 0

  property int rectStartx:0
  property int rectStarty:0
  property int rectWidth:0
  property int rectHeight:0

  function drawRect(sx,sy,ex,ey)
  {
      rectStartx=sx
      rectStarty=sy
      rectWidth=ex-sx
      rectHeight=ey-sy
      if (rate++%8==0)   requestPaint()
  }
  // proxy for QChart.js
  function getDateStr(time)
  {
      // get display date from time manager
      return FbsfTimeManager.dateTimeStr(time)  }
}
