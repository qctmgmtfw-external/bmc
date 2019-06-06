define(['jquery', 'underscore', 'backbone', 'app',
        //data
        "models/script",
        //template
        'text!templates/script-edit.html',
        //localize
        'i18n!strings/nls/common',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline',
        // 'datepicker',
        // 'iCheck'
        'bower/codemirror/lib/codemirror',
        'bower/codemirror/mode/python/python',
        'modal'
    ],

    function($, _, Backbone, app, TestScriptModel, tpl, CommonStrings, CodeMirror) {

        var view = Backbone.View.extend({

            template: _.template(tpl),

            initialize: function() {},

            events: {
                //"click .btn-close": "closeModal",
                "click .btn-save": "save",
                "click .btn-test": "runScript"
            },

            beforeRender: function() {
                this.$el.addClass("modal fade").attr("id", "script-editor-modal");
            },

            afterRender: function() {
                //this.$el.appendTo("body");
                this.$el.modal({
                    //show: 'false'
                });
                this.$el.modal('hide');
                this.editor = CodeMirror.fromTextArea($("#script-code").get(0), {
                    lineNumbers: true,
                    indentWithTabs: true,
                    mode: "python"
                });

            },

            setData: function(evdata) {
                this.model = evdata.model;
                this.$el.find(".modal-title").html(evdata.model.get("name"));
                this.$el.find("#script-code").val(evdata.model.get("code"));
            },

            show: function() {
                this.$el.modal('show').show();
                this.editor.setValue(this.model.get("code"));
                this.editor.setCursor({
                    line: 1,
                    ch: 1
                });
                //console.log(this.$el);
            },

            closeModal: function() {
                this.$el.modal('hide');
            },

            save: function() {
                var context = this;
                this.model.set("name", $(".modal-title").html());
                this.model.set("code", this.editor.getValue());
                context.$el.modal('hide');

                // this.model.save({}, {
                //     success: function() {
                //         context.$el.modal('hide');
                //     }
                // });
            },

            runScript: function() {
                var test = new TestScriptModel({
                    "id": 1,
                    "name": $(".modal-title").html(),
                    "code": this.editor.getValue()
                });

                test.save({}, {
                    "url": "/api/ae/test"
                });
            },

            load: function(model, collection, xhr) {

            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return new view();

    });