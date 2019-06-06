define(['jquery', 'underscore', 'backbone'],
    function($, _, Backbone) {

        var model = Backbone.Model.extend({

            url: "/api/status/uptime"

        });

        return new model();

    });
