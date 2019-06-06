define(["jquery", "underscore", "backbone",
		"models/maintenance/section-update"],
	
function($, _, Backbone, SectionUpdateModel) {
	
	var collection = Backbone.Collection.extend({
		
		url : "/api/maintenance/section-update",
		
		model : SectionUpdateModel,
		
		save : function() {
			log("updateint Section update Collection in the Server");
			this.sync("update", this);				
		}		
		
	});
	
	return new collection();
	
});
