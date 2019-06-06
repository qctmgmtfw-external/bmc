define(['jquery', 'underscore', 'backbone'],
    function($, _, Backbone) {

        var model = Backbone.Model.extend({

            url: "/api/settings/user-preference"

        });

        return new model();

    });