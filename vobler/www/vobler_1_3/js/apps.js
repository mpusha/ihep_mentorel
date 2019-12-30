//------------------------------------------------ Headers ---------------------------------------------
// UP header
var ui_Upheader= {
  view:"template", 
  id:"Upheader",
  template: localizator.lex_Upheader, //"Vobler magnet power supply control program",
  type:"header",
  css:"headerClass",
  //width:200,
 // height:60
}

// Status name header
var ui_statusHeader= {
  view:"template", 
  id:"statusHeader",
  template: localizator.lex_statusHeader,//"Status PS",
  type:"header", 
  width:80,
  //css:"headerClass",
//  height:60
}

// Status value
var ui_status= {
  view:"template", 
  id:"status",
  template: "#power#",
  type:"header", 
  css:"headerClass",
  width:80,
 // css:"headerClass",
//  height:60
}

// Error name header
var ui_codeErrorHeader= {
  view:"template", 
  id:"codeErrorHeader",
  template: localizator.lex_codeErrorHeader,//"Status error",
  type:"header", 
  width:80,
  //css:"headerClass",
 // height:60
}

// Error value
var ui_codeError= {
  view:"template", 
  id:"codeError",
  template: "#error#",
  type:"header", 
  css:"headerClass",
 // height:60
}
// Write status name header
var ui_statusWriteHeader= {
  view:"template", 
  id:"statusWriteHeader",
  template: localizator.lex_statusWriteHeader,//"Status error",
  type:"header", 
  width:60,
  //css:"headerClass",
//  height:40
}

//Status value
var ui_statusWrite= {
  view:"template", 
  id:"statusWrite",
  template: "#statusReq#",
  type:"header", 
  css:"headerClass",
  width:110
}

// Selected file header
var ui_selFileHeader= {
  view:"label", 
  id:"selFileHeader",
  label: localizator.lex_selFileHeader,//"Select file:",
  //type:"header", 
  css:"labelClass",
  width:120,
}

// Selected file name
var ui_selFile= {
  view:"label", 
  id:"selFile",
  css:"labelClass",
 // height:60
}

// data for template plot with 1s and all zeroes
var  testdata = [ {t:0, y1:0, y2:0},{t:200, y1:0, y2: 0},{t:400, y1:0, y2:0},{ t:600, y1:0,  y2:0},{t:800, y1:0, y2:0},{t:1000, y1:0, y2:0} ]

//------------------------------------------------ Plot setup data-----------------------------------------			
var ui_chartPsDataSet={
  id:"graphPsDataSet",
  view:"chart",
  type:"line",
  xValue:"#t#",
  xAxis:{lines:function(obj){return obj.t%100?false:true}, template:function(obj){return obj.t%100?"":obj.t},title:"t, ms"},
  offset:0.0,
  yAxis:{start:-800,end:800,step:200,title:"I,A"},
  item:{radius:0},
  origin:0.0,	
  legend:{
    values:[{text:"PS1",color:"#1293f8"},{text:"PS2",color:"#66cc00"}],
    align:"right",valign:"middle",layout:"y",width: 100,margin: 8
  }, 
  series:[
    {value:"#y1#",line:{color:"#1293f8",width:2}},
	{value:"#y2#",line:{color:"#66cc00",width:2}}
  ],
  data:testdata
}

//------------------------------------------------ Plot measured data--------------------------------------
var ui_chartPsDataMeas={
  id:"graphPsDataMeas",
  view:"chart",
  type:"line",
  xValue:"#t#",
  xAxis:{lines:function(obj){return obj.t%100?false:true}, template:function(obj){return obj.t%100?"":obj.t},title:"t, ms"},
  offset:0.0,
 // yAxis:{start:-800,end:800,step:200,title:"I,A"},
  yAxis:{title:"I,A"},
  item:{radius:0},
  origin:0.0,	
  legend:{
    values:[{text:"PS1",color:"#1293f8"},{text:"PS2",color:"#66cc00"}],
    align:"right",valign:"middle",layout:"y",width: 100,margin: 8
  }, 
  series:[
    {value:"#y1#",line:{color:"#1293f8",width:2}},
	{value:"#y2#",line:{color:"#66cc00",width:2}}
  ],
  data:testdata
}
//------------------------------------------------ Tab contents --------------------------------------------
//------------------------------------------------ File List -----------------------------------------------
var ui_fileList={
  view:"list",
  id:"fileList",
  select:true,
  multiselect:false,
  template:"#title#",
  on:{
    'onItemClick' :fileForPSSelect
  }
}

//------------------------------------------------ View Plot setup data ------------------------------------
var ui_graphViewPsSetupData = {
  id:"graphViewPsSetupData",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_chartPsDataSet]}]
}

//------------------------------------------------ View Plot measure data ----------------------------------
var ui_graphViewPsMeasData = {
  id:"graphViewPsMeasData",
    view:"layout",
	container:"formContainer",
	rows:[{cols:[ui_chartPsDataMeas]}]
}
//------------------------------------------------ Tab view ------------------------------------------------
var ui_tabview = {
    view:"tabview",
	id:"tabview",
	height:350, // change height of form width in index.html!
    cells:[ 
      {
        header: localizator.lex_t1,//"Select file:","Data for setup",
        body:{
		  id:"t1", rows:[ui_fileList]   
        }    
	  },
      {
        header: localizator.lex_t2,//"Vobler PS setup current",
        body:{
           id:"t2",rows:[ui_graphViewPsSetupData ]   
        }      
      },
      { 
	    header: localizator.lex_t3,//"Vobler PS measured current", 
        body:{
		  id:"t3",rows:[ui_graphViewPsMeasData ]
        } 
      }
    ],
	tabbar:{
	  on:{
	    'onAfterTabClick' :changeTab
	  }
	}
}

//------------------------------------------------ Buttons ------------------------------------------------
// Upload data on server
var ui_buttonLoad = {
  view:"uploader",
  id:"buttonLoad",
  type:"base",
  value:localizator.lex_buttonLoad,//"Load data",
  name:"files", accept:"csv/plain",
  multiple:false,
  upload:"data/upload.php",
  tooltip:localizator.lex_buttonLoadTT,//"UpLoading data on server.",
  on:{
    'onFileUpload' :uploadCompleate
  }
}

// Write data into hardware
var ui_buttonWrite = {
  view:"button",
  id:"buttonWrite",
  type:"base",
  value:localizator.lex_buttonWrite,//"Write data",
  disabled:true,
  tooltip: localizator.lex_buttonWriteTT,//"Wrie data into hardware.",
  on:{
    'onItemClick' :writePressed
  }
}

// Power switch On 
var ui_buttonOn = {
  view:"button",
  id:"buttonOn",
  type:"danger",
  value: localizator.lex_buttonOn,//"Switch On",
  tooltip: localizator.lex_buttonOnTT,//"Turn on power supply.",
    on:{
    'onItemClick' :swPowerOn
  }
}

// Power switch Off
var ui_buttonOff = {
  view:"button",
  id:"buttonOff",
  type:"form",
  value: localizator.lex_buttonOff,//"Switch Off",
  tooltip: localizator.lex_buttonOffTT,//"Turn off power supply.",
    on:{
    'onItemClick' :swPowerOff
  }
}

// Button refresh status
var ui_buttonRefresh = {
  view:"button",
  id:"buttonRefresh",
  type:"base",
  value:"Refresh",
  tooltip:"Refresh all data.",
  on:{
    'onItemClick' :getStatus
  }
}
//------------------------------------------------ Main element---------------------------------------------
// Top GUI form
var ui_top = {
    id:"top_ui",
    view:"layout",
	container:"formContainer",
	tipe:"line",
	rows:[
	//  { cols:[ui_Upheader]},
      { cols:[ui_tabview] }, 
      { cols:[ui_buttonLoad,ui_buttonWrite,ui_buttonOn,ui_buttonOff,ui_buttonRefresh],height:50},
	  { cols:[ui_selFileHeader,ui_selFile]},
	  { cols:[ui_statusHeader,ui_statusWrite,ui_status,ui_codeErrorHeader,ui_codeError]},
    ]
};
