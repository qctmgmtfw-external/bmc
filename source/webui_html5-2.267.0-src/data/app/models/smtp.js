define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/smtp"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            email_id: {
                pattern:'email',
                msg: locale.smtp_invalid_email_format_refer_help_content
            },
            primary_smtp_enable: {
                fn: function(value, attr, computedState) {
                    if(value < 1 && (computedState.primary_server_ip == "" || (!(/((^\s*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))\s*$)|(^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$))/.test(computedState.primary_server_ip))))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.primary_server_ip_is_required_valid
            },
            primary_server_name: {
                fn: function(value, attr, computedState) {
                    if(computedState.primary_smtp_enable == 1 && (!((/^[a-zA-Z0-9\.\-]*$/.test(value)) && value!="" && value.length<=25))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.primary_server_name_is_required
            },
            primary_smtp_port: {
                fn: function(value, attr, computedState) {
                    if (computedState.primary_smtp_enable == 1 && (!(value >= 1 && value <= 65535))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.primary_server_port_is_required
            },
            primary_smtp_secure_port: {
                fn: function(value, attr, computedState) {
                    if (computedState.primary_smtp_enable == 1 && computedState.primary_ssltls_enable == 1 && (!(value >= 1 && value <= 65535))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.primary_server_secure_port_is_required
            },

            primary_server_ip: {
                fn: function(value, attr, computedState) {
                    if (computedState.primary_smtp_enable == 1 &&  (!(/((^\s*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))\s*$)|(^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$))/.test(value)))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.primary_server_ip_is_required_valid
            },
            primary_username: {
                fn: function(value, attr, computedState) {
                    if (computedState.primary_smtp_enable == 1 && computedState.primary_smtp_authentication == 1 && (!(/(^[a-zA-Z][a-zA-Z0-9.~@#$^()_+=[\]{}/-]*$)/.test(value)) || !(value.length >= 4 && value.length <= 64))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.username_for_primary_server_is_required
            },
            primary_password: {
                fn: function(value, attr, computedState) {
                    if (computedState.primary_smtp_enable == 1 && computedState.primary_smtp_authentication == 1 && ((/\s/g.test(value)) || !(value.length >= 4))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.password_for_primary_server_is_required
            },
            secondary_server_name: {
                fn: function(value, attr, computedState) {
                    if(computedState.secondary_smtp_enable == 1 && (!((/^[a-zA-Z0-9\.\-]*$/.test(value)) && value!="" && value.length<=25))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.secondary_server_name_is_required
            },
            secondary_smtp_port: {
                fn: function(value, attr, computedState) {
                    if (computedState.secondary_smtp_enable == 1 && (!(value >= 1 && value <= 65535))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.secondary_server_port_is_required
            },
            secondary_smtp_secure_port: {
                fn: function(value, attr, computedState) {
                    if (computedState.secondary_smtp_enable == 1 && computedState.secondary_ssltls_enable == 1 && (!(value >= 1 && value <= 65535))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.secondary_server_secure_port_is_required
            },
            secondary_server_ip: {
                fn: function(value, attr, computedState) {
                    if (computedState.secondary_smtp_enable == 1 &&  (!(/((^\s*((([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]))\s*$)|(^\s*((([0-9A-Fa-f]{1,4}:){7}([0-9A-Fa-f]{1,4}|:))|(([0-9A-Fa-f]{1,4}:){6}(:[0-9A-Fa-f]{1,4}|((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){5}(((:[0-9A-Fa-f]{1,4}){1,2})|:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3})|:))|(([0-9A-Fa-f]{1,4}:){4}(((:[0-9A-Fa-f]{1,4}){1,3})|((:[0-9A-Fa-f]{1,4})?:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){3}(((:[0-9A-Fa-f]{1,4}){1,4})|((:[0-9A-Fa-f]{1,4}){0,2}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){2}(((:[0-9A-Fa-f]{1,4}){1,5})|((:[0-9A-Fa-f]{1,4}){0,3}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(([0-9A-Fa-f]{1,4}:){1}(((:[0-9A-Fa-f]{1,4}){1,6})|((:[0-9A-Fa-f]{1,4}){0,4}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:))|(:(((:[0-9A-Fa-f]{1,4}){1,7})|((:[0-9A-Fa-f]{1,4}){0,5}:((25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)(\.(25[0-5]|2[0-4]\d|1\d\d|[1-9]?\d)){3}))|:)))(%.+)?\s*$))/.test(value)))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.secondary_server_ip_is_required
            },
            secondary_username: {
                fn: function(value, attr, computedState) {
                    if (computedState.secondary_smtp_enable == 1 && computedState.secondary_smtp_authentication == 1 && (!(/(^[a-zA-Z][a-zA-Z0-9.~@#$^()_+=[\]{}/-]*$)/.test(value)) || !(value.length >= 4 && value.length <= 64))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.username_for_secondary_server_is_required
            },
            secondary_password: {
                fn: function(value, attr, computedState) {
                    if (computedState.secondary_smtp_enable == 1 && computedState.secondary_smtp_authentication == 1 && ((/\s/g.test(value)) || !(value.length >= 4))) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.password_for_secondary_server_is_required
            }
        }
    });

    return model;

});
