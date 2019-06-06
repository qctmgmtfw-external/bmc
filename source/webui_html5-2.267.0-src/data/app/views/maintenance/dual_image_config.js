define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//dual_image_config',
//localize
'i18n!strings/nls/dual_image_config',
//template
'text!templates/maintenance/dual_image_config.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, DualImageConfigModel, locale, DualImageConfigTemplate) {

    var view = Backbone.View.extend({

        template: _.template(DualImageConfigTemplate),

        initialize: function() {
            this.model = DualImageConfigModel;
            this.model.bind('change:fw_version_1', this.updateFwVerion1, this);
            this.model.bind('change:state_1', this.updateState1, this);
            this.model.bind('change:fw_version_2', this.updateFwVerion2, this);
            this.model.bind('change:state_2', this.updateState2, this);
            this.model.bind('change:boot_image', this.updateDualImageConfig, this);


        },

        events: {
            "ifChecked input[name='boot_image']": "checkedDualImageConfig",
            "ifUnchecked input[name='boot_image']": "uncheckedDualImageConfig",
            "click #save": "save",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.$el.find('input[name="boot_image"]').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            this.model.clear();
            this.model.set({});

            this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

        checkedDualImageConfig: function(ev) {
            console.log('checked radio', $(ev.target).filter(':checked').val());
            this.model.set('boot_image', $(ev.target).filter(':checked').val());


        },
        uncheckedDualImageConfig: function() {},
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
            $('.tooltip').hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        updateFwVerion1: function(model, value) {
        	$('#idfw_version_1').html(value);
        },
        updateState1: function(model, value) {
            $('#idstate_1').html(value);
        },
        updateFwVerion2: function(model, value) {
            $('#idfw_version_2').html(value);
        },
        updateState2: function(model, value) {
            $('#idstate_2').html(value);
        },
        updateDualImageConfig: function(model, value) {
            $('input[name="boot_image"]').iCheck('uncheck');
            $('input[name="boot_image"][value="' + value + '"]').iCheck('check');
        },

        validData: function(model) {
            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }
        },

        errorFields: [],

        invalidData: function(model, errors) {
            console.log('invalid', errors);
            var elMap = [],
                jel;

            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }

            for (var e in errors) {
                jel = this.$el.find("#" + elMap[e]);
                jel.tooltip({
                    animation: false,
                    title: errors[e],
                    trigger: 'manual',
                    placement: 'top'
                });
                jel.tooltip('show');
                this.errorFields.push(jel);


            }
            $("#save-icon").removeClass().addClass("fa fa-save");
        },

        save: function() {


            $("#save-icon").removeClass().addClass("ion-load-d");
            this.$(".alert-success,.alert-danger").addClass("hide");


            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }

            context.model.save({
                'boot_image': $("input:radio[name='boot_image']:checked").val()
            }, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', context);
                },

                error: function(collection, response, options) {
                    var errorcode = '';
                    var errormsg = '';
                    if (response.responseJSON.code != undefined) errorcode = response.responseJSON.code;
                    else if (response.responseJSON.cc != undefined) errorcode = response.responseJSON.cc;
                    if (response.responseJSON.error != undefined) errormsg = response.responseJSON.error;

                    if (typeof context.getStdErrMsg == 'function') {
                        context.getStdErrMsg(errorcode, errormsg);
                    }


                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', context);
                }

            });


        },

        serialize: function() {
            return _.extend({
                locale: locale
            }, this.model.toJSON());
        }

    });

    return view;

});
