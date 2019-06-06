define(["jquery", "underscore", "backbone", "i18n!strings/nls/users"],

function($, _, Backbone, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            name: {
                required: true,
                pattern: /^[a-zA-Z][a-zA-Z0-9\-\_\@\.]+$/,
                maxLength: 16,
                msg: locale.invalid_username_refer_help_content
            },
            password: {
                fn: function(value, attr, computedState) {
                    //if( (computedState.changepassword == 1 && !(value.length>=8) ) || (computedState.snmp == 1 && !(value.length>=8) ) ){
                    if( (computedState.changepassword == 1 && !(/^[\S]{1,}$/.test(value) ) ) /*|| (computedState.snmp == 1 && computedState.prev_snmp != 1 && !(/^[\S]{8,}$/.test(value) ) )*/ ){
                        return true;
                    } else if( (computedState.changepassword == 1 && computedState.password_size == 'bytes_16' && (value.length>16) )){
                        return true;
                    } else if( (computedState.changepassword == 1 && computedState.password_size == 'bytes_20' && (value.length>20) )) {
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.password_field_must_be_8_characters_atleast
            },
            confirm_password: {
                //equalTo: 'password',
                fn: function(value, attr, computedState) {
                    console.log(computedState);
                    if(computedState.changepassword == 1 && !(computedState.password == value)){
                        return true;
                    } else if( (computedState.changepassword == 1 && computedState.password_size == 'bytes_16' && (value.length>16) )){
                        return true;
                    } else if( (computedState.changepassword == 1 && computedState.password_size == 'bytes_20' && (value.length>20) )) {
                        return true;
                    } 
                    else {
                        return false;
                    }
                },
                msg: locale.passwords_do_not_match
            },
            email_id: {
                fn: function(value, attr, computedState) {
                    if(value != "" && (!(/^((([a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+(\.([a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+)*)|((\x22)((((\x20|\x09)*(\x0d\x0a))?(\x20|\x09)+)?(([\x01-\x08\x0b\x0c\x0e-\x1f\x7f]|\x21|[\x23-\x5b]|[\x5d-\x7e]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(\\([\x01-\x09\x0b\x0c\x0d-\x7f]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]))))*(((\x20|\x09)*(\x0d\x0a))?(\x20|\x09)+)?(\x22)))@((([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.)+(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))$/i.test(value)))){
                        return true;
                    } else {
                        return false;
                    }
                },
                msg: locale.invalid_email_format_refer_help_content
            }
        }
    });

    return model;

});
