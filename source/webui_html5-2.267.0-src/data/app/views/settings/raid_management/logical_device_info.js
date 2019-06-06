define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'collection//logical_device_info',
        //views
        'views/settings/raid_management/logical_device_info_item',
        //localize
        'i18n!strings/nls/logical_device_info',
        //template
        'text!templates/settings/raid_management/logical_device_info.html',
        //plugin
        'footable'
    ],

    function($, _, Backbone, app, LogicalDeviceInfoCollection, LogicalDeviceInfoItemView, locale, LogicalDeviceInfoTemplate) {

        var view = Backbone.View.extend({

            template: _.template(LogicalDeviceInfoTemplate),

            initialize: function() {
                this.logical_device_info = LogicalDeviceInfoCollection;
                this.collection = this.logical_device_info;
                this.logical_device_info.bind('add', this.affect, this);

                /*this.logical_device_info.bind('reset', this.reset, this);
                this.logical_device_info.bind('remove', this.removeModel, this);
                this.logical_device_info.bind('change', this.change, this);
                this.logical_device_info.bind('change add reset remove', this.affect, this);*/
            },

            events: {
                //"change #idgroup_by_controller_name": "groupChanged",
                "change #idgroup_by_controller_name": "filter",
                "click #idcreat_virtual": "createVirtualDevice",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

            afterRender: function() {

                var that = this;
                that.collection.reset();
                that.collection.fetch({
                    success: function() {
                        var _parent = that;
                        //if (that.collection.length > 0) {
                        that.collection.each(function(model) {
                            _parent.affect(model, that.collection);
                        }, that);
                        if(that.collection.length > 0){
                            that.collection.each(function(model) {
                                var index = that.collection.indexOf(model);
                                that.doConsistencyCheck(model.get("controller_id"),model.get("dev_id"),index,model.get("type"));
                            },that);
                        }
                    },
                    error: function(collection, response, options) {
                        var code;
                        code = response.responseJSON.code
                        if (code == 1365) { //this error_code mapping value.
                            $('#idcreat_virtual').addClass("disabled");
                        }
                    }
                });
                this.$('.footable').footable();
            },

            doConsistencyCheck: function(cid,did,i,type){
                var that=this;
                var data = {};
                data.CTRLID=cid;//that.model.get('controller_id');
                data.DEVICEID=did;//that.model.get('dev_id');
                var object=JSON.stringify(data);
                $.ajax({
                    url: "/api/settings/raid_management/logical_device_progress_cmd",
                    type: "PUT",
                    dataType: "json",
                    data:object,
                    processData: false,
                    async: false,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        var con_status=data.CONSISTENCY_PROGRESS;
                        if(con_status==0xFF){
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").html("<i class='fa fa-circle'></i>");
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").attr("data-consistency",1); //start
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").attr("title","Click to Start Consistency Check");
                        } else {
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").html("<i class='fa fa-circle text-success'></i>");
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").attr("data-consistency",0); //cancel
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").attr("title","Click to Cancel Consistency Check");
                        }
                        if(type==0){ //RAID0 will not support consistency check
                            $($('.footable tbody tr')[i]).find(".consistencyCheck").addClass("disabled");
                        }
                    },
                    error : function(xhr, status, err) {
                        app.HTTPErrorHandler(xhr, status, err);
                    }
                });
            },
            add: function(model, collection, options) {
                var itemView = new LogicalDeviceInfoItemView({
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
                    return model.get('controller_name');
                });
                this.$('#idgroup_by_controller_name').html('');
                for (i in group_models) {
                    this.$('#idgroup_by_controller_name').append("<option value='" + i + "'>" + i + "</option>");
                    //this.$('#idgroup_by_controller_name').change();
                }

                this.$("#idgroup_by_controller_name").prop('selectedIndex', 0);
                this.$('#idgroup_by_controller_name').change();
            },
            filter: function() {
                var elraid_id = this.$el.find("#idgroup_by_controller_name");
                var filterData = this.collection.byRAIDName(elraid_id.val(), 10);
                this.removeRows();
                var that = this;
                filterData.each(function(model) {
                    that.add(model);
                }, this);
                this.$('.footable').trigger('footable_redraw');
            },
            reload: function(model, value) {

            },
            removeRows: function() {
                $(".footable>tbody>tr").each(function(index, elem) {
                    $(elem).remove();
                });
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

            createVirtualDevice: function(ev) {
                var context = this;
                var path = "settings/raid_management/create_logical_device";
                app.router.navigate(path, {
                    trigger: true
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
