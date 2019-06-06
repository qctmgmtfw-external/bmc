define(['jquery','underscore','backbone', 'models/usertask'],
	function($, _, Backbone, UserTaskModel) {

		var collection = Backbone.Collection.extend({

			model: UserTaskModel

		});

		return new collection;

	});