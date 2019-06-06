define(["jquery", "underscore", "backbone", "models//script"],

function($, _, Backbone, ScriptModel) {

    var collection = Backbone.Collection.extend({

        url: "/api/scripts",

        model: ScriptModel

    });

    return new collection();

});
