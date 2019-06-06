define(['jquery','underscore','backbone',
		//localize
        'i18n!strings/nls/common',
        //template
        'text!templates/usertask.html',
        'moment'],
	function($, _, Backbone, CommonStrings, UserTaskTemplate, moment){

		var view = Backbone.View.extend({

			tagName: "li",

			template: _.template(UserTaskTemplate),

			initialize: function(){

				this.model.on("destroy", this.remove, this);

			},

			remove: function() {
				this.$el.remove();
			},

			serialize: function(){
				return {
					id : this.model.get('id'),
					title : this.model.get('title'),
					progress : this.model.get('progress')
				}
			}

		});

		return view;

	});