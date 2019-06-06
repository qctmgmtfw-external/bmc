define(['jquery', 'underscore','backbone',
		'models/project-configuration'
		],
function($, _, Backbone, ProjectConfigurationModel){

	var collection = Backbone.Collection.extend({

		url : "/api/configuration/project",

		model : ProjectConfigurationModel,

		isFeatureAvailable: function(feature) {
			return this.findWhere({"feature": feature});
		}

	});

	return new collection();

});