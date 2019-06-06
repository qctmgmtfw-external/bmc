define(['jquery','underscore','backbone', 'models/notification'],
	function($, _, Backbone, NotificationModel) {

		var collection = Backbone.Collection.extend({

			model: NotificationModel

		});

		return new collection;

	});