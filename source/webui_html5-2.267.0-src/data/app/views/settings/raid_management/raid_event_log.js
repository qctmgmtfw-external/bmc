define(['jquery', 'underscore', 'backbone', 'app',

        'collection//raid_controller_info',
        //data
        'collection//raid_event_log',
        //views
        'views/settings/raid_management/raid_event_log_item',
        //localize
        'i18n!strings/nls/raid_event_log',
        //template
        'text!templates/settings/raid_management/raid_event_log.html',
        //plugin
        'footable'
    ],

    function($, _, Backbone, app, RaidControllerInfoCollection, RaidEventLogCollection, RaidEventLogItemView, locale, RaidEventLogTemplate) {

        var view = Backbone.View.extend({

            template: _.template(RaidEventLogTemplate),
            MAX_EVENT_TYPE: 9, //0 to 8, Filter of Event type
            EVENT_LOG_FILTER_DATA: [],

            initialize: function() {
                this.raid_event_log = RaidEventLogCollection;
                this.raid_controller_info = RaidControllerInfoCollection;
                this.collection = this.raid_event_log;
                //this.raid_controller_info.bind('add', this.affect, this);

                /*this.raid_event_log.bind('reset', this.reset, this);
                this.raid_event_log.bind('remove', this.removeModel, this);
                this.raid_event_log.bind('change', this.change, this);
                this.raid_event_log.bind('change add reset remove', this.affect, this);*/
                //this.raid_event_log.bind('footable_row_expanded', this.showExpandDetails, this);

            },

            events: {
                //"change #idgroup_by_controller_name": "groupChanged",
                "change #idgroup_by_controller_name": "filter",
                "change #idgroup_by_event_type": "filter",
                "click #btnClearAll": "clickBtnClearAll",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

            afterRender: function() {
                var that = this;
                that.EVENT_LOG_FILTER_DATA = new Array();
                this.raid_controller_info.fetch({
                    success: function() {
                        that.bindDropdowns.call(that);
                    }
                });

                //this.bindDropdowns();

                /*var _parent = this;
                this.raid_controller_info.each(function(model) {
                    console.log("inside raid_controller_info");
                    _parent.affect(model, this.raid_controller_info);
                }, this);*/
                this.$('.footable').footable();

                /*this.collection.reset();
                this.collection.fetch();
                var _parent = this;
                this.collection.each(function(model) {
                    //_parent.add(model, this.collection);
                    _parent.affect(model, this.collection);
                }, this);
                this.$('.footable').footable();*/
            },

            add: function(model, collection, options) {
                var itemView = new RaidEventLogItemView({
                    model: model
                });
                itemView.$el.appendTo(".table-body");
                //itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
                itemView.render();



                if ($('#btnClearAll').length > 0 && $('.table-body tr').length > 0) {
                    $('#btnClearAll').removeAttr('disabled');
                }

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

            showExpandDetails: function(model, collection, options) {
                $('.footable').footable().bind('footable_row_expanded', function(e) {
                    $('.footable tbody tr.footable-detail-show').not(e.row).each(function() {
                        $('.footable').data('footable').toggleDetail(this);
                    });
                });
            },

            affect: function(model, collection, options) {

                //this.$('#idgroup_by_event_type').change();
                //}
            },

            bindDropdowns: function() {

                var avset;
                var group_models = this.raid_controller_info.groupBy(function(model) {
                    return model.get('controller_name');
                });
                this.$('#idgroup_by_controller_name').html('');
                for (i in group_models) {
                    var val = i.substr(i.indexOf("("), i.indexOf(")")).replace("(", "").replace(")", "");

                    this.$('#idgroup_by_controller_name').append("<option value='" + val + "'>" + i + "</option>");
                    //this.$('#idgroup_by_controller_name').change();
                }

                this.$('#idgroup_by_event_type').html('');
                this.$('#idgroup_by_event_type').append("<option value='all'>" + locale.all_events + "</option>");

                for (var i = 1; i < this.MAX_EVENT_TYPE; i++) {
                    var temp = "event_type" + "_" + i;
                    //this.$('#idgroup_by_event_type').append("<option value='" + i + "'>" + i + "</option>");
                    var text = locale[temp];
                    var value = i; //locale[temp];
                    $('#idgroup_by_event_type').append(new Option(text, value));
                }
                //this.filter();
                this.$('#idgroup_by_controller_name').change();
            },

            reload: function(model, value) {

            },
            filter: function() {

                var context = this;
                var elraid_id = this.$el.find("#idgroup_by_controller_name");
                var elevent_type = this.$el.find("#idgroup_by_event_type");

                //var event_type=(elevent_type.val() == 'all') ? -1 : parseInt(elevent_type.val());

                //$('.table-body').html('');

                context.removeRows.call();

                var data = {};
                data.EVTTYPE = (elevent_type.val() == 'all') ? -1 : parseInt(elevent_type.val());
                data.CTRLID = parseInt(elraid_id.val());

                var object = JSON.stringify(data);
                context.collection.fetch({
                    type: 'GET',
                    dataType: "json",
                    data: data,
                    success: function() {
                        var _parent = context;
                        if(context.collection.length>0){
                            context.collection.each(function(model) {
                                _parent.add(model, context.collection);
                            }, context);
                        }
                        else{
                            $('.table-body').html("<tr><td class=\"footable-even\" colspan=\"7\">No Records to display...<td></tr>");
                        }
                    },
                    failure: function() {

                    }
                });
            },
            removeRows: function() {
                $(".footable>tbody>tr").each(function(index, elem) {
                    $(elem).remove();
                });
                //this.$('.footable').addClass("no-paging");
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
            clickBtnClearAll: function() {
                var context = this;
                if (!confirm("This will clear all the events in the log. Click Ok if you want to proceed?")) return;

                //if (window.confirm("You are about to clear the Raid Event Log. Do you really want to clear the Raid Event Log?")) {
                $.ajax({
                    url: "/api/settings/raid_management/raid_event_log",
                    type: "DELETE",
                    success: function(data) {
                        alert("All logs has been cleared successfully");
                        Backbone.history.loadUrl(Backbone.history.fragment);
                    },
                    error: function() {
                        alert("Error while deleting. Try Again Later");
                    }
                });
                //}
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
