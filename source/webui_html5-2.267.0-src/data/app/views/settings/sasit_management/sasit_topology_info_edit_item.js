define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//sasit_topology_info',
//localize
'i18n!strings/nls/sasit_topology_info',
//template
'text!templates/settings/sasit_management/sasit_topology_info_edit_item.html',
//plugins

],

function($, _, Backbone, app, SasitTopologyInfoCollection, locale, SasitTopologyInfoEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(SasitTopologyInfoEditItemTemplate),

        initialize: function() {
            SasitTopologyInfoCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = SasitTopologyInfoCollection.get(id);
            } else {
                this.model = new SasitTopologyInfoCollection.model;
                this.model.url = SasitTopologyInfoCollection.url;
            };
            this.model.bind('change:controller_id', this.updateControllerId, this);
            this.model.bind('change:controller_name', this.updateControllerName, this);
            this.model.bind('change:phy_enabled', this.updatePhyEnabled, this);
            this.model.bind('change:phy_count', this.updatePhyCount, this);
            this.model.bind('change:dev_type', this.updateDevType, this);
            this.model.bind('change:dev_id', this.updateDevId, this);
            this.model.bind('change:connected_phy_id', this.updateConnectedPhyId, this);
            this.model.bind('change:connected_dev_id', this.updateConnectedDevId, this);


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
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
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
        updatePhyEnabled: function(model, value) {},
        updatePhyCount: function(model, value) {},
        updateDevType: function(model, value) {},
        updateDevId: function(model, value) {},
        updateConnectedPhyId: function(model, value) {},
        updateConnectedDevId: function(model, value) {},

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
                    var back_url = location.hash.split('/');
                    back_url.pop();
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
