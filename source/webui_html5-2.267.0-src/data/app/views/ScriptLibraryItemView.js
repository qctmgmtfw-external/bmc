define(['jquery', 'underscore', 'backbone', 'app',
        //data

        //template
        'text!templates/script-library-item.html',
        //localize
        'i18n!strings/nls/common',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
        'tooltip'
    ],

    function($, _, Backbone, app, ScriptItemTpl, CommonStrings) {

        var view = Backbone.View.extend({

            tagName: 'li',

            template: _.template(ScriptItemTpl),

            initialize: function() {
                this.model.bind("change:name", this.updateScriptName, this);
                this.model.bind("remove", this.removeItem, this);
            },

            events: {
                //"click .script-edit": "openItem",
                "click .script-import": "importScript"
            },

            updateID: function(model, value) {
                this.$el.data("id", value);
            },

            updateScriptName: function(model, value) {
                this.$el.find(".script-title", value);
            },


            beforeRender: function() {
                //this.$el.addClass("row");
            },

            afterRender: function() {
                this.$el.find(".script-delete").tooltip({
                    placement: 'top',
                    title: CommonStrings.click_confirm_deletion,
                    trigger: 'click'
                });
            },

            importScript: function() {
                this.trigger("import", {script_id: this.model.get('id')});
            },

            removeItem: function() {
                this.$el.remove();
            },

            serialize: function() {
                return {
                    locale: CommonStrings,
                    title: this.model.get('name')
                };
            }

        });

        return view;

    });