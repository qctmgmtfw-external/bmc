define(['jquery', 'underscore', 'backbone', 'app',
        'collection/sensors',
        'views/dashboard/QuickSensorItemView',
        'text!templates/dashboard/sensor.html',
        "i18n!strings/nls/dashboard"
    ],
    function($, _, Backbone, app, SensorCollection, QuickSensorItemView, QuickSensorTemplate, CommonStrings) {

        var view = Backbone.View.extend({

            template: _.template(QuickSensorTemplate),

            initialize: function() {
                this.collection = SensorCollection;
                this.collection.bind("add reset remove change", this.loaded, this);

                app.on("beforeLogout", this.clearTimer, this);
            },

            beforeRender: function() {

            },

            afterRender: function() {
                var context = this;
                //this.timer = setInterval(function() {
                //    context.collection.fetch();
                //}, 2500);
                this.$el.find("#sensor-dashboard").find('.box-body').html("");
                this.loaded.call(this);
            },

            clearTimer: function() {
                clearInterval(this.timer);
            },

            loaded: function() {
                this.collection.each(function(model) {
                    if (!app.readings) app.readings = {};

                    if (!app.readings['sensor_' + model.get('sensor_number')]) {
                        app.readings['sensor_' + model.get('sensor_number')] = [];
                    }
                    app.readings['sensor_' + model.get('sensor_number')].push(model.reading());
                });

                var criticalSensors = this.collection.getCriticalSensors();
                var sensorDashboard = this.$el.find("#sensor-dashboard");
                var context = this;

                //console.log('no of crit sensors ' + criticalSensors.length);

                if (criticalSensors.length) {
                    this.$el.find(".icon").removeClass("fa-check-circle");
                    this.$el.find(".icon").addClass("fa-warning");
                    if (sensorDashboard.hasClass("success")) {
                        sensorDashboard.find('.box-body').html("");
                    }
                    sensorDashboard.removeClass("box-success success");
                    sensorDashboard.addClass("box-danger");
                    //console.log(criticalSensors);
                    sensorDashboard.find("h3 small").html(" - " + criticalSensors.length +" "+ CommonStrings.critical_sensors);
                    criticalSensors.each(function(model) {

                        if ($("#qsi_" + model.get('id')).length) {
                            if ($("#qsi_" + model.get('id')).parent().hasClass("box-body")) {
                                //console.log('skipped', $("#qsi_" + model.get('id')).parent());
                                return;
                            } else if ($("#qsi_" + model.get('id')).parent().hasClass("box-footer")) {
                                $("#qsi_" + model.get('id')).find(".knob-label-dark").addClass("knob-label").removeClass("knob-label-dark");
                                $("#qsi_" + model.get('id')).appendTo("#sensor-dashboard  .box-body");
                                $("#sensor-dashboard").find("h4 small").html(" (" + $("#sensor-dashboard  .box-footer .row").length + " recovered)");
                            }
                            return;
                        }

                        context.insertView('.box-body', new QuickSensorItemView({
                            model: model
                        })).render();
                    });


                } else {
                    this.$el.find(".icon").removeClass("fa-warning");
                    this.$el.find(".icon").addClass("fa-check-circle");
                    sensorDashboard.removeClass("box-danger");
                    sensorDashboard.addClass("box-success success");
                    sensorDashboard.find("h3 small").html("");
                    sensorDashboard.find('.box-body').html('<div class="row">' +
                        '<div class="col-xs-12">' +
                        '<div class="knob-label text-center">'+CommonStrings.allsensor_goodnow+'</div>' +
                        '</div>' +
                        '</div>');

                }


            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });
