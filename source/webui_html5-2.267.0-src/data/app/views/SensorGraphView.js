define(['jquery', 'underscore', 'backbone', 'app',
        'i18n!strings/nls/common',
        'sparkline'
    ],
    function($, _, Backbone, app, CommonStrings) {

        var view = Backbone.View.extend({

            initialize: function() {
                this.model.bind("change", this.update, this);
            },

            events: {

            },

            beforeRender: function() {
                this.$el.addClass("sparkline-graph").html(CommonStrings.loading);
            },

            afterRender: function() {
                this.maxGraphPoints = 30;

                var context = this;

                //console.log(this.graphSettings);

                var settings = _.extend({
                    type: 'line',
                    tooltipSuffix: ' ' + context.model.unit()
                }, this.graphSettings);

                this.timer = setInterval(function() {
                    //context.readings.push(context.currentReading);
                    var sensor_readings = [];
                    if(app.readings['sensor_'+context.model.get('sensor_number')].length) {
                        sensor_readings = _.pluck(app.readings['sensor_' + context.model.get('sensor_number')], 'reading');
                    }
                    if (sensor_readings.length > context.maxGraphPoints) {
                        sensor_readings = sensor_readings.slice(sensor_readings.length - context.maxGraphPoints, sensor_readings.length);
                    }
                    // Quanta ---
                    //context.$el.sparkline(sensor_readings, _.extend(settings, { //AMI original
                    //    lineColor: (context.model.isCritical() ? '#fff' : 'lightblue')
                    //}));
                    var dup_sensor_readings = [];
                    for(var i=0; i<sensor_readings.length; i++){
                    	if(!isNaN(sensor_readings[i]))
                    		dup_sensor_readings.push(sensor_readings[i]);
                    }
                    context.$el.sparkline(dup_sensor_readings, _.extend(settings, { 
                        lineColor: 'lightblue' // #fff is same with background color.
                    }));
                    // --- Quanta
                }, 1000);
            },

            update: function() {}

        });

        return view;

    });
