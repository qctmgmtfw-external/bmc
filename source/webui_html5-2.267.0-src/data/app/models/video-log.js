define(["jquery", "underscore", "backbone"],

    function($, _, Backbone) {

        var model = Backbone.Model.extend({

            defaults: {},

            validation: {

            }
        });

        return model;

    });