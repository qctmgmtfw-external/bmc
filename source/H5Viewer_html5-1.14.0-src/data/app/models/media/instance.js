define(["jquery", "underscore", "backbone", "app", "i18n!strings/nls/instance"],

function($, _, Backbone, app, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {
            kvm_num_fd: {
                fn: function(value, attr, computedState) {
                    if (value <= computedState.num_fd) {
                        return false;
                    } else {
                        return true;
                    }
                },
                msg: locale.remote_kvm_floppy_device_instance_value_should_be_less_than_or_equal_to_virtual_floppy_device_count
            },
            kvm_num_cd: {
                fn: function(value, attr, computedState) {
                    if (value <= computedState.num_cd) {
                        return false;
                    } else {
                        return true;
                    }
                },
                msg: locale.remote_kvm_cddvd_device_instance_value_should_be_less_than_or_equal_to_virtual_cddvd_device_count
            },
            kvm_num_hd: {
                fn: function(value, attr, computedState) {
                    if (value <= computedState.num_hd) {
                        return false;
                    } else {
                        return true;
                    }
                },
                msg: locale.remote_kvm_hard_disk_instance_value_should_be_less_than_or_equal_to_virtual_hard_disk_count
            }
        },

        url: function() {
            return "/api/settings/media/instance"
        }
    });

    return new model();

});
