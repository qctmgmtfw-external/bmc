define(['jquery', 'underscore', 'backbone', 'app',
        'collection/tasks',
        'text!templates/dashboard/calendar.html',
        'i18n!strings/nls/calendar'
    ],
    function($, _, Backbone, app, TaskCollection, CalendarView, CommonStrings) {

        var view = Backbone.View.extend({

            initialize: function(){
                this.collection = TaskCollection;
                app.configurations.bind('features_loaded', this.afterFeatures, this);
                //this.collection.bind("add change remove reset", this.load, this);
            },

            template: _.template(CalendarView),

            beforeRender: function() {},

            afterRender: function() {},

            afterFeatures: function() {
                var that = this;
                if(app.features.indexOf('AUTOMATION') == -1) return;
                this.collection.fetch({
                    success: function(){
                        that.load();
                    }
                });
            },

            load: function() {
                //Date for the calendar events (dummy data)
                /*
                var date = new Date();
                var d = date.getDate(),
                    m = date.getMonth(),
                    y = date.getFullYear();
                */
                var ae_events = TaskCollection.map(function(model){
                    var ev = {};

                    ev.title = model.get('task_title');
                    ev.start = new Date(model.get('schedule').start_time*1000);
                    if(model.get('schedule')['ends']) {
                        //TODO: make this repeating based on repeats
                        //ev.end = new Date(mode.get('schedule').ends*1000);
                    }

                    return ev;
                });
                //console.log(ae_events);
                //Calendar
                $('#calendar').fullCalendar({
                    editable: true, //Enable drag and drop
                    backgroundColor: "#0073b7", //Blue
                    borderColor: "#0073b7", //Blue
                    /*
                    events: [{
                        title: CommonStrings.monitor_cpu_load, 
                        start: new Date(y, m, 17)
                        // ,
                        // backgroundColor: "#3c8dbc", //light-blue 
                        // borderColor: "#3c8dbc" //light-blue
                    }, {
                        title: CommonStrings.external_security_aduit, 
                        start: new Date(y, m, d - 5),
                        end: new Date(y, m, d - 2)
                        // ,
                        // backgroundColor: "#f39c12", //yellow
                        // borderColor: "#f39c12" //yellow
                    }, {
                        title: CommonStrings.update_bmc, 
                        start: new Date(y, m, d, 10, 30),
                        allDay: false
                        // ,
                        // backgroundColor: "#0073b7", //Blue
                        // borderColor: "#0073b7" //Blue
                    }, {
                        title: CommonStrings.os_management, 
                        start: new Date(y, m, d + 1, 19, 0),
                        end: new Date(y, m, d + 1, 22, 30),
                        allDay: false
                        // ,
                        // backgroundColor: "#00a65a", //Success (green)
                        // borderColor: "#00a65a" //Success (green)
                    }, {
                        title: CommonStrings.goto_full_event, 
                        start: new Date(y, m, 28),
                        end: new Date(y, m, 29),
                        url: '#calendar/event/' + 10
                        // ,
                        // backgroundColor: "#f56954", //red
                        // borderColor: "#f56954" //red
                    }],
                    */
                    events: ae_events,
                    buttonText: { //This is to add icons to the visible buttons
                        prev: "<span class='fa fa-caret-left'></span>",
                        next: "<span class='fa fa-caret-right'></span>",
                        today: 'today',
                        month: 'month',
                        week: 'week',
                        day: 'day'
                    },
                    header: {
                        left: 'title',
                        center: '',
                        right: 'prev,next'
                    }
                });
            },

             serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });
