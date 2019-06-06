define([
    "jquery",
    "underscore",
    "backbone",

    "i18n!strings/nls/common",
    //template
    'text!templates/timeline-date.html',
    "moment"
], function($, _, Backbone, CommonStrings, TLDTemplate, moment) {

    var view = Backbone.View.extend({

        tagName: "li",

        template: _.template(TLDTemplate),

        initialize: function() {

        },

        events: {

        },

        beforeRender: function() {
            this.$el.addClass("time-label"); // animated fadeInDown
        },

        afterRender: function() {

        },

        serialize: function() {
            return {
                locale: CommonStrings,
                date: this.date
            };
        }

    });

    return view;

});