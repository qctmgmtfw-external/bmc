define([
    "jquery",
    "underscore",
    "backbone",
    "collection/sensors",

    "i18n!strings/nls/common",
    //template
    'text!templates/timeline-item.html',
    'moment',
], function($, _, Backbone, SensorCollection, CommonStrings, TLITemplate, moment) {

    var view = Backbone.View.extend({

        tagName: "li",

        template: _.template(TLITemplate),

        initialize: function() {
        },

        events: {
            "click": "toggleDetails"
        },

        beforeRender: function() {
            //this.$el.addClass("animated fadeInDown");
        },

        afterRender: function() {
            this.$el.addClass("timeline-item-holder sensor_" + this.model.get('sensor_number') + " " + this.model.get('record_type'));
        },

        toggleDetails: function() {
            if (this.$el.find(".timeline-header").hasClass("expanded")) {
                this.$el.find(".timeline-header, .timeline-body, .timeline-footer").removeClass("expanded");
            } else {
                this.$el.find(".timeline-header, .timeline-body, .timeline-footer").addClass("expanded");
                if ($('.timeZone:checked').val() == '1'){
                    $(".client_time").hide();
                    $(".bmc_time").show();
                }
                if ($('.timeZone:checked').val() == '2'){
                    $(".bmc_time").hide();
                    $(".client_time").show();
                }
            }
        },

        getIcon: function(type) {
            var sensor_icons = {
                "temperature": "ion ion-thermometer",
                "voltage": "ion ion-ios-pulse-strong",
                "current": "ion ion-ios-bolt",
                "fan": "ion ion-nuclear",
                "chassis_intrusion": "fa fa-server",
                "physical_security":"fa fa-server",
                "platform_security_violation": "ion ion-unlocked",
                "platform_security": "ion ion-unlocked",
                "processor": "ion ion-code-working",
                "power_supply": "ion ion-outlet",
                "power_unit": "ion ion-power",
                "cooling_device": "ion ion-ios-snowy",
                "other_units": "ion ion-information-circled",
                    "other_units_based_sensor": "ion-information-circled",
                "memory": "ion ion-information-circled",
                "drive_slot": "fa fa-hdd-o",
                "post_memory_resize": "ion ion-ios-settings-strong",
                "system_firmware_error": "fa fa-warning",
                "event_logging_disabled": "ion ion-disc",
                "watchdog": "ion ion-eye",
                "watchdog_2": "ion ion-eye",
                "watchdog_1": "ion ion-eye",
                "system_event": "ion ion-flag",
                "critical_interrupt": "ion ion-close-circled",
                "button": "ion ion-toggle-filled",
                "board": "ion ion-information-circled",
                "microcontroller": "ion ion-information-circled",
                "add_in_card": "fa fa-plus",
                "chassis": "fa fa-server",
                "chipset": "ion ion-information-circled",
                "chip_set": "ion ion-information-circled",
                "other_fru": "ion ion-information-circled",
                "cable": "ion ion-information-circled",
                "cable_or_interconnect": "ion ion-information-circled",
                "terminator": "fa fa-minus-circle",
                "system_boot": "fa fa-terminal",
                "system_boot_or_restart_initiated": "fa fa-terminal",
                "boot_error": "fa fa-times-circle",
                "base_os_boot": "fa fa-terminal",
                "os_boot": "fa fa-terminal",
                "os_shutdown": "ion ion-power",
                "slot": "fa fa-navicon",
                "system_acpi": "ion ion-power",
                "platform_alert": "fa fa-bell",
                "entity_presence": "fa fa-check-circle",
                "monitor_asic": "ion ion-ios-search-strong",
                "monitor_asic_or_ic": "ion ion-ios-search-strong",
                "lan": "fa fa-sitemap",
                "management_subsystem_health": "fa fa-medkit",
                "battery": "ion ion-battery-half",
                "session_audit": "fa fa-user",
                "version_change": "ion ion-information-circled",
                "fru_state": "ion ion-help-circled",
                "oem_reserved": "",
                "Unspecified" :"fa fa-question-circle bg-grey",
                "Monitor":"fa fa-info-circle bg-blue",
                "Information":"fa fa-info-circle bg-blue",
                "OK":"fa fa-check-circle bg-green",
                "Warning":"fa fa-exclamation-triangle bg-yellow",
                "Critical":"fa fa-times bg-red",
                "Recoverable":"ion-sad bg-black"
            };
            return sensor_icons[type] || "fa fa-dashboard";
        },

        serialize: function() {
            var sensor = SensorCollection.getSensor(this.model.get('sensor_number'));

            var cause = (sensor? sensor.name(): this.model.get('sensor_name') || "Unknown Component");
            if((this.model.get('system_software_type')=="oem_record")&&(this.model.get('sensor_number')==0))
            {
                cause = this.model.get('sensor_name');
            }

            var ad = this.model.get('advanced_event_description');

            var localtime  = new Date(this.model.get('timestamp')*1000);

            var offset = this.model.get('utc_offset');
            var timezoneoffset = new Date().getTimezoneOffset();
            return {
                recordid : this.model.get('id'),
                locale: CommonStrings,
                Client_fromnow: moment((this.model.get('timestamp')*1000)-(offset*60*1000)).fromNow(),
                BMC_fromnow: moment(this.model.get('timestamp')*1000).fromNow(),
                cause: cause,
                sensor_type: this.model.get('sensor_type'),
                event: this.model.get('event') ,
                sensor_type: this.model.get('sensor_type'),
                //event: this.model.description(),
                event_message: this.model.get('event_direction') + ' ' + ((ad!='unknown')?ad:'') + " on ",
                bmc_time: moment((this.model.get('timestamp')*1000)+(new Date().getTimezoneOffset()*60*1000)).format("dddd, MMMM Do YYYY, h:mm:ss a"),
                client_time: moment(this.model.get('timestamp')*1000-offset*1000).format("dddd, MMMM Do YYYY, h:mm:ss a"),
                sensor_icon: this.getIcon(this.model.get('sensor_type')),
                sensor_number: this.model.get('sensor_number'),
                record_type: this.model.get('record_type'),
                severity_icon : this.getIcon(this.model.get('severity'))
            };
        }
    });

    return view;

});
