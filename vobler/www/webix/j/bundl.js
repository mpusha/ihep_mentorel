var translations = {
	// English
	"en-US": {
		localeName: "en-US",
		headerTitle: "Data master",
		resetFilters: "Reset filters",
		changeLocale: "Change language:",
		loadData: "Load data",
		addRow: "Add row",
		clearSelection: "Clear selection",
		deleteRow: "Delete row",
		saveData: "Save data",
		title: "Title",
		noItemSelected: "No item selected",
		dataSaved: "Data saved",
		reservedButton: "Reserved botton"
	},

	// Russian
	"ru-RU": {
		localeName: "ru-RU",
		headerTitle: "Мастер данных",
		resetFilters: "Сбросить фильтры",
		changeLocale: "Сменить язык:",
		loadData: "Загрузить данные",
		addRow: "Добавить ряд",
		clearSelection: "Снять выделение",
		deleteRow: "Удалить ряд",
		saveData: "Сохранить",
		title: "Название",
		noItemSelected: "Нет выбранных рядов",
		dataSaved: "Данные сохранены",
		reservedButton: "Зарезервировано..."
	}
};

var defaultLocale = "en-US";

// object from translations.js
var localizator = translations[defaultLocale];

/**
 * Get data from backend and fill datatable grid
 */
function getData() {
   // $$("dataFromBackend").clearAll();
    $$("dataFromBackend").load("data/data.php");

    // old backend on Apache/PHP:
    //$$("dataFromBackend").load("http://localhost/data_master/data/data.php");
    
}

/**
 * Add new row to datatable
 */
function addRow() {
    $$("dataFromBackend").add(
        {
            title: "-----",
            content: "-----",
            place: "-----"
            //date: "-----",
            //priority: "-----"
        }
    );
}

/**
 * Reset selection in datatable grid
 */
function clearSelection() {
    $$("dataFromBackend").unselectAll();
}

/**
 * Delete selected row
 */
function deleteRow() {
    if (!$$("dataFromBackend").getSelectedId()) {
        webix.alert(localizator.noItemSelected);
        return;
    }

    //removes the selected item
    $$("dataFromBackend").remove($$("dataFromBackend").getSelectedId());
}

/**
 * Save data to backend from datatable grid
 */
function saveData() {
    var grid = $$("dataFromBackend");
    var serializedData = grid.serialize();
    
    // The default Webix header definition is "Content-type": "application/x-www-form-urlencoded"
    // JSON data will not work in this case. Set "Content-Type": "application/json"
    //webix.ajax().headers({
    //    "Content-Type": "application/json"
    //}).post("data/save.php", {data: serializedData});

    // old backend on Apache/PHP:
    webix.ajax().post("data/save.php", {data: serializedData});

    webix.alert(localizator.dataSaved);
    
}

/**
 * Reset filters settings
 */
function resetFilters() {
    $$("dataFromBackend").getFilter("title").value = null;
    $$("dataFromBackend").getFilter("content").value = null;
    $$("dataFromBackend").getFilter("place").value = null;
    $$("dataFromBackend").getFilter("date").value = null;
    $$("dataFromBackend").getFilter("priority").value = null;
    
    // reload grid
   // $$("dataFromBackend").clearAll();
    $$("dataFromBackend").load("data/data.php"); 

    // old backend on Apache/PHP:
    //$$("dataFromBackend").load("http://localhost/data_master/data/data.php"); 
}

/**
 * Change translation to selected
 */
function changeLoc(locale) {
    localizator = translations[locale];
    
    $$("headerContainer").define("template", localizator.headerTitle);
    $$("headerContainer").refresh();

    $$("resetFiltersContainer").define("value", localizator.resetFilters);
    $$("resetFiltersContainer").refresh();

    $$("changeLocale").define("label", localizator.changeLocale);
    $$("changeLocale").refresh();

    $$("loadData").define("value", localizator.loadData);
    $$("loadData").refresh();

    $$("addRow").define("value", localizator.addRow);
    $$("addRow").refresh();

    $$("clearSelection").define("value", localizator.clearSelection);
    $$("clearSelection").refresh();

    $$("deleteRow").define("value", localizator.deleteRow);
    $$("deleteRow").refresh();

    $$("saveData").define("value", localizator.saveData);
    $$("saveData").refresh();

    $$("reservedButton").define("value", localizator.reservedButton);
    $$("reservedButton").refresh();

    webix.i18n.setLocale(locale);
}

/**
 * Function for reserved button
 */
function reservedButton() {
    // your code...
		$$("chart").xAxis={start:0,
                end:100,
                step:10,
				template:"#x#"
            };
	$$("chart").clearAll();		
	//dt=$$("chart").load("data/gr.php");
//	$$("chart").render();
 webix.ajax().post("data/gr.php", function (text) {
  $$("chart").parse(text);
 //return ;
  webix.message({
    type:"", 
    text:text,
    //expire:10000
    expire:-1   //for canceling expire period
  });
  
});
}

/**
 * Create object with type "Button"
 *
 * @constructor
 */
function Button(id, value, type, width, onClickFunction) {
	this.view = "button";
	this.id = id;
	this.value = value;
	this.type = type;
	this.width = width;
	this.on = {
		"onItemClick": function(){ 
	      onClickFunction();
	    }
	}
}
var  dt = [   { x:1, y:-50},
                 { x:2, y:50}
		     ];
function chart(id) {

	this.id=id;
	this.view="chart";
    this.type="line";
	this.yValue="#y#";
	this.xValue="#x#";
	this.xAxis={start:0,
                end:100,
                step:10,
				template:"#x#"
            };
	this.offset=0;
    this.yAxis={
                start:-10,
                end:100,
                step:50
                //template:function(obj){
                    //return (obj%20?"":obj);
                };
	this.origin=0;			
	this.data=dt;
}
function upLoader(id) {
  this.id=id;
  this.view="uploader";
  this.value="Uploader file";
  this.name="files";
  this.autosend=false;
  this.upload="data/upload.php";
}
/**
 * Create main layout
 */
webix.ui({
	view: "layout",
	id: "page",
	rows:[
		{
			cols: [
				{
					view:"icon",
					id: "headerIconContainer",
					icon:"shopping-cart"
				},
				{
					view:"template",
					id: "headerContainer",
					type:"header",
					template:"Data master"
			    },

			    new Button("ubutt", "upload file", "form", 150, resetFilters),
				{
					id: "divider0",
					width: 20
				},
				new Button("resetFiltersContainer", "Reset filters", "form", 150, resetFilters),
				{
					id: "divider",
					width: 20
				},
				{
					view: "combo", 
				    id: "changeLocale",
				    label: 'Change locale:',
				    labelWidth: 130,
				    width: 230,
				    align: "right",
				    value: "en-US",
				    options: [
				    	"ru-RU",
				    	"en-US",
						"XZ"
				    ],
				    on: {
				        "onChange": function(newv, oldv) { 
				          	changeLoc(newv);
				        }
				    }
				}
			]
		},
	  {
	  	view: "datatable",
	  	id: "dataFromBackend",
		columns: [
			{
				id: "title",
				header: [
					{
						text: "<b>Title</b>"
					},
					{
						content: "textFilter"
					}
				],
				editor: "text",
				fillspace: 2
			},
			{
				id: "content",
				header: [
					{
						text: "<b>Content</b>"
					},
					{
						content: "textFilter"
					}
				],
				editor: "popup",
				fillspace: 8
			},
			{
				id: "place",
				header: [
					{
						text: "<b>Place</b>"
					},
					{
						content: "textFilter"
					}
				],
				editor: "text",
				fillspace: 2
			},
			{
				id: "date",
				header: [
					"<b>Date</b>",
					{
						content: "dateFilter"
					}
				],
				editor: "date",
				map: "(date)#date#",
				format: webix.Date.dateToStr("%d.%m.%Y"),
				fillspace: 2
			},
			{
				id: "priority",
				header: [
					"<b>Priority</b>",
					{
						content: "selectFilter"
					}
				],
				editor: "select",
				options: [1, 2, 3, 4, 5],
				fillspace: 1
			}
		],
		editable: true,
		select: "row",
		multiselect: true,
	    // initial data load
	    //data: webix.ajax().post("http://localhost/electron_with_backend/data/data.php")
	    data: webix.ajax().get("data/data.php")
	    //data: initialLoad()
	  },
	  {
	  		view: "layout",
	  		id: "chartContainer",
	  		height: 450,
	  		cols: [
			  	// Webix ui.button structure example:
			  	/*{
				  	view: "button", 
				    id: "loadData", 
				    value: "Load data", 
				    type: "form", 
				    width: 150,
				    on: {
					    "onItemClick": function(id, e, trg){ 
					      getData();
					    }
					}
				},*/
		  		new chart("chart")
			]
	  	},
	  	{
	  		view: "layout",
	  		id: "buttonContainer",
	  		height: 50,
	  		cols: [
			  	// Webix ui.button structure example:
			  	/*{
				  	view: "button", 
				    id: "loadData", 
				    value: "Load data", 
				    type: "form", 
				    width: 150,
				    on: {
					    "onItemClick": function(id, e, trg){ 
					      getData();
					    }
					}
				},*/
		  		new Button("loadData", "Load data", "form", 150, getData),
		  		new Button("addRow", "Add row", "form", 150, addRow),
		  		new Button("clearSelection", "Clear selection", "form", 150, clearSelection),
		  		new Button("deleteRow", "Delete row", "form", 150, deleteRow),
		  		new Button("saveData", "Save data", "form", 150, saveData),
		  		new Button("reservedButton", "Reserved button", "form", 150, reservedButton),
				new upLoader("ul"),
				{ view: "button", label: "Save files", click: function() {
					$$("ul").send(function(response){
						if(response){
							webix.message(response.status);
							webix.message(response.sname);}
					});
					}},
				{}
			]
	  	}
	]
});

/*$$("buttonContainer").define("css", "buttonContainerClass");
$$("resetFiltersContainer").define("css", "resetFiltersContainerClass");
$$("resetFiltersContainer1").define("css", "resetFiltersContainerClass");
$$("headerIconContainer").define("css", "headerIconContainerClass");
$$("headerContainer").define("css", "headerContainerClass");
$$("changeLocale").define("css", "changeLocaleClass");
$$("divider").define("css", "dividerClass");
*/
//# sourceMappingURL=bundle.js.map
