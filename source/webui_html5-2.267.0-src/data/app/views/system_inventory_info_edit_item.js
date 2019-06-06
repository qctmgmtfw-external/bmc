define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//system_inventory_device_info', 'collection//system_inventory_cpu_info', 'collection//system_inventory_dimm_info', 'collection//system_inventory_pci_info',
//localize
'i18n!strings/nls/system_inventory_info',
//template
'text!templates//system_inventory_info_edit_item.html',
//plugins

],

function($, _, Backbone, app, SystemInventoryDeviceInfoCollection, SystemInventoryCpuInfoCollection, SystemInventoryDimmInfoCollection, SystemInventoryPciInfoCollection, locale, SystemInventoryInfoEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(SystemInventoryInfoEditItemTemplate),

        initialize: function() {
            SystemInventoryDeviceInfoCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = SystemInventoryDeviceInfoCollection.get(id);
            } else {
                this.model = new SystemInventoryDeviceInfoCollection.model;
                this.model.url = SystemInventoryDeviceInfoCollection.url;
            };


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
            /*context.model.save({ 
            }, {
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
            });*/
        },

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
