define(['jquery', 'underscore', 'backbone'],
    function($, _, Backbone) {

        var model = Backbone.Model.extend({

            url: "/api/system-info"

        });

        return new model();

    });
