/*
	App configuration
*/

define([
	//"libs/webix-jet/core"
], function( core){

	//configuration
	var app = core.create({
		id:			"Mpp", //change this line!
		name:		"My App from 20!",
		version:	"0.1.0",
		debug:		true,
		start:		"/top/data"
	});

	return ;
});
