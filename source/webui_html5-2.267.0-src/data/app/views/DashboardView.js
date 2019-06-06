define(
    ["jquery", "underscore", "backbone", "app",
        "views/dashboard/StatsView",
        "views/dashboard/LogDetailsView",
        "views/dashboard/QuickSensorView",
        "views/dashboard/CalendarView",
        //template
        'text!templates/dashboard.html',
        //localize
        'i18n!strings/nls/dashboard',
        //plugins
        //"knob",
        "raphael", "morris", "todo", "fullcalendar"
        // , "bower/bootstrap/js/bootstrap-tab"
        // , "bower/bootstrap/js/bootstrap-dropdown"
        // , "jvectormap"
    ],
    function($, _, Backbone, app, StatsView, LogDetailsView, QuickSensorView, CalendarView, DashTemplate, CommonStrings) {
        var view = Backbone.View.extend({

            //keep: true,

            template: _.template(DashTemplate),

            initialize: function() {
                this.on("close", this.close, this);
            },

            beforeRender: function() {
                //this.$el.addClass("widgets");
                var stat = new StatsView(),
                    log = new LogDetailsView(),
                    sensor = new QuickSensorView(),
                    cal = new CalendarView();
                this.views = [stat, log, sensor, cal];
                this.insertView(".top", stat);
                this.insertView(".left", log);
                this.insertView(".right", sensor);
                this.insertView(".left", cal);
            },

            renderCalendar: function() {
            },

            afterRender: function() {

            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            },

            close: function() {
                var context = this;
                _.each(this.views, function(View){
                    View.trigger('close');
                    context.removeView(View);
                });
            }

        });

        return view;
    });
