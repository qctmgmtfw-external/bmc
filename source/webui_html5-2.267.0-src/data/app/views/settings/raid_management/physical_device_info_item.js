define(['jquery', 'underscore', 'backbone', 'app',
        //localize
        'i18n!strings/nls/physical_device_info',

        'collection//logical_device_info',

        //template
        'text!templates/settings/raid_management/physical_device_info_item.html'
    ],

    function($, _, Backbone, app, locale, LogicalDeviceInfoCollection, PhysicalDeviceInfoTemplate) {

        var view = Backbone.View.extend({

            tagName: "tr",
            template: _.template(PhysicalDeviceInfoTemplate),
            initialize: function() {
                this.logical_device_info = LogicalDeviceInfoCollection;
            },
            events: {
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {},

            afterRender: function() {

                this.$('.edit').removeClass('hide');

                var href = this.$el.find('a.edit').attr('href');
                this.$el.find('a.edit').attr('href', href.replace(':id', this.model.get('id')));
                this.$('.edit').attr("title", "Click to view Advanced Properties");

                this.$('.delete').removeClass('hide');
                this.$('.hotspare').removeClass('hide');

                this.$('.dedicatedHotspare').removeClass('hide');
                this.$('.LEDActivation').removeClass('hide');

                var PhysicalStateId = this.model.get('state');
                var RemovalStatus = this.model.get('removal_status');

                this.$('.hotspare').attr("data-devid", this.model.get('dev_id'));
                this.$('.delete').attr("data-devid", this.model.get('dev_id'))

                if (PhysicalStateId == 0) { //Unconfig_good
                    this.$('.hotspare').attr("data-hotspare", 0);
                    this.$('.hotspare').attr("title", "Click to Add as Global HotSpare");
                } else if (PhysicalStateId == 2) { //HotSpare
                    this.$('.hotspare').attr("data-hotspare", 3);
                    this.$('.hotspare').html("<i class='ion ion-minus'></i>");
                    this.$('.hotspare').attr("title", "Click to Remove from Global HotSpare");
                }

                if (PhysicalStateId == 0 || PhysicalStateId == 2) {
                    //do nothing
                } else {
                    this.$('.hotspare').addClass("disabled");
                    this.$('.dedicatedHotspare').addClass("disabled");
                }

                this.$('.hotspare').on('click', { model: this.model }, this.callHotSpare);
                this.$('.dedicatedHotspare').on('click', { model: this.model, devId: this.model.get('dev_id'), logical: this.logical_device_info, currentRow: this.$el }, this.DisplayDedicatedHotSpare);
                this.$('.makededicatedHotspare').on('click', { model: this.model, devId: this.model.get('dev_id') }, this.callDedicatedHotSpare);
                this.$('.LEDActivation').on('click', { model: this.model, devId: this.model.get('dev_id') }, this.callLEDActivation);
                this.$('.delete').on('click', { model: this.model, devId: this.model.get('dev_id') }, this.deleteItem);

                this.$('.dedicatedHotspare').attr("title", "Click to view available Logical Devices to make Dedicated HotSpare");
                this.$('.makededicatedHotspare').attr("title", "Click to Add as Dedicated HotSpare");
                this.$('.LEDActivation').attr("title", "Click to Locate Physical Device");


                if (PhysicalStateId == 0) {
                    //do nothing
                } else {
                    this.$('.delete').addClass("disabled");
                    this.$('.dedicatedHotspare').addClass("disabled");
                }

                if (RemovalStatus == 1) {
                    //Undo Prepare For Removal CMDPARAM value;	
                    this.$('.delete').attr("data-removalstatus", 1);
                    this.$('.delete').attr("title", "Click to Undo Prepare For Removal");
                    this.$('.delete').html("<i class='ion ion-reply'></i>");
                } else if (RemovalStatus == 2) {
                    //Prepare For Removal CMDPARAM value;
                    this.$('.delete').attr("data-removalstatus", 0);
                    this.$('.delete').attr("title", "Click to Prepare For Removal");
                    this.$('.delete').html("<i class='ion ion-minus'></i>");
                }
            },
            callHotSpare: function(ev) {

                var currentRowModel = ev.data.model;
                var that = this;
                var currentRow = $(this).parent();

                var ctlrid = currentRowModel.get('controller_id');
                var ctrlName = $('#idgroup_by_controller_name').val();

                var devId = currentRowModel.get("dev_id"); //$('.hotspare').data("devid");
                var cmdpram = currentRow.find('#idhotspare').data("hotspare");

                ev.stopPropagation();
                ev.preventDefault();
                if (!confirm('Are you sure to perform this operation?')) return;

                currentRow.find('#makededicatedHotspare').addClass('hide');
                currentRow.find('#idselect_logical').addClass('hide');


                var data = {};
                data.CMDPARAM = cmdpram;
                data.CTRLID = ctlrid;
                data.DEVICEID = devId;

                var object = JSON.stringify(data);

                //console.log(object);
                //localStorage.setItem("ctrl_name", ctrlName);

                //console.log("cmdpram" +cmdpram);
                //console.log("CTRLID" +ctlrid);
                //console.log("DEVICEID" +devId);

                $.ajax({
                    url: "/api/settings/raid_management/physical_hotspare_cmd",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    processData: false,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        alert(locale.hot_spare_success);
                        Backbone.history.loadUrl(Backbone.history.fragment);
                    },
                    error: function(xhr, status, err) {
                        app.HTTPErrorHandler(xhr, status, err);
                    }
                });
            },

            refreshData: function() {
                //refresh current view
                Backbone.history.loadUrl(Backbone.history.fragment);
                if (localStorage.getItem("ctrl_name") != null) {
                    var selected = localStorage.getItem("ctrl_name");
                    if (selected == 'all') {
                        this.$('table.footable tbody tr').show();
                        return;
                    }
                    this.$('table.footable tbody tr').hide();
                    this.$('table.footable tbody tr:contains("' + selected + '")').show();
                }
            },

            removeItem: function() {
                this.$el.remove();
            },

            deleteItem: function(ev) {

                var that = this;
                var currentRowModel = ev.data.model;

                var currentRow = $(this).parent();

                var ctlrid = currentRowModel.get('controller_id');
                var ctrlName = $('#idgroup_by_controller_name').val();

                var devId = currentRowModel.get('dev_id'); //$('.delete').data("devid");
                var cmdpram = currentRow.find('#iddelete').data("removalstatus"); //$('.delete').data("removalstatus");

                ev.stopPropagation();
                ev.preventDefault();
                if (!confirm('Are you sure to perform this operation?')) return;

                currentRow.find('#idselect_logical').addClass('hide');
                currentRow.find('#idmakededicatedHotspare').addClass('hide');

                var data = {};
                data.CMDPARAM = cmdpram;
                data.CTRLID = ctlrid;
                data.DEVICEID = devId;

                var object = JSON.stringify(data);

                $.ajax({
                    url: "/api/settings/raid_management/physical_removal_cmd",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    processData: false,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        alert(locale.prepre_removal_success);
                        Backbone.history.loadUrl(Backbone.history.fragment);

                    },
                    error: function(xhr, status, err) {
                        app.HTTPErrorHandler(xhr, status, err);
                    }
                });
            },
            DisplayDedicatedHotSpare: function(ev) {

                var currentRowModel = ev.data.model;

                var loggical_collection = ev.data.logical;

                var count=0;

                if (loggical_collection.length == 0) {
                    alert("Atleast one Logical device should exist to add a Dedicated HotSpare.");
                    return;
                }

                var ctlrid = currentRowModel.get('controller_id');

                loggical_collection.each(function(model) {
                    if (ctlrid == model.get("controller_id")) {
                        count++;
                    }
                }, this);

                if (count == 0) {
                    alert("Atleast one Logical device should exist to add a Dedicated HotSpare.");
                    return;
                }

                var currentRow = $(this).parent();

                currentRow.find('#idselect_logical').html('');

                currentRow.find('#idselect_logical').removeClass('hide');

                currentRow.find('.makededicatedHotspare').removeClass('hide');




                loggical_collection.each(function(model) {
                    if (ctlrid == model.get("controller_id")) {
                        var name = model.get("ld_name") + "-" + model.get("dev_id");
                        var value = model.get("dev_id");
                        currentRow.find('#idselect_logical').append("<option value='" + value + "'>" + name + "</option>");
                    }
                }, this);
            },
            callDedicatedHotSpare: function(ev) {

                ev.stopPropagation();
                ev.preventDefault();
                if (!confirm('Are you sure to perform this operation?')) return;

                var that = this;
                var currentRow = $(this).parent();

                var currentRowModel = ev.data.model;

                var ctlrid = currentRowModel.get('controller_id');
                var ctrlName = $('#idgroup_by_controller_name').val();


                var devId = ev.data.devId; //$('.delete').data("devid");
                var diskCount = 1; //always 

                var data = {};
                data.CTRLID = ctlrid;
                data.DEVICEID = devId;
                data.DISKCOUNT = diskCount;
                data.DISKID = currentRow.find('#idselect_logical').val();

                var object = JSON.stringify(data);

                //localStorage.setItem("ctrl_name", ctrlName);

                $.ajax({
                    url: "/api/settings/raid_management/physical_dedicated_hotspare_cmd",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    processData: false,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        alert(locale.dedicated_hot_spare_success);
                        Backbone.history.loadUrl(Backbone.history.fragment);
                    },
                    error: function(xhr, status, err) {
                        app.HTTPErrorHandler(xhr, status, err);
                    }
                });
            },
            callLEDActivation: function(ev) {

                var currentRow = $(this).parent();

                ev.stopPropagation();
                ev.preventDefault();
                //var c= ev.data.model;
                if (!confirm('Are you sure to perform this operation?')) return;

                currentRow.find('#idmakededicatedHotspare').addClass('hide');
                currentRow.find('#idselect_logical').addClass('hide');

                var that = this;
                var currentRowModel = ev.data.model;

                var ctlrid = currentRowModel.get('controller_id');
                var ctrlName = $('#idgroup_by_controller_name').val();

                var devId = ev.data.devId; //$('.delete').data("devid");
                var duration = 5; // for now duration made it as constant.
                var action = 1; // for now start locate

                var data = {};
                data.ACTION = action;
                data.CTRLID = ctlrid;
                data.DEVICEID = devId;
                data.DURATION = duration;

                var object = JSON.stringify(data);

                //localStorage.setItem("ctrl_name", ctrlName);

                $.ajax({
                    url: "/api/settings/raid_management/physical_start_stop_locate_cmd",
                    type: "PUT",
                    dataType: "json",
                    data: object,
                    processData: false,
                    contentType: "application/json",
                    success: function(data, status, xhr) {
                        alert(locale.located_physical_success);
                        Backbone.history.loadUrl(Backbone.history.fragment);
                    },
                    error: function(xhr, status, err) {
                        app.HTTPErrorHandler(xhr, status, err);
                    }
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
            displaySuccessMsg: function() {
                this.$(".alert-success").removeClass("hide");
                setTimeout(function() {
                    $(".alert-success").addClass("hide");
                }, 5000);

            },
            displayErrorMsg: function() {
                this.$(".alert-danger").removeClass("hide");
                setTimeout(function() {
                    $(".alert-danger").addClass("hide");
                }, 5000);
            },

            serialize: function() {
                return {
                    locale: locale,
                    model: this.model
                };
            }

        });

        return view;

    });
