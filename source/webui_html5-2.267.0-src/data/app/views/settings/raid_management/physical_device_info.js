define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//physical_device_info',

'collection//logical_device_info',
//views
'views/settings/raid_management/physical_device_info_item',
//localize
'i18n!strings/nls/physical_device_info',
//template
'text!templates/settings/raid_management/physical_device_info.html',
//plugin
'footable'],

function($, _, Backbone, app, PhysicalDeviceInfoCollection, LogicalDeviceInfoCollection,PhysicalDeviceInfoItemView, locale, PhysicalDeviceInfoTemplate) {

    var view = Backbone.View.extend({

        template: _.template(PhysicalDeviceInfoTemplate),

        initialize: function() {
            this.physical_device_info = PhysicalDeviceInfoCollection;
            this.logical_device_info = LogicalDeviceInfoCollection;
            this.collection = this.physical_device_info;
            this.physical_device_info.bind('add', this.affect, this);
            
            /*this.physical_device_info.bind('reset', this.reset, this);
            this.physical_device_info.bind('remove', this.removeModel, this);
            this.physical_device_info.bind('change', this.change, this);
            this.physical_device_info.bind('change add reset remove', this.affect, this);*/
        },

        events: {
            "change #idgroup_by_controller_name": "filter",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.collection.reset();
            this.collection.fetch();

            this.logical_device_info.reset();
            this.logical_device_info.fetch();

            var _parent = this;
            this.collection.each(function(model) {
                _parent.affect(model, this.collection);
            }, this);
            this.$('.footable').footable();
        },

        add: function(model, collection, options) {
            var itemView = new PhysicalDeviceInfoItemView({
                model: model
            });
            itemView.$el.appendTo(".table-body");
            //itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
            itemView.render();
            //this.$('.footable').trigger('footable_redraw');
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
                //var temp=model.get('controller_name') +" (" + model.get('controller_id') +")";
                return model.get('controller_name');
            });
            this.$('#idgroup_by_controller_name').html('');
            for (i in group_models) {
                this.$('#idgroup_by_controller_name').append("<option value='" + i + "'>" + i + "</option>");
            }
            this.$("#idgroup_by_controller_name").prop('selectedIndex', 0);
            this.$('#idgroup_by_controller_name').change();
        },

        reload: function(model, value) {

        },

        groupChanged: function(e) {
            var selected = $(e.currentTarget).val();
            if (selected == 'all') {
                this.$('table.footable tbody tr').show();
                return;
            }
            this.$('table.footable tbody tr').hide();
            this.$('table.footable tbody tr:contains("' + selected + '")').show();
        },
        filter: function() {
            var elraid_id = this.$el.find("#idgroup_by_controller_name");
            var filterData= this.collection.byRAIDName(elraid_id.val(),10);
            this.removeRows();
            var that = this;
            filterData.each(function(model) {
                that.add(model);
            }, this);
            this.$('.footable').trigger('footable_redraw');
        },

        removeRows:function(){
             $(".footable>tbody>tr").each(function(index, elem){
                    $(elem).remove();
            });
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