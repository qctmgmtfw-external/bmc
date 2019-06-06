define(['jquery', 'underscore', 'backbone',
        //data

        //localize
        'i18n!strings/nls/common',
        //template
        'text!templates/disabled-sensor.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, CommonStrings, SensorTemplate) {

        var view = Backbone.View.extend({

            template: _.template(SensorTemplate),

            initialize: function() {

            },

            events: {},

            beforeRender: function() {
                this.$el.addClass("noscan-sensor col-md-4 col-lg-3").attr("id", "disabled-sensor-" + this.model.get('sensor_number'));
            },

            afterRender: function() {

            },

            load: function(model, collection, xhr) {

            },

            serialize: function() {
                return {
                    locale: CommonStrings,
                    name: this.model.get('name')
                };
            }

        });

        return view;

    });