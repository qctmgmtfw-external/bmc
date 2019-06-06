define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/services"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            time_out: {
                fn: function(value, attr, computedState) {
                    if (computedState.state == 1 && value != -1 && value != '2147483647' && (parseInt(value, 10) % 60 != 0 || ((computedState.service_name == 'web' || computedState.service_name == 'kvm') && !(value >= 300 && value <= 1800) ) || ((computedState.service_name == 'ssh' || computedState.service_name == 'telnet' || computedState.service_name == 'solssh') && (!(value >= 60 && value <= /*1800*/480))))) { //Quanta
                		return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.invalid_time_out_refer_help_for_more_information
            },
            non_secure_port: {
                fn: function(value, attr, computedState) {
                    if (computedState.state == 1 &&  value != -1 && value !== null &&  value != 'N/A' &&  value != 0xFFFFFFFF && value != '2147483647' && (value == "" || !(value >= 1 && value <= 65535)))
                    {
                        return true;
                    } 
                    else {
                        return false;
                    }
                },
                msg: locale.invalid_non_secure_port_refer_help_for_more_information
            },
            secure_port: {
                fn: function(value, attr, computedState) {
                    if (computedState.state == 1 && value != -1 && value !== null &&  value != 'N/A' &&  value != 0xFFFFFFFF && value != '2147483647' && (value == "" || !(value >= 1 && value <= 65535)))
                    {
                        return true;
                    } 
                    else {
                        return false;
                    }
                },
                msg: locale.invalid_secure_port_refer_help_for_more_information
            }
        }
    });

    return model;

});
