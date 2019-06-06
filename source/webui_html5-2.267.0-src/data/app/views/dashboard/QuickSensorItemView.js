define(['jquery', 'underscore', 'backbone', 'app',
        'collection/event-log',
        'views/SensorGraphView',
        'text!templates/dashboard/sensor-item.html',
        "i18n!strings/nls/dashboard",
        "moment",
        "sparkline"
    ],
    function($, _, Backbone, app, EventLogCollection, SensorGraphView, QuickSensorItemTemplate, CommonStrings, moment) {

        var view = Backbone.View.extend({

            template: _.template(QuickSensorItemTemplate),

            initialize: function() {
                this.model.bind("change:reading", this.updateReading, this);
            },

            events: {
                "click": "showDetails"
            },

            beforeRender: function() {
                this.$el.addClass("row qsi");
            },

            afterRender: function() {
                this.$el.attr("id", "qsi_" + this.model.get('id'));

                this.$el.find(".qsi-name").html(this.model.name());
                //this.$el.find(".qsi-reading").html(this.model.reading() + " " + this.model.unit());

                this.setView(".qsi-graph", new SensorGraphView({
                    model: this.model,
                    graphSettings: {
                        lineColor: (this.model.isCritical() ? '#fff' : 'lightblue'),
                        fillColor: 'rgba(255,255,255,.2)',
                        width: '120px'
                    }
                })).render();

                this.updateReading.call(this, this.model, this.model.get('reading'));
            },

            showDetails: function(){
                app.router.navigate("sensors/"+this.model.get('sensor_number'), {trigger: true});
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

            updateReading: function(model, value) {
                this.$el.find(".qsi-reading").html(model.reading() + " " + model.unit());

                var context = this;

                var lastEvent = EventLogCollection.find(function(eventModel){
                   return eventModel.get('sensor_number') == model.get('sensor_number');
                });

                if(lastEvent) {
                    context.$el.find(".qsi-time").html(lastEvent.description());
                }
/*
                app.db.from('ipmi_events').where('sensor_number', '=', model.get('sensor_number'))
                    .order('timestamp').reverse()
                    .list()
                    .done(function(results) {
                        if (results.length) {
                            context.$el.find(".qsi-time").html(results[0].event_description + " for " + moment(results[0].timestamp).fromNow(true));
                        }
                    });
*/
                if (model.isNormal()) {
                    //var sgv = this.getView(".qsi-graph");
                    //clearInterval(sgv.timer);
                    //this.remove();
                    //move it recently recovered list
                    this.$el.find(".knob-label").addClass("knob-label-dark").removeClass("knob-label");
                    this.$el.appendTo("#sensor-dashboard  .box-footer");
                    $("#sensor-dashboard").find("h4 small").html(" (" + $("#sensor-dashboard  .box-footer .row").length + " recovered)");
                }
            },

            serialize: function() {
                return {
                    locale: CommonStrings,
                    sensor_icon: this.getIcon(this.model.sensorType().toString())
                };
            }

        });

        return view;

    });
