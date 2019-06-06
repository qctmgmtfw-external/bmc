define(["jquery", "underscore", "backbone", "models//task"],

function($, _, Backbone, TaskModel) {

    var collection = Backbone.Collection.extend({

        url: "/api/tasks",

        model: TaskModel

    });

    return new collection();

});
