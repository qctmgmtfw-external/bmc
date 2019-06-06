define(['jquery','underscore','backbone',
		//localize
        'i18n!strings/nls/common',
        //template
        'text!templates/message.html',
        'moment'],
	function($, _, Backbone, CommonStrings, MessageTemplate, moment){

		var view = Backbone.View.extend({

			tagName: "li",

			template: _.template(MessageTemplate),

			initialize: function(){

				this.model.on("destroy", this.remove, this);

			},

			remove: function() {
				this.$el.remove();
			},

			serialize: function(){
				return {
					id : this.model.get('id'),
					avatar : this.model.get('avatar'),
					sender : this.model.get('sender'),
					title : this.model.get('title'),
					time : moment(this.model.get('timestamp'), "X").fromNow(),
					message: this.model.get('message')
				}
			}

		});

		return view;

	});