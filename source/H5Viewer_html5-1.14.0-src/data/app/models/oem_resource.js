define(["jquery", "underscore", "backbone", "i18n!strings/nls/common"],

	function($, _, Backbone, locale) {

		var model = Backbone.Model.extend({
			url: function(){
				return this.get('path') // the value should be updated during initialization
			},

			// call original Backbone.Model#fetch with `dataType` as `text` for fetching text file
			fetch: function(options){
				options = _.extend(options || {}, {
					dataType: "text"
				});
				this.constructor.__super__.fetch.call(this, options);
			},

			// store response in content attribute for further usage
			parse: function(response){
				return {content: response};
			}
		});

		return model;
	});
