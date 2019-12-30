//------------------------------------------------ Headers ---------------------------------------------

// UP header
var ui_Upheader= {
  view:"template", 
  id:"Upheader",
  template: localizator.lex_Upheader+"#direction#"+"#graphview#", //"Orbita U-70 view measure program",
  type:"header",
  css:"headerClass"
  //width:200,
 // height:60
}

// Status of measure header
var ui_statusHeader= {
  view:"template",
  id:"statusHeader",
  template: localizator.lex_statusHeader,//"Status of orbita",
  type:"header",
  width:140,
  css:"headerClass",
//  height:60
}
var ui_dataFreshHeader= {
  view:"template",
  id:"dataFreshHeader",
  template: "Data view: "+"#oldnew#",
  type:"header",
  width:200,
  css:"headerClass",
//  height:60
}

// Start from header
var ui_startHeader= {
  view:"template",
  id:"startHeader",
  template: localizator.lex_startHeader,//"Status of orbita",
  type:"header",
  width:140,
  css:"headerClass",
//  height:60
}

// Date time header
var ui_dataTimeHeader= {
  view:"template",
  id:"dataTimeHeader",
  template: localizator.lex_dataTimeHeader,//"Data/time of measure",
  type:"header",
  width:180,
  css:"headerClass",
//  height:60
}

//------------------------------------------------ Statuses ---------------------
// Status value
var ui_status= {
  view:"template",
  id:"status",
  template: "#status#",
  type:"header", 
  css:"headerClass",
  //width:80
 // css:"headerClass",
//  height:60
}

// Start from B2/KC1
var ui_startFrom={
  view:"template",
  id:"statusSt",
  template: "#start# on time #T1# #T2# #T3# #T4# #T5# #T6# #T7# #T8# #T9# #T10# #T11# #T12# #T13# #T14# #T15# #T16# ms",
  type:"header",
  css:"headerClass",
 // width:80
 // css:"headerClass",
//  height:60
}

// Measure time
var ui_measDT={
  view:"template",
  id:"statusDT",
  template: "#datetime#",
  type:"header",
  css:"headerClass",
  //width:80
 // css:"headerClass",
//  height:60
}


var ui_setLimits={
  view:"combo",
  id:"setLimits",
  label:localizator.lex_setLimits,//"Set graph limits in mm",
  css:"headerClass",
//  width:300,
  labelWidth:200,
  value:1,
  options:[{id:1,value:"50"},{id:2,value:"40"},{id:3,value:"25"},{id:4,value:"10"},{id:5,value:"5"}],
  on:{'onChange':changeLimits}
}
//------------------------------------------------ Table orbita  R -----------------------------------------
var ui_tableOrbR={
  view:"datatable",
  id:"tableOrbR",
  columns:[  {id:"axe",  header:"Axe"},
   	     {id:"t1",header:"T1"},
             {id:"t2",header:"T2"},
             {id:"t3",header:"T3"},
             {id:"t4",header:"T4"},
             {id:"t5",header:"T5"},
             {id:"t6",header:"T6"},
             {id:"t7",header:"T7"},
             {id:"t8",header:"T8"},
             {id:"t9",header:"T9"},
             {id:"t10",header:"T10"},
             {id:"t11",header:"T11"},
             {id:"t12",header:"T12"},
             {id:"t13",header:"T13"},
             {id:"t14",header:"T14"},
             {id:"t15",header:"T15"},
             {id:"t16",header:"T16"}
	   ],
//  url:"./data/getorbtext.php",
  scrollAlignY:true,
  editable:false,
  autoheight:false,
  autowidth:false
}
//------------------------------------------------ Table orbita harm R -----------------------------------------
var ui_tableHarmR={
  view:"datatable",
  id:"tableHarmR",
  columns:[  {id:"ti",  header:"time, ms"},
   	     {id:"sin9",header:"Sin9",editor:"text"},
             {id:"cos9",header:"Cos9",editor:"text"},
             {id:"sin10",header:"Sin10",editor:"text"},
             {id:"cos10",header:"Cos10",editor:"text"},
             {id:"sin11",header:"Sin11",editor:"text"},
             {id:"cos11",header:"Cos11",editor:"text"},
             {id:"avr",header:"Average",editor:"text"},
             {id:"rms",header:"RMS",editor:"text"}
	   ],
//  url:"./data/getharmtext.php",
  scrollAlignY:true,
  editable:false,
  autoheight:false,
  autowidth:false
}

//------------------------------------------------ Table orbita Z -----------------------------------------
var ui_tableOrbZ={
  view:"datatable",
  id:"tableOrbZ",
  columns:[  {id:"axe",  header:"Axe"},
   	     {id:"t1",header:"T1"},
             {id:"t2",header:"T2"},
             {id:"t3",header:"T3"},
             {id:"t4",header:"T4"},
             {id:"t5",header:"T5"},
             {id:"t6",header:"T6"},
             {id:"t7",header:"T7"},
             {id:"t8",header:"T8"},
             {id:"t9",header:"T9"},
             {id:"t10",header:"T10"},
             {id:"t11",header:"T11"},
             {id:"t12",header:"T12"},
             {id:"t13",header:"T13"},
             {id:"t14",header:"T14"},
             {id:"t15",header:"T15"},
             {id:"t16",header:"T16"}
	   ],
//  url:"./data/getorbtext.php",
  scrollAlignY:true,
  editable:false,
  autoheight:false,
  autowidth:false
}

//------------------------------------------------ Table orbita harm Z -----------------------------------------
var ui_tableHarmZ={
  view:"datatable",
  id:"tableHarmZ",
  columns:[  {id:"ti",  header:"time, ms"},
   	     {id:"sin9",header:"Sin9",editor:"text"},
             {id:"cos9",header:"Cos9",editor:"text"},
             {id:"sin10",header:"Sin10",editor:"text"},
             {id:"cos10",header:"Cos10",editor:"text"},
             {id:"sin11",header:"Sin11",editor:"text"},
             {id:"cos11",header:"Cos11",editor:"text"},
             {id:"avr",header:"Average",editor:"text"},
             {id:"rms",header:"RMS",editor:"text"}
	   ],
//  url:"./data/getharmtext.php",
  scrollAlignY:true,
  editable:false,
  autoheight:false,
  autowidth:false
}

// data for template plot with 1s and all zeroes
var  testdata = [ {axe:2, y1:0, y2:0},{axe:4, y1:0, y2: 0},{axe:6, y1:0, y2:0},{axe:116, y1:0,  y2:0},{axe:118, y1:0, y2:0},{axe:120, y1:0, y2:10}]

//------------------------------------------------ Plot orbita R data -----------------------------------------
var ui_chartOrbR={
  id:"chartOrbR",
  view:"chart",
  type:"line",
  xValue:"#axe#",
  xAxis:{lines:function(obj){return obj.axe%10?false:true}, template:function(obj){return obj.axe%10?"":obj.axe},title:"position"},
  offset:0.0,
  yAxis:{start:-50,end:50,step:10,title:"Position,mm"},
  item:{radius:0},
  origin:0.0,
  legend:{
    values:[
           {text:"T1",color :"#000000"},
           {text:"T2",color :"#0A6CE4"},
           {text:"T3",color :"#200AE4"},
           {text:"T4",color :"#9F0AE4"},
           {text:"T5",color :"#8A05E9"},
           {text:"T6",color :"#C1009E"},
           {text:"T7",color :"#05E977"},
           {text:"T8",color :"#0AE4D2"},
           {text:"T9",color :"#0AE486"},
           {text:"T10",color:"#0EE40A"},
           {text:"T11",color:"#ACE905"},
           {text:"T12",color:"#E9DA05"},
           {text:"T13",color:"#6daa2c"},
           {text:"T14",color:"#E97405"},
           {text:"T15",color:"#E94A05"},
           {text:"T16",color:"#E90505"} 
           ],
    align:"right",valign:"middle",layout:"y",width: 100,margin: 8
  },
  series:[
    {value:"#t1#",line:{color:"#1293f8",width:2}},
    {value:"#t2#",line:{color:"#66cc00",width:2}}
  ],
  data:testdata
}
var ui_chartHarmR={
  id:"chartHarmR",
  view:"chart",
  type:"line",
  xValue:"#ti#",
  xAxis:{lines:function(obj){return obj.ti%10?false:true}, template:function(obj){return obj.ti%10?"":obj.ti},title:"time, ms"},
  offset:0.0,
  yAxis:{start:-50,end:50,step:10,title:"Position,mm"},
  item:{radius:0},
  origin:0.0,
  legend:{
    values:[
	{text:"Sin9",color:"#000000"},{text:"Cos9",color:"#0A6CE4"},
	{text:"Sin10",color:"#200AE4"},{text:"Cos10",color:"#9F0AE4"},
	{text:"Sin11",color:"#E90505"},{text:"Cos11",color:"#E97405"},
	{text:"Avr",color:"#05E977"},{text:"RMS",color:"#ACE905"}
	],
    align:"right",valign:"middle",layout:"y",width: 100,margin: 8
  },
  series:[
    {value:"#t1#",line:{color:"#1293f8",width:2}},
    {value:"#t2#",line:{color:"#66cc00",width:2}}
  ],
  data:testdata
}
var ui_chartOrbZ={
  id:"chartOrbZ",
  view:"chart",
  type:"line",
  xValue:"#axe#",
  xAxis:{lines:function(obj){return obj.axe%10?false:true}, template:function(obj){return obj.axe%10?"":obj.axe},title:"position"},
  offset:0.0,
  yAxis:{start:-50,end:50,step:10,title:"Position,mm"},
  item:{radius:0},
  origin:0.0,
  legend:{
    values:[
           {text:"T1",color :"#000000"},
           {text:"T2",color :"#0A6CE4"},
           {text:"T3",color :"#200AE4"},
           {text:"T4",color :"#9F0AE4"},
           {text:"T5",color :"#8A05E9"},
           {text:"T6",color :"#C1009E"},
           {text:"T7",color :"#05E977"},
           {text:"T8",color :"#0AE4D2"},
           {text:"T9",color :"#0AE486"},
           {text:"T10",color:"#0EE40A"},
           {text:"T11",color:"#ACE905"},
           {text:"T12",color:"#E9DA05"},
           {text:"T13",color:"#6daa2c"},
           {text:"T14",color:"#E97405"},
           {text:"T15",color:"#E94A05"},
           {text:"T16",color:"#E90505"},
	],
    align:"right",valign:"middle",layout:"y",width: 100,margin: 8
  },
  series:[
    {value:"#t1#",line:{color:"#1293f8",width:2}},
    {value:"#t2#",line:{color:"#66cc00",width:2}}
  ],
  data:testdata
}
var ui_chartHarmZ={
  id:"chartHarmZ",
  view:"chart",
  type:"line",
  xValue:"#ti#",
  xAxis:{lines:function(obj){return obj.ti%10?false:true}, template:function(obj){return obj.ti%10?"":obj.ti},title:"position"},
  offset:0.0,
  yAxis:{start:-50,end:50,step:10,title:"Position,mm"},
  item:{radius:0},
  origin:0.0,
  legend:{
    values:[
	{text:"Sin9",color:"#000000"},{text:"Cos9",color:"#0A6CE4"},
	{text:"Sin10",color:"#200AE4"},{text:"Cos10",color:"#9F0AE4"},
	{text:"Sin11",color:"#E90505"},{text:"Cos11",color:"#E97405"},
	{text:"Avr",color:"#05E977"},{text:"RMS",color:"#ACE905"}
	],
    align:"right",valign:"middle",layout:"y",width: 100,margin: 8
  },
  series:[
    {value:"#t1#",line:{color:"#1293f8",width:2}},
    {value:"#t2#",line:{color:"#66cc00",width:2}}
  ],
  data:testdata
}

//------------------------------------------------ Tab contents -----------------------------------------
//------------------------------------------------ View Plot orbita R------------------------------------
var ui_graphViewOrbR= {
  id:"ui_graphViewOrbR",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_chartOrbR]}]
}
//------------------------------------------------ View Plot harm R------------------------------------
var ui_graphViewHarmR= {
  id:"ui_graphViewHarmR",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_chartHarmR]}]
}
//------------------------------------------------ View table orbit R ----------------------------------
var ui_tableViewOrbR = {
  id:"ui_tableViewOrbR",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_tableOrbR]}]
}
//------------------------------------------------ View table harm R ----------------------------------
var ui_tableViewHarmR = {
  id:"ui_tableViewHarmR",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_tableHarmR]}]
}

//------------------------------------------------ View Plot orbita Z ------------------------------------
var ui_graphViewOrbZ = {
  id:"ui_graphViewOrbZ",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_chartOrbZ]}]
}
//------------------------------------------------ View plot harm Z ----------------------------------
var ui_graphViewHarmZ = {
  id:"ui_graphViewHarmZ",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_chartHarmZ]}]
}
//------------------------------------------------ View table orbit Z ----------------------------------
var ui_tableViewOrbZ = {
  id:"ui_tableViewOrbZ",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_tableOrbZ]}]
}
//------------------------------------------------ View table harm Z ----------------------------------
var ui_tableViewHarmZ = {
  id:"ui_tableViewHarmZ",
  view:"layout",
  container:"formContainer",
  rows:[{cols:[ui_tableHarmZ]}]
}

//------------------------------------------------ Tab view ------------------------------------------------
var ui_tabview = {
    view:"tabview",
	id:"tabview",
	height:500, // change height of form width in index.html!
    cells:[ 
      {
        header: localizator.lex_t1,//"Orbita R",
        body:{
	    id:"t1", rows:[ui_graphViewOrbR]
        }    
      },
      {
        header: localizator.lex_t2,//"Harmonics R",
        body:{
          id:"t2",rows:[ui_graphViewHarmR]
        }      
      },
      {
        header: localizator.lex_t3,//"Table Orb R",
        body:{
	  id:"t3",rows:[ui_tableViewOrbR]
        }
      },
            {
        header: localizator.lex_t4,//"Table harm R",
        body:{
	  id:"t4",rows:[ui_tableViewHarmR]
        }
      },
      {
        header: localizator.lex_t5,//"Orbita Z",
        body:{
          id:"t5",rows:[ui_graphViewOrbZ]
        }      
      },
      { 
        header: localizator.lex_t6,//"Harmonics Z",
        body:{
	  id:"t6",rows:[ui_graphViewHarmZ]
        } 
      },
      {
        header: localizator.lex_t7,//"Table Orb Z",
        body:{
	  id:"t7",rows:[ui_tableViewOrbZ]
        }
      },
      {
        header: localizator.lex_t8,//"Table harm Z",
        body:{
	  id:"t8",rows:[ui_tableViewHarmZ]
        }
      },
    ],
	tabbar:{
	  on:{
	    'onAfterTabClick' :changeTab
	  }
	}
}

//------------------------------------------------ Buttons ------------------------------------------------
// Button refresh status
var ui_buttonRefresh = {
  view:"button",
  id:"buttonRefresh",
  type:"base",
  value:localizator.lex_buttonRefresh,
  tooltip:"Refresh all data.",
  on:{
    'onItemClick' :refreshData
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
	  { cols:[ui_Upheader,ui_dataFreshHeader,ui_setLimits]},
          { cols:[ui_tabview] },
          //{ cols:[ui_buttonRefresh],height:50},
		  {cols:[ui_startHeader,ui_startFrom]},
//	  { cols:[ui_selFileHeader,ui_selFile]},
	  { cols:[ui_statusHeader,ui_status,ui_dataTimeHeader,ui_measDT]},
    ]
};
