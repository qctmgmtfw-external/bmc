define(["jquery", "underscore", "backbone", "models//dns_registration"],

function($, _, Backbone, DnsRegistrationModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/dns/registration"
        },

        model: DnsRegistrationModel

    });

    return new collection();

});
