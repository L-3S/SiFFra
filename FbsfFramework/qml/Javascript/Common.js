.pragma library

var formatDateUTC=false
var startTime=undefined
var currentTime

function setFormatDateUTC(time)
{
    formatDateUTC=true
    startTime=time
    currentTime=new Date()
}
function getStartTime()
{
    return new Date(startTime);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function to format time (H:M:S[.MS]) (time : real in seconds)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
var msdisplay=false
function ms2TimeString(time)
{
    time=Math.round(time*1000)
    var k=Math.round(time%1e3)
    var s=time/1e3%60|0
    var m=time/6e4%60|0
    var h=time/36e5|0//%24|0
    if(msdisplay==false&&k>0) {msdisplay=true}
    var str=(h<10?'0'+h:h)+':'+
            (m<10?'0'+m:m)+':'+
            (s<10?'0'+s:s)+
            (k?(k<100?k<10?'.00':'.0':'.'+k):(msdisplay?'.000':''))
    return str
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Function to convert formatted time (H.M.S.MS)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
function timeString2ms(a)
{
  a=a.split('.')
  var b=a[1]*1||0
  a=a[0].split(':')
  return b+(a[2]?a[0]*3600+a[1]*60+a[2]*1:a[1]?a[0]*60+a[1]*1:a[0]*1)*1e3
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// function to format time to string
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
function getTime(time)
{
    return ms2TimeString(time)
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// function to format time to date string
// TODO check time resolution
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
var lastTime    = 0         // to test for seconds
function getDateStr(time)
{
    if (formatDateUTC===false)
    {
        // elapsed time from 0
        return ms2TimeString(time)
    }
    else
    {
        // UTC time from startTime
        var datestring;
        var delta=time-lastTime
        var showSeconds = true
        var showMinutes = true
        var showHours   = true

        if (delta>0 && delta >= 86400)  showHours=false
        if (delta>0 && delta >= 3600)   showMinutes=false
        if (delta>0 && delta >= 60)     showSeconds=false

        lastTime=time
        if(startTime!==undefined)
        {
            var s = startTime.getTime() + time*1000;// add time ms
            currentTime.setTime( s );
        }
        else
        {
            currentTime.setTime(time*1000)
        }

        var hours   =(showHours  ?(("0"+        currentTime.getHours()).slice(-2)):"")
        var minutes =(showMinutes?(":" + ("0" + currentTime.getMinutes()).slice(-2)):"")
        var seconds =(showSeconds?(":" + ("0" + currentTime.getSeconds()).slice(-2)):"")

        datestring = ("0" + currentTime.getDate()).slice(-2) + "-"
                + ("0"    +(currentTime.getMonth()+1)).slice(-2) + "-"
                + currentTime.getFullYear() + " "
                + hours
                + minutes
                + seconds
    }
    return datestring
}

function messageBox(type,text)
{
    var component = Qt.createComponent("../Components/Dialog.qml");

    var messageBox = component.createObject(null,{"title":type,"text":text})
}
