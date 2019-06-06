define(['jquery','underscore','backbone', 'models/message'],
	function($, _, Backbone, MessageModel) {

		var collection = Backbone.Collection.extend({

			model: MessageModel

		});

		return new collection;

	});