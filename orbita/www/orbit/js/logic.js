// global variable here
var status=0;
var ticCnt=0;
var defaultLocale = "en-US";
//var defaultLocale = "ru-RU";
//object from translations.js
var localizator = translations[defaultLocale]; 
var expireMess = -1; // expire webix message on screen Forever
var expireTemp = 5000; //temperarry message on screen

var scales=[10,10,5,2.5,1];
var grLim=50; // limits of graph
var grScale=10; // axe scale

// get call function name and return with error
function gFN(){
return("\n\rError in function: "+gFN.caller.name);
};

// catch close window event on browser
function reqestOnTryClose(e) {
  e = e || window.event;
// For IE and Firefox prior to version 4
  if (e) {
    e.returnValue = 'Sure?';
  }
// For Safari
  return 'Sure?';
};

// send stop request into server on exit from broser window  (delete lock file...)
function reqestOnQuit(){
  webix.ajax().sync().post("./data/stop.php", { status:"stop"});
  var ms=500; // wait some time
  ms += new Date().getTime();
  while (new Date() < ms){};
//  return reqestOnTryClose();
}

// Access to programm.
/*function access() {
  //return (1);         // remove!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  var lock=0;
  var xhr=webix.ajax().sync().post("./data/start.php"); // synchronous access. Get promis data
  if(xhr.response=="null") return(0);
  lock=JSON.parse(xhr.response)[0].status;
  if(lock==2) { // node is busy
    alert("\nCan't working with program VPSCP.\n"+JSON.parse(xhr.response)[0].lock_ip+"\nClose program on remote IP and try again.");
	return(0);
  }
  return (lock);
}
*/

// Access to programm.
function access() {
  //return (1);         // remove!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  var lock=0;
  var result=webix.ajax().sync().post("./data/start.php"); // synchronous access. Get promis data
  
  if(result.response=="null") return(0);
  var xhr=JSON.parse(result.responseText); 
  lock=xhr[0].status;
  if(lock==2) { // node is busy
    alert("\nCan't working with program ORBIT.\n"+xhr[0].lock_ip+"\nClose program on remote IP and try again.");
    console.log("Error");
    return(0);
  }
  return (lock);
}

// Plot measured data. Call from getStatus function
function plotMeasuredData(){
  webix.ajax().post("./data/getorb.php",{filename:"./ctrl/orbitr.dat",delfile:0}).then( function (result) {
    $$("chartOrbR").clearAll();
    var meas=result.json();
    $$("chartOrbR").parse(meas);
//    $$("chartOrbR").define({ yAxis:{start:-50,end:50,step:10,title:"Position, mm"}});
    $$("chartOrbR").define({ yAxis:{start:-grLim,end:grLim,step:grScale,title:"Position, mm"}});

    $$("chartOrbR").define( {series:[
                                    {value:"#t1#", line:{color:"#000000",width:2}}, {value:"#t2#",line: {color:"#0A6CE4",width:2}},
                                    {value:"#t3#", line:{color:"#200AE4",width:2}}, {value:"#t4#",line: {color:"#9F0AE4",width:2}},
                                    {value:"#t5#", line:{color:"#8A05E9",width:2}}, {value:"#t6#",line: {color:"#C1009E",width:2}},
                                    {value:"#t7#", line:{color:"#05E977",width:2}}, {value:"#t9#",line: {color:"#0AE4D2",width:2}},
                                    {value:"#t9#", line:{color:"#0AE486",width:2}}, {value:"#t10#",line:{color:"#0EE40A",width:2}},
                                    {value:"#t11#",line:{color:"#ACE905",width:2}}, {value:"#t12#",line:{color:"#E9DA05",width:2}},
                                    {value:"#t13#",line:{color:"#6daa2c",width:2}}, {value:"#t14#",line:{color:"#E97405",width:2}},
                                    {value:"#t15#",line:{color:"#E94A05",width:2}}, {value:"#t16#",line:{color:"#E90505",width:2}}
                                    ] } );
    $$("chartOrbR").refresh();
  });
  
  webix.ajax().post("./data/getharm.php",{filename:"./ctrl/harmr.dat",delfile:0}).then( function (result) {
    $$("chartHarmR").clearAll();
    meas=result.json();
    $$("chartHarmR").parse(meas);
    //$$("chartHarmR").define({ yAxis:{start:-50,end:50,step:10,title:"Position, mm"}});
    $$("chartHarmR").define({ yAxis:{start:-grLim,end:grLim,step:grScale,title:"Position, mm"}});
    $$("chartHarmR").define( {series:[
                                    {value:"#sin9#", line:{color:"#000000",width:2}}, {value:"#cos9#",line: {color:"#0A6CE4",width:2}},
                                    {value:"#sin10#", line:{color:"#200AE4",width:2}}, {value:"#cos10#",line: {color:"#9F0AE4",width:2}},
                                    {value:"#sin11#", line:{color:"#E90505",width:2}}, {value:"#cos11#",line: {color:"#E97405",width:2}},
                                    {value:"#avr#", line:{color:"#05E977",width:2}},{value:"#rms#", line:{color:"#ACE905",width:2}}
                                    ] } );
    $$("chartHarmR").refresh();
  });
  
  webix.ajax().post("./data/getorb.php",{filename:"./ctrl/orbitz.dat",delfile:0}).then( function (result) {
    $$("chartOrbZ").clearAll();
    meas=result.json();
    $$("chartOrbZ").parse(meas);
    //$$("chartOrbZ").define({ yAxis:{start:-50,end:50,step:10,title:"Position, mm"}});
    $$("chartOrbZ").define({ yAxis:{start:-grLim,end:grLim,step:grScale,title:"Position, mm"}});
    $$("chartOrbZ").define( {series:[
                                   {value:"#t1#", line:{color:"#000000",width:2}}, {value:"#t2#",line: {color:"#0A6CE4",width:2}},
                                    {value:"#t3#", line:{color:"#200AE4",width:2}}, {value:"#t4#",line: {color:"#9F0AE4",width:2}},
                                    {value:"#t5#", line:{color:"#8A05E9",width:2}}, {value:"#t6#",line: {color:"#C1009E",width:2}},
                                    {value:"#t7#", line:{color:"#05E977",width:2}}, {value:"#t9#",line: {color:"#0AE4D2",width:2}},
                                    {value:"#t9#", line:{color:"#0AE486",width:2}}, {value:"#t10#",line:{color:"#0EE40A",width:2}},
                                    {value:"#t11#",line:{color:"#ACE905",width:2}}, {value:"#t12#",line:{color:"#E9DA05",width:2}},
                                    {value:"#t13#",line:{color:"#6daa2c",width:2}}, {value:"#t14#",line:{color:"#E97405",width:2}},
                                    {value:"#t15#",line:{color:"#E94A05",width:2}}, {value:"#t16#",line:{color:"#E90505",width:2}}
                                    ] } );
    $$("chartOrbZ").refresh();
  });
  
  webix.ajax().post("./data/getharm.php",{filename:"./ctrl/harmz.dat",delfile:0}).then( function (result) {
    $$("chartHarmZ").clearAll();
    meas=result.json();
    $$("chartHarmZ").parse(meas);
    //$$("chartHarmZ").define({ yAxis:{start:-50,end:50,step:10,title:"Position, mm"}});
    $$("chartHarmZ").define({ yAxis:{start:-grLim,end:grLim,step:grScale,title:"Position, mm"}});
    $$("chartHarmZ").define( {series:[
                                    {value:"#sin9#", line:{color:"#000000",width:2}}, {value:"#cos9#",line: {color:"#0A6CE4",width:2}},
                                    {value:"#sin10#", line:{color:"#200AE4",width:2}}, {value:"#cos10#",line: {color:"#9F0AE4",width:2}},
                                    {value:"#sin11#", line:{color:"#E90505",width:2}}, {value:"#cos11#",line: {color:"#E97405",width:2}},
                                    {value:"#avr#", line:{color:"#05E977",width:2}},{value:"#rms#", line:{color:"#ACE905",width:2}}
                                    ] } );
    $$("chartHarmZ").refresh();
  });
// get table orbit R
  webix.ajax().post("./data/getorbtxt.php",{filename:"./ctrl/orbitr.dat",delfile:0}).then( function (result) {
    $$("tableOrbR").clearAll();
    meas=result.json();
    $$("tableOrbR").parse(meas);
    //$$("tableOrbR").showItemByIndex(meas[meas.length-1].id);
    $$("tableOrbR").refresh();
  });
// get table harm R
  webix.ajax().post("./data/getharmtxt.php",{filename:"./ctrl/harmr.dat",delfile:0}).then( function (result) {
    $$("tableHarmR").clearAll();
    meas=result.json();
    $$("tableHarmR").parse(meas);
    //$$("tableHarmR").showItemByIndex(meas[meas.length-1].id);
    $$("tableHarmR").refresh();
  });

// get table orbit Z
  webix.ajax().post("./data/getorbtxt.php",{filename:"./ctrl/orbitz.dat",delfile:0}).then( function (result) {
    $$("tableOrbZ").clearAll();
    meas=result.json();
    $$("tableOrbZ").parse(meas);
    //$$("tableOrbZ").showItemByIndex(meas[meas.length-1].id);
    $$("tableOrbZ").refresh();
  });

// get table harm Z
  webix.ajax().post("./data/getharmtxt.php",{filename:"./ctrl/harmz.dat",delfile:0}).then( function (result) {
    $$("tableHarmZ").clearAll();
    meas=result.json();
    $$("tableHarmZ").parse(meas);
    //$$("tableHarmZ").showItemByIndex(meas[meas.length-1].id);
    $$("tableHarmZ").refresh();
  });
}

// Change tabs logic
function changeTab(id){
  if(id=="t1"){
    //$$("buttonLoad").enable();
  }
  if(id=="t2"){
    //$$("buttonLoad").disable();
  }
  if(id=="t3"){
    //$$("buttonLoad").disable();
  }
  if(id=="t4"){
    //$$("buttonLoad").disable();
  }
}

// Events on buttons clicks
// Refresh request
function refreshData(){
  getStatus();
}

// Visualisation of status on form
function setStatus(obj){
  $$("Upheader").parse(obj);
  $$("Upheader").refresh(obj);

  $$("status").parse(obj);
  $$("status").refresh();

  $$("statusSt").parse(obj);
  $$("statusSt").refresh();

  $$("statusDT").parse(obj);
  $$("statusDT").refresh();


  $$("chartOrbR").config.legend.values[0].text=obj[0].T1;
  $$("chartOrbR").config.legend.values[1].text=obj[0].T2;
  $$("chartOrbR").config.legend.values[2].text=obj[0].T3;
  $$("chartOrbR").config.legend.values[3].text=obj[0].T4;
  $$("chartOrbR").config.legend.values[4].text=obj[0].T5;
  $$("chartOrbR").config.legend.values[5].text=obj[0].T6;
  $$("chartOrbR").config.legend.values[6].text=obj[0].T7;
  $$("chartOrbR").config.legend.values[7].text=obj[0].T8;
  $$("chartOrbR").config.legend.values[8].text=obj[0].T9;
  $$("chartOrbR").config.legend.values[9].text=obj[0].T10;
  $$("chartOrbR").config.legend.values[10].text=obj[0].T11;
  $$("chartOrbR").config.legend.values[11].text=obj[0].T12;
  $$("chartOrbR").config.legend.values[12].text=obj[0].T13;
  $$("chartOrbR").config.legend.values[13].text=obj[0].T14;
  $$("chartOrbR").config.legend.values[14].text=obj[0].T15;
  $$("chartOrbR").config.legend.values[15].text=obj[0].T16;

  $$("chartOrbZ").config.legend.values[0].text=obj[0].T1;
  $$("chartOrbZ").config.legend.values[1].text=obj[0].T2;
  $$("chartOrbZ").config.legend.values[2].text=obj[0].T3;
  $$("chartOrbZ").config.legend.values[3].text=obj[0].T4;
  $$("chartOrbZ").config.legend.values[4].text=obj[0].T5;
  $$("chartOrbZ").config.legend.values[5].text=obj[0].T6;
  $$("chartOrbZ").config.legend.values[6].text=obj[0].T7;
  $$("chartOrbZ").config.legend.values[7].text=obj[0].T8;
  $$("chartOrbZ").config.legend.values[8].text=obj[0].T9;
  $$("chartOrbZ").config.legend.values[9].text=obj[0].T10;
  $$("chartOrbZ").config.legend.values[10].text=obj[0].T11;
  $$("chartOrbZ").config.legend.values[11].text=obj[0].T12;
  $$("chartOrbZ").config.legend.values[12].text=obj[0].T13;
  $$("chartOrbZ").config.legend.values[13].text=obj[0].T14;
  $$("chartOrbZ").config.legend.values[14].text=obj[0].T15;
  $$("chartOrbZ").config.legend.values[15].text=obj[0].T16;

  $$("tableOrbR").config.columns[1].header=obj[0].T1+localizator.lex_ms;
  $$("tableOrbR").config.columns[2].header=obj[0].T2+localizator.lex_ms;
  $$("tableOrbR").config.columns[3].header=obj[0].T3+localizator.lex_ms;
  $$("tableOrbR").config.columns[4].header=obj[0].T4+localizator.lex_ms;
  $$("tableOrbR").config.columns[5].header=obj[0].T5+localizator.lex_ms;
  $$("tableOrbR").config.columns[6].header=obj[0].T6+localizator.lex_ms;
  $$("tableOrbR").config.columns[7].header=obj[0].T7+localizator.lex_ms;
  $$("tableOrbR").config.columns[8].header=obj[0].T8+localizator.lex_ms;
  $$("tableOrbR").config.columns[9].header=obj[0].T9+localizator.lex_ms;
  $$("tableOrbR").config.columns[10].header=obj[0].T10+localizator.lex_ms;
  $$("tableOrbR").config.columns[11].header=obj[0].T11+localizator.lex_ms;
  $$("tableOrbR").config.columns[12].header=obj[0].T12+localizator.lex_ms;
  $$("tableOrbR").config.columns[13].header=obj[0].T13+localizator.lex_ms;
  $$("tableOrbR").config.columns[14].header=obj[0].T14+localizator.lex_ms;
  $$("tableOrbR").config.columns[15].header=obj[0].T15+localizator.lex_ms;
  $$("tableOrbR").config.columns[16].header=obj[0].T16+localizator.lex_ms;

  $$("tableOrbZ").config.columns[1].header=obj[0].T1+localizator.lex_ms;
  $$("tableOrbZ").config.columns[2].header=obj[0].T2+localizator.lex_ms;
  $$("tableOrbZ").config.columns[3].header=obj[0].T3+localizator.lex_ms;
  $$("tableOrbZ").config.columns[4].header=obj[0].T4+localizator.lex_ms;
  $$("tableOrbZ").config.columns[5].header=obj[0].T5+localizator.lex_ms;
  $$("tableOrbZ").config.columns[6].header=obj[0].T6+localizator.lex_ms;
  $$("tableOrbZ").config.columns[7].header=obj[0].T7+localizator.lex_ms;
  $$("tableOrbZ").config.columns[8].header=obj[0].T8+localizator.lex_ms;
  $$("tableOrbZ").config.columns[9].header=obj[0].T9+localizator.lex_ms;
  $$("tableOrbZ").config.columns[10].header=obj[0].T10+localizator.lex_ms;
  $$("tableOrbZ").config.columns[11].header=obj[0].T11+localizator.lex_ms;
  $$("tableOrbZ").config.columns[12].header=obj[0].T12+localizator.lex_ms;
  $$("tableOrbZ").config.columns[13].header=obj[0].T13+localizator.lex_ms;
  $$("tableOrbZ").config.columns[14].header=obj[0].T14+localizator.lex_ms;
  $$("tableOrbZ").config.columns[15].header=obj[0].T15+localizator.lex_ms;
  $$("tableOrbZ").config.columns[16].header=obj[0].T16+localizator.lex_ms;

  $$("tableOrbR").refreshColumns();
  $$("tableOrbZ").refreshColumns();
}

// function onChange graph limits
function changeLimits(newVal,oldVal){
 grLim=Number($$("setLimits").getText());
 grScale=scales[Number($$("setLimits").getValue())-1];
}

// Get status from server
function getStatus() {
  ticCnt++;
  webix.ajax().post("./data/getstatus.php").then( function (result){ //tutu
    if(result.text().length!=0){
      var stat=result.json();
      ticCnt=0;
      var oldNewStat=[{'oldNew':"old"}];
      if(stat[0].update) {        
        $$("dataFreshHeader").parse([{'oldnew':"new data"}]);
        $$("dataFreshHeader").refresh();
        setStatus(stat);
	plotMeasuredData();
      }
      else {
        $$("dataFreshHeader").parse([{'oldnew':"old data"}]);
        $$("dataFreshHeader").refresh();
      }
    }
    else {
      stat=[ {'status':"unknown", 'start':"unknown", 'datetime':"unknown",'update':0  } ];
      if(ticCnt>15){
	if(ticCnt==16) {
	  //stat[0].power="unknown";
	  stat[0].error=localizator.lex_StatusDWP;//"Control program don't work property.";
	  ticCnt=15;
          plotMeasuredData();
        }
      }
      setStatus(stat);
    }
  }).fail(function getStatus(result){
    var stat=[ {'status':"unknown", 'start':"unknown", 'datetime':"unknown",'update':0  } ]; setStatus(stat);
    webix.message({type:"error",text:"Error"+gFN(),expire:expireTemp});
  });
}

var logic = {
init: function(){
// set send stop request on close window event
//  window.onunload=reqestOnQuit; 
// catch close event on close browser
//  window.onbeforeunload =reqestOnTryClose;
//  ticCnt=0;
//  setInterval(function(){getStatus()}, 4000);
//  webix.ui(ui_top);

  if(access())
  {
// set send stop request on close window event
    window.onunload=reqestOnQuit;
// catch close event on close browser
    window.onbeforeunload =reqestOnTryClose;
    ticCnt=0;
    setInterval(function(){getStatus()}, 4000);
    webix.ui(ui_top);
  }
  else
    window.history.back();
}
}
//------------------------------------------------- END ----------------------------------------------
// trucs
/*
interst hints
webix message: 
webix.alert("error");
webix.message("blabla");
webix.message({
   type:"", 
   text:"blablabla",
   expire:2000
   //expire:-1   //for canceling expire period
});
  make timer:
  setInterval(function(){funccall()}, 1500);
  
  output into console:
  console.log("sdsds");
  
  // load data from ext php
  $$("statusValue").load("file.php",{
    error: function(text, data, http_request){
      webix.alert("error");
    },
    success:function(text, data, http_request){
      webix.message(text);
    }
  });
  
*/
