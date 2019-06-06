define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/sol_recorded_video',
//template
'text!templates/settings/video/sol/sol_recorded_video_item.html'],

function($, _, Backbone, app, locale, SolRecordedVideoTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(SolRecordedVideoTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
        },

        events: {
            "click .delete": "deleteItem",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {},

        afterRender: function() {
            this.$('.delete').removeClass('hide');
            this.$('.action-1').removeClass('hide');
            var href = this.$el.find('a.action-1').attr('href');
            this.$el.find('a.action-1').attr('href', href.replace(':file_name', this.model.get('file_name')));
            if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();  
                app.disableAllinput();  
            }
        },

        removeItem: function() {
            this.$el.remove();
        },

        deleteItem: function(ev) {
            var that = this;
            ev.stopPropagation();
            ev.preventDefault();
            if (!confirm('Are you sure to perform this operation?')) return;
            var data = {};
            var file_name = that.model.get('file_name');
            var path = that.model.collection.url().split('?')[0] + '/' + that.model.get('file_name');
            data.file_name = file_name;
            var object = JSON.stringify(data);
            $.ajax({
                url: path,
                type: 'DELETE',
                dataType: 'json',
                data: object,
                processData: false,
                contentType: 'application/json',
                success: function(data, status, xhr) {
                    alert('Deleted Successfully');
                    Backbone.history.loadUrl(Backbone.history.fragment);
                },
                error: function(xhr, status, err) {
                    alert('Error while deleting. Try again later');
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

        serialize: function() {
            return {
                locale: locale,
                model: this.model
            };
        }

    });

    return view;

});
