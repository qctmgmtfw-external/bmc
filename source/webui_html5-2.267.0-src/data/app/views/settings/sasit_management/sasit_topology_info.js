define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'collection//sasit_topology_info',
        //views
        'views/settings/sasit_management/sasit_topology_info_item',
        //localize
        'i18n!strings/nls/sasit_topology_info',
        //template
        'text!templates/settings/sasit_management/sasit_topology_info.html',
        //plugin
        'footable',
        'tree-view'
    ],

    function($, _, Backbone, app, SasitTopologyInfoCollection, SasitTopologyInfoItemView, locale, SasitTopologyInfoTemplate) {

        var view = Backbone.View.extend({

            template: _.template(SasitTopologyInfoTemplate),
            topology_data: [],
            topology_info: [],

            initialize: function() {
                this.sasit_topology_info = SasitTopologyInfoCollection;
                this.collection = this.sasit_topology_info;
                this.sasit_topology_info.bind('add', this.add, this);
                this.sasit_topology_info.bind('reset', this.reset, this);
                this.sasit_topology_info.bind('remove', this.removeModel, this);
                this.sasit_topology_info.bind('change', this.change, this);
                this.sasit_topology_info.bind('change add reset remove', this.affect, this);
            },

            events: {
                "change #idgroup_by_controller_name": "groupChanged",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

            afterRender: function() {
                this.collection.reset();
                this.topology_info = [];
                this.topology_data = [];
                $('#tree').html("");
                var context = this;
                this.collection.fetch({
                    success: function() {
                        context.load.call(context);
                    },
                    failure: function() {}
                });
            },
            load: function() {
                $('#tree').html("");
                var that = this;

                topology_info = this.collection;

                topology_info.each(function(model) {

                    var dev_type = model.get("DevType");
                    if (dev_type == 1) {

                        var dev_type_text = that.getTopologyDevType.call(that, dev_type) + " " + "(" + model.get("ConnectedDevID") + ")";
                        that.topology_data.push({
                            "text": dev_type_text,
                            "nodes": []
                        });

                        var topology_count = model.get("PhyCount");
                        if (topology_count > 0) {
                            var phy_enabled = model.get("PhyEnabled");
                            if (phy_enabled == 1) {
                                for (var i = 0; i < topology_count; i++) {
                                    that.buildPhysicalTree.call(that, i, 1);
                                }
                            }
                        }
                    }
                })
                var final_data = JSON.stringify(that.topology_data);
                $('#tree').treeview({ data: final_data });
            },

            buildPhysicalTree: function(i, l) {
                var that = this;
                var phy_text = "PHY_" + i;

                var isFound = false;
                //console.log("topology_info = this.collection;" + this.collection);

                //console.log("before this.topology_info total"+topology_info);

                topology_info = this.collection;

                for (var k = l; k < topology_info.length; k++) {
                    var model = topology_info.models[k];
                    if (i == model.get("ConnectedPhyID")) {
                        if (model.get("DevType") == 4) {
                            var type = that.getTopologyDevType.call(that, model.get("DevType"));
                            var type_text = type + " " + "(" + model.get("DeviceID") + ")";
                            that.topology_data[0].nodes.push({
                                "text": phy_text,
                                "nodes": [{ "text": type_text }]
                            });
                        }
                        isFound = true;
                    }
                }
                if (isFound == false) {
                    that.topology_data[0].nodes.push({
                        "text": phy_text
                    });
                }
            },

            getTopologyDevType: function(type) {
                //console.log("inside getTopologyDevType" +type);
                var actual = parseInt(type);
                var value;
                switch (actual) {
                    case 0:
                        value = locale.topology_dev_type_0;
                        break;
                    case 1:
                        value = locale.topology_dev_type_1;
                        break;
                    case 2:
                        value = locale.topology_dev_type_2;
                        break;
                    case 3:
                        value = locale.topology_dev_type_3;
                        break;
                    case 4:
                        value = locale.topology_dev_type_4;
                        break;
                    default:
                        value = locale.topology_dev_type_0;
                        break;
                }
                return value;
            },

            add: function(model, collection, options) {
                /*var itemView = new SasitTopologyInfoItemView({
                    model: model
                });
                itemView.$el.appendTo(".table-body");
                //itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
                itemView.render();



                this.$('.footable').trigger('footable_redraw');*/
            },

            reset: function(model, collection, options) {

                this.$('.footable').trigger('footable_redraw');
            },

            removeModel: function(model, collection, options) {

                this.$('.footable').trigger('footable_redraw');
            },

            change: function(model, collection, options) {

                this.$('.footable').trigger('footable_redraw');
            },

            affect: function(model, collection, options) {
                var group_models = this.collection.groupBy(function(model) {
                    return model.get('controller_name');
                });
                this.$('#idgroup_by_controller_name').html('');
                for (i in group_models) {
                    this.$('#idgroup_by_controller_name').append("<option value='" + i + "'>" + i + "</option>");
                    this.$('#idgroup_by_controller_name').change();
                }
            },

            reload: function(model, value) {

            },

            groupChanged: function(e) {
                var selected = $(e.currentTarget).val();

                /*if (selected == 'all') {
                    this.$('table.footable tbody tr').show();
                    return;
                }
                this.$('table.footable tbody tr').hide();
                this.$('table.footable tbody tr:contains("' + selected + '")').show();*/

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
            serialize: function() {
                return {
                    locale: locale
                };
            }

        });

        return view;

    });
