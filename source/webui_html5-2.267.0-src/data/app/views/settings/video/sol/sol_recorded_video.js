define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//sol_recorded_video',
//views
'views/settings/video/sol/sol_recorded_video_item',
//localize
'i18n!strings/nls/sol_recorded_video',
//template
'text!templates/settings/video/sol/sol_recorded_video.html',
//plugin
'footable'],

function($, _, Backbone, app, SolRecordedVideoCollection, SolRecordedVideoItemView, locale, SolRecordedVideoTemplate) {

    var view = Backbone.View.extend({

        template: _.template(SolRecordedVideoTemplate),

        initialize: function() {
            this.sol_recorded_video = SolRecordedVideoCollection;
            this.collection = this.sol_recorded_video;
            this.sol_recorded_video.bind('add', this.add, this);
            this.sol_recorded_video.bind('reset', this.reset, this);
            this.sol_recorded_video.bind('remove', this.removeModel, this);
            this.sol_recorded_video.bind('change', this.change, this);
            this.sol_recorded_video.bind('change add reset remove', this.affect, this);
        },

        events: {
			 "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.collection.reset();
            this.collection.fetch();
            var _parent = this;
            this.collection.each(function(model) {
                _parent.add(model, this.collection);
                _parent.affect(model, this.collection);
            }, this);
            this.$('.footable').footable();
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
        add: function(model, collection, options) {
            var itemView = new SolRecordedVideoItemView({
                model: model
            });
            itemView.$el.appendTo(".table-body");
            //itemView.$el.insertBefore(this.$el.find(".list .list-add-item"));
            itemView.render();



            if ($('.btnClearAll').length > 0 && $('.table-body tr').length > 0) {
                $('.btnClearAll').removeAttr('disabled');
            }

            this.$('.footable').trigger('footable_redraw');
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

        },

        reload: function(model, value) {

        },



        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
