define([
    "jquery",
    "underscore",
    "backbone",

    "i18n!strings/nls/common",
    //template
    'text!templates/systemlog-item.html'
], function($, _, Backbone, CommonStrings, SLITemplate) {

    var view = Backbone.View.extend({

        tagName: "li",

        template: _.template(SLITemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeView, this);
        },

        events: {
            "click": "toggleDetails"
        },

        beforeRender: function() {
            //this.$el.addClass("animated fadeInDown");
        },

        afterRender: function() {
            //this.$el.addClass("timeline-item-holder sensor_" + this.model.get('sensor_number') + " " + this.model.get('record_type'));
        },

        toggleDetails: function() {
            if (this.$el.find(".timeline-header").hasClass("expanded")) {
                this.$el.find(".timeline-header, .timeline-body, .timeline-footer").removeClass("expanded");
            } else {
                this.$el.find(".timeline-header, .timeline-body, .timeline-footer").addClass("expanded");
            }
        },

        removeView: function() {
            this.remove();
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
                "other_units_based_sensor": "ion ion-information-circled",
                "memory": "ion ion-information-circled",
                "drive_slot": "fa fa-hdd-o",
                "post_memory_resize": "ion-ios-settings-strong",
                "system_firmware_error": "fa fa-warning",
                "event_logging_disabled": "ion-android-note",
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
                "oem_reserved": ""
            };
                return sensor_icons[type] || "fa fa-dashboard";
        },

        serialize: function() {
            return {
                locale: CommonStrings,
                datetime: moment(this.model.get('timestamp'), 'X').format("MMMM Do YYYY, h:mm:ss a") + "  ", 
                hostname: "  " +this.model.get('hostname') + "  ",
                logdescription: "  " +this.model.get('message'),
                fromnow: moment(this.model.get('timestamp'), 'X').fromNow(),
                cause: this.model.cause(),
                event: this.model.event(),
                event_message: this.model.get('message') + " on " + moment(this.model.get('timestamp'), 'X').format("dddd, MMMM Do YYYY, h:mm:ss a"), //"Reading went lower than Lower Critical Threshold"
                log_icon: "fa fa-flag" //this.getIcon(this.model.get('sensor_type')),
            };
        }

    });

    return view;

});