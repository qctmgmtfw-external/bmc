define(["jquery", "underscore", "backbone", "models//domain_option"],

function($, _, Backbone, DomainOptionModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/dns/domain-options"
        },

        model: DomainOptionModel

    });

    return new collection();

});
