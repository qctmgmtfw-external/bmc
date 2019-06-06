define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//physical_device_info',
//localize
'i18n!strings/nls/physical_device_info',
//template
'text!templates/settings/raid_management/physical_device_info_edit_item.html',
//plugins

],

function($, _, Backbone, app, PhysicalDeviceInfoCollection, locale, PhysicalDeviceInfoEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(PhysicalDeviceInfoEditItemTemplate),

        initialize: function() {
            PhysicalDeviceInfoCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = PhysicalDeviceInfoCollection.get(id);
            } else {
                this.model = new PhysicalDeviceInfoCollection.model;
                this.model.url = PhysicalDeviceInfoCollection.url;
            };
            this.model.bind('change:controller_id', this.updateControllerId, this);
            this.model.bind('change:controller_name', this.updateControllerName, this);
            this.model.bind('change:dev_id', this.updateDevId, this);
            this.model.bind('change:type', this.updateType, this);
            this.model.bind('change:state', this.updateState, this);
            this.model.bind('change:vendor_id', this.updateVendorId, this);
            this.model.bind('change:product_id', this.updateProductId, this);
            this.model.bind('change:serial_num', this.updateSerialNum, this);
            this.model.bind('change:slot', this.updateSlot, this);
            this.model.bind('change:interface_type', this.updateInterfaceType, this);
            this.model.bind('change:cache', this.updateCache, this);
            this.model.bind('change:speed', this.updateSpeed, this);
            this.model.bind('change:size', this.updateSize, this);
            this.model.bind('change:link_speed', this.updateLinkSpeed, this);
            this.model.bind('change:power_sate', this.updatePowerSate, this);
            this.model.bind('change:temperature', this.updateTemperature, this);


        },

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {


            //this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, this.model, value);
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

        toggleHelp: function(e) {
            e.preventDefault();
            var help_items = this.$('.help-item').filter(function() {
                var f = $(this).data('feature');
                var nf = $(this).data('not-feature');
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
                } else if (nf) {
                    return (app.features.indexOf(nf) == -1 ? true : false);
                } else {
                    return true;
                }
            });
            $(".tooltip").hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        updateControllerId: function(model, value) {},
        updateControllerName: function(model, value) {},
        updateDevId: function(model, value) {
            $('#iddev_id').html(value);
        },
        updateType: function(model, value) {},
        updateState: function(model, value) {},
        updateVendorId: function(model, value) {
            $('#idvendor_id').html(value);
        },
        updateProductId: function(model, value) {
            $('#idproduct_id').html(value);
        },
        updateSerialNum: function(model, value) {
            $('#idserial_num').html(value);
        },
        updateSlot: function(model, value) {},
        updateInterfaceType: function(model, value) {
            var result="";
            switch  (value) {
                case 0 :
                    result=locale.interface_type_0;
                    break;
                case 1 :
                    result=locale.interface_type_1;
                    break;
                case 2 :
                    result=locale.interface_type_2;
                    break;
                case 3 :
                    result=locale.interface_type_3;
                    break;
                case 4 :
                    result=locale.interface_type_4;
                    break;
                default:
                    result=value;
                    break;
            }
            $('#idinterface_type').html(result);
        },
        updateCache: function(model, value) {},
        updateSpeed: function(model, value) {},
        updateSize: function(model, value) {},
        updateLinkSpeed: function(model, value) {},
        updatePowerSate: function(model, value) {
            var ps_state="";
            if(value==0){
                ps_state=locale.ps_state_0;
            } else if(value==1){
                ps_state=locale.ps_state_1;
            }else {
                ps_state=value;
            }
            $('#idpower_sate').html(ps_state);
        },
        updateTemperature: function(model, value) {},

        validData: function(model) {
            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }
        },

        errorFields: [],

        invalidData: function(model, errors) {
            console.log('invalid', errors);
            var elMap = [],
                jel;

            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }

            for (var e in errors) {
                jel = this.$el.find("#" + elMap[e]);
                jel.tooltip({
                    animation: false,
                    title: errors[e],
                    trigger: 'manual',
                    placement: 'top'
                });
                jel.tooltip('show');
                this.errorFields.push(jel);


            }
            $("#save-icon").removeClass().addClass("fa fa-save");
        },

        save: function() {


            $("#save-icon").removeClass().addClass("ion-loading-c");
            this.$(".alert-success,.alert-danger").addClass("hide");


            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }
            context.model.save({}, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', context);
                    var back_url = location.hash.split('/').slice(0, - 2);
                    //back_url.pop();
                    app.router.navigate(back_url.join('/'), {
                        trigger: true
                    });
                },

                error: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', context);
                }
            });


        },

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
