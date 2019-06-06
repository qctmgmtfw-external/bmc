define(['jquery', 'underscore', 'backbone', 'app',
        //data

        //template
        'text!templates/task-item.html',
        //localize
        'i18n!strings/nls/common',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
        'tooltip'
    ],

    function($, _, Backbone, app, TaskItemTpl, CommonStrings) {

        var view = Backbone.View.extend({

            template: _.template(TaskItemTpl),

            initialize: function() {
                this.model.bind("change:id", this.updateID, this);
                this.model.bind("change:author", this.updateAuthor, this);
                this.model.bind("change:title", this.updateTitle, this);
                this.model.bind("change:schedule", this.updateTimeInfo, this);
                this.model.bind("remove", this.removeItem, this);
            },

            events: {
                "click .task-edit": "openItem",
                "click .task-delete": "confirmDelete"
            },

            updateID: function(model, value) {
                this.$el.find(".task-id").val(value).data("id", value);
            },

            updateAuthor: function(model, value) {
                this.$el.find(".task-author").val(value);
            },

            updateTitle: function(model, value) {
                this.$el.find(".task-title").val(value);
            },

            updateTimeInfo: function(model, value) {

                this.$el.find(".task-time-info").html(this.getTimeInfo());
            },

            getTimeInfo: function() {
                var str = CommonStrings.created_time + " " + moment(this.model.get('create_time'), 'X').fromNow() + "; ";
                str += CommonStrings.runs_on + moment.unix(this.model.get('schedule').next_run).format('MMM D \'YY H:mm:ss Z');
                return str;
            },

            beforeRender: function() {
                this.$el.addClass("clearfix");
            },

            afterRender: function() {
                this.$el.find(".task-delete").tooltip({
                    placement: 'top',
                    title: CommonStrings.click_confirm_deletion,
                    trigger: 'click'
                });
            },

            openItem: function() {
                app.router.navigate("task/" + this.model.get('id'), {
                    trigger: true
                });
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

            serialize: function() {
                return {
                    locale: CommonStrings,
                    id: this.model.get('id'),
                    author: this.model.get('author'),
                    title: this.model.get('task_title'),
                    time_info: this.getTimeInfo()
                };
            }

        });

        return view;

    });