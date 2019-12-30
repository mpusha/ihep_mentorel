// global variable here
var status=0;
var ticCnt=0;
//var defaultLocale = "en-US";
var defaultLocale = "ru-RU";
// object from translations.js
var localizator = translations[defaultLocale]; 
var expireMess = -1; // expire webix message on screen Forever
var expireTemp = 5000; //temperarry message on screen
// get call function name and return with error
function gFN(){
return("\n\rError in function: "+gFN.caller.name);
};

// catch close window event oin browser
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
  webix.ajax().sync().post("./data/stop.php", { status:"stop"});;
  var ms=200; // wait some time
  ms += new Date().getTime();
  while (new Date() < ms){};
}

// Access to programm. 
function access() { 
//  return (1);         // remove!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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

// Plot measured data. Call from getStatus function
function plotMeasuredData(){
  webix.ajax().post("./data/csvtojson.php",{filename:"./ctrl/meas.dat",delfile:1}).then( function (result) {
    $$("graphPsDataMeas").clearAll();
	var meas=result.json();
	var mi=meas[meas.length-1].min;
	var ma=meas[meas.length-1].max;
	var st=meas[meas.length-1].step;
	meas.pop(); // remove last data min max step
    $$("graphPsDataMeas").parse(meas); 
	$$("graphPsDataMeas").define({ yAxis:{start:mi,end:ma,step:st,title:"I,A"}});
	$$("graphPsDataMeas").define( {series:[ {value:"#y1#",line:{color:"#1293f8",width:2}}, {value:"#y2#",line:{color:"#66cc00",width:2}} ] } );
    $$("graphPsDataMeas").refresh();
  }).fail( function plotMeasuredData(result){
    $$("graphPsDataMeas").clearAll();
    data=testdata;
    $$("graphPsDataMeas").refresh();
	webix.message({type:"error",text:localizator.lex_emptyString+gFN(),expire:expireTemp});
  });
}

// Plot setup data. Call from button click on page list file
function fileForPSSelect(id){
  var selfile=$$("fileList").getItem(id);
  webix.ajax().post("data/csvtojson_e.php",{filename:"./data/"+selfile.title,delfile:0}).then( function (result) {
    $$("graphPsDataSetEven").clearAll();
    var meas=result.json();
    var mi=meas[meas.length-1].min;
    var ma=meas[meas.length-1].max;
    var st=meas[meas.length-1].step;
    meas.pop(); // remove last data min max step
    $$("graphPsDataSetEven").parse(meas); 
    $$("graphPsDataSetEven").define({ yAxis:{start:mi,end:ma,step:st,title:"I,A"}});
    $$("graphPsDataSetEven").define( {series:[ {value:"#y1e#",line:{color:"#1293f8",width:2}},{value:"#y2e#",line:{color:"#66cc00",width:2}} ] } );
    $$("graphPsDataSetEven").refresh();
    $$("selFile").setValue(selfile.title);
    $$("selFile").refresh();
    $$("tabview").getTabbar().setValue("t2_e");
    $$("buttonWrite").enable();
    $$("buttonLoad").disable();
  }).fail(function fileForPSSelect(result){
    data=testdata;
	webix.message({type:"error",text:localizator.lex_emptyString+gFN(),expire:expireMess});
  });
  webix.ajax().post("data/csvtojson_o.php",{filename:"./data/"+selfile.title,delfile:0}).then( function (result) {
    $$("graphPsDataSetOdd").clearAll();
    var meas=result.json();
    var mi=meas[meas.length-1].min;
    var ma=meas[meas.length-1].max;
    var st=meas[meas.length-1].step;
    meas.pop(); // remove last data min max step
    $$("graphPsDataSetOdd").parse(meas); 
    $$("graphPsDataSetOdd").define({ yAxis:{start:mi,end:ma,step:st,title:"I,A"}});
    $$("graphPsDataSetOdd").define( {series:[ {value:"#y1e#",line:{color:"#1293f8",width:2}},{value:"#y2e#",line:{color:"#66cc00",width:2}} ] } );
    $$("graphPsDataSetOdd").refresh();
//    $$("selFile").setValue(selfile.title);
//    $$("selFile").refresh();
//    $$("tabview").getTabbar().setValue("t2_o");
    $$("buttonWrite").enable();
    $$("buttonLoad").disable();
  }).fail(function fileForPSSelect(result){
    data=testdata;
	webix.message({type:"error",text:localizator.lex_emptyString+gFN(),expire:expireMess});
  });
}

// Change tabs logic
function changeTab(id){

  if(id=="t1"){
    $$("buttonLoad").enable();
  }
  if(id=="t2_e"){
    $$("buttonLoad").disable();
  }
  if(id=="t2_o"){
    $$("buttonLoad").disable();
  }
  if(id=="t3"){
    $$("buttonLoad").disable();
  }
}

// UpLoad
// Get new file list after upload compleate
function uploadCompleate(item, response){
  webix.ajax().post("data/dataupload.php",{fselect:""}).then( function (result){
  $$("fileList").clearAll();
  var list=result.json();
  $$("fileList").parse(list);
  }).fail(function uploadCompleate(result){
	webix.message({type:"error",text:localizator.lex_emptyString+gFN(),expire:expireMess});
  });
}

// Events on buttons clicks
// Write data request
function writePressed(id,e){
  var selfile=$$("fileList").getSelectedItem();
  webix.ajax().post("data/dataupload.php",{fselect:selfile.title}).fail(function writePressed(result){
	webix.message({type:"error",text:localizator.lex_serverDataError+gFN(),expire:expireMess});
  });
}

function swPowerOn(id,e){
  webix.ajax().post("./data/writereq.php",{power:"power=1"}).fail(function swPowerOn(result){
	webix.message({type:"error",text:localizator.lex_serverDataError+gFN(),expire:expireMess});
  });
}

function swPowerOff(id,e){
  webix.ajax().post("./data/writereq.php",{power:"power=0"}).fail(function swPowerOff(result){
	webix.message({type:"error",text:localizator.lex_serverDataError+gFN(),expire:expireMess});
  });
}

// Visualisation of status on form
function setStatus(obj){
  $$("status").parse(obj);
  if(obj[0].power=="On") {
    webix.html.removeCss( $$("status").getNode(), "psOffClass");
    webix.html.addCss( $$("status").getNode(), "psOnClass");
    $$("status").refresh();
  }
  else if(obj[0].power=="Off") {
    webix.html.removeCss( $$("status").getNode(), "psOnClass");
    webix.html.addCss( $$("status").getNode(), "psOffClass");
    $$("status").refresh();
  }
  else {
    webix.html.removeCss( $$("codeError").getNode(), "psErrClass");
	webix.html.removeCss( $$("status").getNode(), "psOffClass");
    webix.html.addCss( $$("codeError").getNode(), "headerClass");
    $$("codeError").refresh();
  }
  $$("codeError").parse(obj);
  if(obj[0].error!="None") {
    webix.html.removeCss( $$("codeError").getNode(), "headerClass");
    webix.html.addCss( $$("codeError").getNode(), "psErrClass");
    $$("codeError").refresh();
  }
  else{
    webix.html.removeCss( $$("codeError").getNode(), "psErrClass");
    webix.html.addCss( $$("codeError").getNode(), "headerClass");
    $$("codeError").refresh();
  }
  $$("statusWrite").parse(obj);
  $$("statusWrite").refresh();
}

// Get status from server
function getStatus() {
  ticCnt++;
  webix.ajax().post("./data/getstatus.php").then( function (result){ //tutu
    if(result.text().length!=0){
      var stat=result.json();
	  ticCnt=0;
      setStatus(stat);
      if(stat[0].update) {
		plotMeasuredData();
      }
	}
	else {
	   stat=[ {'power':"unknown", 'error':"unknown", 'statusReq':"unknown",'update':0  } ];
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
    var stat=[ {'power':"unknown", 'error':"unknown",'statusReq':"unknown", 'update':0  } ]; setStatus(stat);
	webix.message({type:"error",text:localizator.lex_serverDataError+gFN(),expire:expireTemp});
  });
}

// get listing upload files
function getFileList() {
  webix.ajax().post("data/dataupload.php",{fselect:""}).then(function (result){
    var list=result.json();
    $$("fileList").parse(list);
    var index=list[0].idsel;
    if(index>0){
  	  $$("fileList").select(index);
	  fileForPSSelect(index);
    }
  }).fail(function getFileList(result){
    webix.message({type:"error",text:localizator.lex_emptyString+gFN(),expire:expireMess});
  });
}


var logic = {
init: function(){
  if(access())
  { 
// set send stop request on close window event
    window.onunload=reqestOnQuit; 
// catch close event on close browser
    window.onbeforeunload =reqestOnTryClose;
	ticCnt=0;
	getFileList();
    setInterval(function(){getStatus()}, 1500);
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
