define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'collection/system-log',

        //subview
        'views/SystemLogItemView',
        "views/TimelineDateView",

        //template
        "text!templates/systemlog.html",

        //localize
        'i18n!strings/nls/systemlog',
        //plugins
        'moment',
        // 'knob',
        // 'sparkline'
        "datepicker"
    ],

    function($, _, Backbone, app, SystemLogCollection, SystemLogItemView, TimelineDateView, SystemLogTemplate, CommonStrings, moment) {

        var view = Backbone.View.extend({

            template: _.template(SystemLogTemplate),

            initialize: function() {

                this.collection = SystemLogCollection;

                //this.collection.bind("add", this.load, this);

            },

            events: {
                "click #iddate_range_begin": "showDatePickerDateRangeBegin",
                "click #iddate_range_end": "showDatePickerDateRangeEnd",
                "change #idevent_type": "changeCategory",
                "dp.change #iddate_range_begin": "load",
                "dp.change #iddate_range_end": "load",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

            afterRender: function() {
            	if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                    app.disableAllbutton();   
                 }
		 app.logslimscroll();
                $('#iddate_range_begin').datetimepicker({
                    //                pickTime: false
                });
                $("#iddate_range_end").datetimepicker({
                    //                pickTime: false
                });
                this.changeCategory();
            },

            showDatePickerDateRangeBegin: function() {
                $('#iddate_range_begin').data('DateTimePicker').show();
            },

            showDatePickerDateRangeEnd: function() {
                $('#iddate_range_end').data('DateTimePicker').show();
            },

            changeCategory: function() {
                var context = this;
                this.collection.fetch({
                    data: $.param({
                        level: this.$el.find("#idevent_type").val()
                    }),
                    success: function() {
                        context.load.call(context);
                    }
                });
            },

            load: function(model, collection, xhr) {

                var systemLogs = this.collection;

                if ($("#iddate_range_begin input").val()) {
                    var start = $("#iddate_range_begin").data("DateTimePicker").getDate(),
                        end;

                    start = parseInt(moment(start).format('X'), 10);

                    if ($("#iddate_range_end input").val()) {
                        end = $("#iddate_range_end").data("DateTimePicker").getDate();
                        end = parseInt(moment(end).format('X'), 10);
                    }

                    systemLogs = this.collection.byDateRange(start, end);
                    console.log("loading events...");
                }

                this.$el.find('#event-content').html('<li><i class="fa fa-clock-o animated fadeInDown"></i></li>');

                var groupedLog = systemLogs.groupBy(function(model) {
                    //return moment(model.get('timestamp'), 'X').format('MMM Do YYYY');
                    return moment(model.get('timestamp'), 'X').format('MMMM YYYY');
                });
                _.forEachRight(groupedLog, this.addEvents, this);


            },

            addEvents: function(modelList, title, collection) {
                //console.log("add", arguments);
                //now render the date
                var tdv = new TimelineDateView({
                    date: title
                }).render().$el;

                //this.chartData.labels.push(title);
                //this.chartData.data.push(modelList.length);

                //this.chart.addData([modelList.length], title);

                this.$el.find('#event-content').prepend(tdv);

                _.eachRight(modelList, function(model) {

                    var siv = new SystemLogItemView({
                        model: model
                    }).render().$el;

                    siv.insertAfter(tdv);
                    $(".wrapper").resize();

                }, this);


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
                    locale: CommonStrings
                };
            }

        });

        return view;

    });