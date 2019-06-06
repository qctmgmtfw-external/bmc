define(['jquery', 'underscore', 'backbone', 'app',
        'views/SensorGraphView',
        //data

        //localize
        'i18n!strings/nls/common',
        //template
        'text!templates/normal-sensor.html',
        //plugins
        // 'moment',
        // 'knob',
        'sparkline'
    ],

    function($, _, Backbone, app, SensorGraphView, CommonStrings, SensorTemplate) {

        var view = Backbone.View.extend({

            tagName: "tr",

            template: _.template(SensorTemplate),

            initialize: function() {
                this.model.bind("change:reading", this.updateReading, this);
                this.model.bind("change:sensor_state", this.updateState, this);
            },

            events: {
                "click": "showDetails"
            },

            beforeRender: function() {
                this.$el.attr("id", "normal-sensor-" + this.model.get('sensor_number'));
            },

            afterRender: function() {
                this.setView(".behavior", new SensorGraphView({
                    model: this.model,
                    graphSettings: {}
                })).render();
                // this.maxGraphPoints = 30;

                // var context = this;

                // this.timer = setInterval(function() {
                //     //context.readings.push(context.currentReading);
                //     var sensor_readings = app.readings['sensor_' + context.model.get('sensor_number')];

                //     if (sensor_readings.length > context.maxGraphPoints) {
                //         sensor_readings = sensor_readings.slice(sensor_readings.length - context.maxGraphPoints, sensor_readings.length);
                //     }

                //     context.$el.find(".sparkline-graph").sparkline(sensor_readings, {
                //         type: 'line',
                //         tooltipSuffix: ' ' + context.model.unit()
                //     });
                // }, 2500);
                // $("#normal-sensor-content .sparkline-graph").sparkline([1, 10, 9, -1, -5, 5, 7, 0, 0, 3, -1, 8, 1, 1, 10, 9, -1, -5, 5, 7, 0, 0, 3, -1, 8, 1], {
                //     type: 'line'
                // });
            },

            load: function(model, collection, xhr) {

            },

            showDetails: function() {
                app.router.navigate("sensors/"+this.model.get('sensor_number'), {trigger: true});
            },

            updateReading: function(model, value, xhr) {
                this.$el.find(".reading").html(model.reading());
                this.currentReading = model.reading();
            },

            updateState: function(model, value, xhr) {
                if (model.get('sensor_state') >= 0x02) {
                    clearInterval(this.timer);
                    this.remove();
                }
            },

            getIcon: function(type) {
                var sensor_icons = {
                    "temperature": "ion-thermometer",
                    "voltage": "ion-ios-pulse-strong",
                    "current": "ion-ios-bolt",
                    "fan": "ion-nuclear",
                    "chassis_intrusion": "fa fa-server",
                    "physical_security":"fa fa-server",
                    "platform_security_violation": "ion-unlocked",
                    "platform_security": "ion-unlocked",
                    "processor": "ion-code-working",
                    "power_supply": "ion-outlet",
                    "power_unit": "ion-power",
                    "cooling_device": "ion-ios-snowy",
                    "other_units": "ion-information-circled",
                    "other_units_based_sensor": "ion-information-circled",
                    "memory": "ion-information-circled",
                    "drive_slot": "fa fa-hdd-o",
                    "post_memory_resize": "ion-ios-settings-strong",
                    "system_firmware_error": "fa fa-warning",
                    "event_logging_disabled": "ion-ios-paper",
                    "watchdog": "ion-eye",
                    "watchdog_2": "ion-eye",
                    "watchdog_1": "ion-eye",
                    "system_event": "ion-flag",
                    "critical_interrupt": "ion-close-circled",
                    "button": "ion-toggle-filled",
                    "board": "ion-information-circled",
                    "microcontroller": "ion-information-circled",
                    "add_in_card": "fa fa-plus",
                    "chassis": "fa fa-server",
                    "chipset": "ion-information-circled",
                    "chip_set": "ion-information-circled",
                    "other_fru": "ion-information-circled",
                    "cable": "ion-information-circled",
                    "cable_or_interconnect": "ion-information-circled",
                    "terminator": "fa fa-minus-circle",
                    "system_boot": "fa fa-terminal",
                    "system_boot_or_restart_initiated": "fa fa-terminal",
                    "boot_error": "fa fa-times-circle",
                    "base_os_boot": "fa fa-terminal",
                    "os_boot": "fa fa-terminal",
                    "os_shutdown": "ion-power",
                    "slot": "fa fa-navicon",
                    "system_acpi": "ion-power",
                    "platform_alert": "fa fa-bell",
                    "entity_presence": "fa fa-check-circle",
                    "monitor_asic": "ion-ios-search-strong",
                    "monitor_asic_or_ic": "ion-ios-search-strong",
                    "lan": "fa fa-sitemap",
                    "management_subsystem_health": "fa fa-medkit",
                    "battery": "ion-battery-half",
                    "session_audit": "fa fa-user",
                    "version_change": "ion-information-circled",
                    "fru_state": "ion-help-circled",
                    "oem_reserved": ""
                };
                return sensor_icons[type] || "fa fa-dashboard";
            },

            serialize: function() {
                this.currentReading = this.model.reading();
                return {
                    locale: CommonStrings,
                    name: this.model.name(),
                    reading: this.model.reading(),
                    unit: this.model.unit(),
                    sensor_type_icon: this.getIcon(this.model.sensorType().toString())
                };
            }

        });

        return view;

    });
