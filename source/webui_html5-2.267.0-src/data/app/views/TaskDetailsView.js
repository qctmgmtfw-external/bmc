define(['jquery', 'underscore', 'backbone', 'app',
        //data
        "collection/tasks",
        "collection/scripts",
        "models/task",

        "views/ScriptLibraryItemView",
        "views/ScriptEditItemView",
        "views/ScriptEditView",
        //template
        'text!templates/task-details.html',
        //localize
        'i18n!strings/nls/task_details',
        //plugins
        'moment',
        'iCheck',
        'datepicker',
        // 'knob',
        // 'sparkline'
        'tooltip'
    ],

    function($, _, Backbone, app, TaskCollection, ScriptCollection, TaskModel,
        ScriptLibraryItemView, ScriptEditItemView, ScriptEditView, TaskDetailsTpl, CommonStrings, moment) {

        var view = Backbone.View.extend({

            template: _.template(TaskDetailsTpl),

            initialize: function() {
                this.collection = TaskCollection;
            },

            events: {
                "change #repeat": "changeRepeat",
                "change #filter-scripts": "searchScriptLibrary",
                "click #btn-filter-scripts": "searchScriptLibrary",
                "click .btn-delete-task": "deleteTask",
                "click .btn-save-task": "saveTask",
                "ifChanged input[name='ends']": "switchEndSelection"
            },


            beforeRender: function() {
                //this.$el.addClass("row");
            },

            afterRender: function() {
                ScriptEditView.render();

                $('#task-start, #task-end').datetimepicker();
                this.$el.find('input[type="radio"],input[type="checkbox"]').iCheck({
                    checkboxClass: 'icheckbox_square-blue',
                    radioClass: 'iradio_square-blue',
                    increaseArea: '10%'
                });

                this.task_id = location.hash.split('/').pop();

                var context = this;

                ScriptCollection.fetch({
                    success: function() {
                        ScriptCollection.each(function(model) {
                            context.addScript(model);
                        });
                    }
                });


                if (isNaN(parseInt(this.task_id, 10)) && this.task_id == "new") {
                    this.action = "new";
                    this.$el.find("btn-delete-task").html("Discard Task");
                    this.model = new TaskModel();
                } else if (parseInt(this.task_id, 10) == this.task_id) {
                    this.action = "edit";
                    //this.model = this.collection.get(this.task_id);
                } else {
                    app.router.navigate("tasks", {
                        trigger: true
                    });
                    return;
                }

                this.collection.fetch({
                    success: function() {
                        if (context.action == "edit") {
                            context.model = context.collection.get(context.task_id);
                            context.loadTask.call(context);
                        }
                    }
                });

                $("#repeat").change();
                //this.model.bind();
            },

            loadTask: function() {
                var seiv = null,
                    context = this;
                _.each(this.model.get("scripts"), function(script) {
                    seiv = new ScriptEditItemView({
                        script: script
                    });
                    context.$el.find("#execution-list small").hide();
                    context.insertView("#execution-list", seiv).render();
                    seiv.on("edit-script", context.editScript, context);
                    seiv.on("removed", context.checkExecutionList, context);
                });

                var efs = this.model.get("event_filters");
                _.each(efs, function(ef) {
                    context.$("#"+ef.type).iCheck("check");
                    var dfid = context.$("#"+ef.type).data("filter-id");
                    if(dfid) {
                        if(ef.type == "Web") {
                            context.$('#'+dfid).val(ef.filter.event);
                        }
                        else {
                            context.$('#'+dfid).val(ef.filter);
                        }
                    }
                });

                var schedule = this.model.get("schedule"),
                    et = "never";

                // if(typeof schedule != "object") {
                //     schedule = JSON.parse(schedule);
                // }

                switch(schedule.ends_type){
                    case "date":
                        et = "ondate";
                        this.$("#task-end").data("DateTimePicker").enable();
                        console.log(moment(schedule.ends, 'X').format("MM/DD/YYYY hh:mm A"));
                        this.$("#task-end").data("DateTimePicker").setDate(moment(schedule.ends, 'X').format("MM/DD/YYYY hh:mm A"));
                    break;
                    case "occurrence":
                        et = "occurrence";
                        this.$("#occurrence").val(schedule.ends);
                    break;
                    default:
                        et = "never";
                    break;
                }
                

                this.$("#task-title").val(this.model.get("task_title"));
                this.$("input[name='task-listen'][value='1']").iCheck("check");
                this.$("#task-start").data("DateTimePicker").setDate(moment(schedule.start_time, 'X').format("MM/DD/YYYY hh:mm A"));
                this.$("#repeat").val(schedule.repeats).change();
                this.$("#repeat-interval").val(schedule.repeat_interval);
                _.each(schedule.repeat_on, function(day){
                    context.$("input[name='repeat-on'][value='"+day+"']").iCheck("check");
                });
                this.$("input[name='repeat_by'][value='"+schedule.repeat_by+"']").iCheck("check");
                this.$("input[name='ends'][value='"+et+"']").iCheck("check");

            },

            switchEndSelection: function(ev) {
                var sel = this.$(ev.currentTarget).filter(":checked").val();

                switch(sel) {
                    case "never":
                        this.$("#occurrence").attr("disabled","disabled");
                        this.$("#task-end").data("DateTimePicker").disable();
                    break;
                    case "occurrence":
                        this.$("#occurrence").removeAttr("disabled");
                        this.$("#task-end").data("DateTimePicker").disable();
                    break;
                    case "ondate":
                        this.$("#occurrence").attr("disabled","disabled");
                        this.$("#task-end").data("DateTimePicker").enable();
                    break;
                }
            },

            deleteTask: function() {
                if (this.action == "edit") {
                    this.model.destroy();
                }
                app.router.navigate("tasks", {
                    trigger: true
                });
            },

            saveTask: function() {

                var scripts = [],
                    repeat_on = [],
                    ends = 0,
                    ends_type = null,
                    event_filters = [],
                    start_time = null;

                if (this.$("input[name='task-listen'][value='0']").prop('checked')) {
                    start_time = moment().format("X");
                }
                else {
                    start_time = this.$("#task-start").data("DateTimePicker").getDate().format("X");
                }

                if (this.$("#Sensor").is(":checked")) {
                    event_filters.push({
                        type: "Sensor",
                        filter: parseInt(this.$("#sensor-filters").val())
                    });
                }

                if (this.$("#Remote").is(":checked")) {
                    event_filters.push({
                        type: "Remote",
                        filter: 0
                    });
                }

                if (this.$("#Web").is(":checked")) {
                    event_filters.push({
                        type: "Web",
                        filter: {
                            event: parseInt(this.$("#web-filters").val())
                        }
                    });
                }

                if (this.$("#BIOS").is(":checked")) {
                    event_filters.push({
                        type: "BIOS",
                        filter: 0
                    });
                }

                if (this.$("#SMS").is(":checked")) {
                    event_filters.push({
                        type: "SMS",
                        filter: 0
                    });
                }

                if (this.$("#SMI").is(":checked")) {
                    event_filters.push({
                        type: "SMI",
                        filter: 0
                    });
                }

                switch (this.$("input[name='ends']:checked").val()) {
                    case "never":
                        ends = 0;
                        break;
                    case "occurrence":
                        ends = this.$("#occurrence").val();
                        ends_type = "occurrence";
                        break;
                    case "ondate":
                        ends = this.$("#task-end").data("DateTimePicker").getDate().format("X");
                        ends_type = "date";
                        break;
                }

                this.$("input[name='repeat-on']:checked").each(function() {
                    repeat_on.push(parseInt($(this).val()));
                });



                this.getViews("#execution-list").each(function(view) {
                    scripts.push(view.model.toJSON());
                });

                //console.log(scripts, repeat_on);

                if(this.model.isNew()) {
                    this.collection.create({
                        task_title: $("#task-title").val(),
                        scripts: scripts,
                        event_filters: event_filters,
                        schedule: {
                            repeats: parseInt(this.$("#repeat").val()),
                            repeat_interval: parseInt(this.$("#repeat-interval").val()),
                            repeat_on: repeat_on,
                            repeat_by: this.$("input[name='repeat_by']:checked").val(),
                            ends: parseInt(ends),
                            ends_type: ends_type,
                            start_time: parseInt(start_time)
                        }

                    }, {
                        success: function() {
                            app.router.navigate("tasks", {
                                trigger: true
                            });
                        }
                    });
                    
                } else {
                    this.model.save({
                        task_title: $("#task-title").val(),
                        scripts: scripts,
                        event_filters: event_filters,
                        schedule: {
                            repeats: parseInt(this.$("#repeat").val()),
                            repeat_interval: parseInt(this.$("#repeat-interval").val()),
                            repeat_on: repeat_on,
                            repeat_by: this.$("input[name='repeat_by']:checked").val(),
                            ends: parseInt(ends),
                            ends_type: ends_type,
                            start_time: parseInt(start_time)
                        }

                    }, {
                        success: function() {
                            app.router.navigate("tasks", {
                                trigger: true
                            });
                        }
                    });
                }

            },

            addScript: function(model) {
                var sliv = new ScriptLibraryItemView({
                    model: model
                });
                this.insertView("#script-library", sliv).render();
                sliv.on("import", this.importScript, this);
            },

            importScript: function(evdata) {
                var script = ScriptCollection.get(evdata.script_id);
                var seiv = new ScriptEditItemView({
                    script: script.toJSON()
                });
                this.$el.find("#execution-list small").hide();
                this.insertView("#execution-list", seiv).render();
                seiv.on("edit-script", this.editScript, this);
                seiv.on("removed", this.checkExecutionList, this);
            },

            editScript: function(evdata) {
                var sev = ScriptEditView;
                sev.setData(evdata);
                sev.show();
                console.log(sev.$el);
            },

            searchScriptLibrary: function(ev) {
                var search = this.$("#filter-scripts").val();
                if(search) {
                    this.$("#script-library li").hide();
                    this.$("#script-library li").filter(function(){
                        return ($(this).find(".script-title").text().indexOf(search) != -1);
                    }).show();
                } else {
                    this.$("#script-library li").show();
                }

            },

            checkExecutionList: function(evdata) {
                if (!this.$el.find("#execution-list div").length) {
                    this.$el.find("#execution-list small").show();
                }
            },

            confirmDelete: function(ev) {
                var btn = $(ev.currentTarget);

                if (btn.hasClass("btn-danger")) {
                    btn.tooltip('destroy');
                    this.model.destroy();
                }
                btn.removeClass("btn-default").addClass("btn-danger").tooltip('show');
                setTimeout(function() {
                    btn.removeClass("btn-danger").addClass("btn-default");
                    btn.tooltip('hide');
                }, 3500);
            },

            removeItem: function() {
                this.$el.remove();
            },

            changeRepeat: function() {
                switch (this.$el.find("#repeat").val()) {
                    case "0":
                        this.$("#repeats-on").hide();
                        this.$("#repeat-by").hide();
                        this.$("#repeat-interval").attr("placeholder", "minute");
                        this.$("#interval-help").html("minutes");
                        break;
                    case "1":
                        this.$("#repeats-on").hide();
                        this.$("#repeat-by").hide();
                        this.$("#repeat-interval").attr("placeholder", "hours");
                        this.$("#interval-help").html("hours");
                        break;
                    case "2":
                        this.$("#repeats-on").hide();
                        this.$("#repeat-by").hide();
                        this.$("#repeat-interval").attr("placeholder", "days");
                        this.$("#interval-help").html("days");
                        break;
                    case "3":
                        this.$("#repeats-on").show();
                        this.$("#repeat-by").hide();
                        this.$("#repeat-interval").attr("placeholder", "weeks once");
                        this.$("#interval-help").html("weeks");
                        break;
                    case "4":
                        this.$("#repeats-on").hide();
                        this.$("#repeat-by").show();
                        this.$("#repeat-interval").attr("placeholder", "months once");
                        this.$("#interval-help").html("months");
                        break;
                    case "5":
                        this.$("#repeats-on").hide();
                        this.$("#repeat-by").hide();
                        this.$("#repeat-interval").attr("placeholder", "years");
                        this.$("#interval-help").html("years");
                        break;
                }
            },

            serialize: function() {
                return {
                    locale: CommonStrings,
                    id: 0
                    // ,
                    // id: this.model.get('id'),
                    // author: this.model.get('author'),
                    // title: this.model.get('title'),
                    // time_info: this.getTimeInfo()
                };
            }

        });

        return view;

    });