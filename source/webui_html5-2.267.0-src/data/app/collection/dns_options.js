define(["jquery", "underscore", "backbone", "models//dns_option"],

function($, _, Backbone, DnsOptionModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/dns/server-options"
        },

        model: DnsOptionModel

    });

    return new collection();

});
