define(['jquery', 'underscore', 'backbone',
        'models/fru'
    ],
    function($, _, Backbone, FRUModel) {

        var collection = Backbone.Collection.extend({
            url: "/api/fru",

            model: FRUModel
        });

        return new collection();

    });