define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//pre_event',
//localize
'i18n!strings/nls/pre_event',
//template
'text!templates/settings/video/auto/pre_event.html',
//plugins
'tooltip', 'alert'],

function($, _, Backbone, app, PreEventModel, locale, PreEventTemplate) {

    var view = Backbone.View.extend({

        template: _.template(PreEventTemplate),

        initialize: function() {
            this.model = PreEventModel;
            this.model.bind('change:id', this.updateId, this);
            this.model.bind('change:enable', this.updateEnable, this);
            this.model.bind('change:video_quality', this.updateVideoQuality, this);
            this.model.bind('change:comp_mode', this.updateCompMode, this);
            this.model.bind('change:fps', this.updateFps, this);
            this.model.bind('change:video_duration', this.updateVideoDuration, this);


        },

        events: {
            "change #idcomp_mode": "change_video_duration",
            "change #idfps": "change_video_duration",
            "click #save": "save",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();  
                app.disableAllinput();  
            }

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

        change_video_duration: function() {
            $('#idvideo_duration').empty();
            var selectValues = [10, 20, 30, 40, 50, 60];
            $.each(selectValues, function(key, value) {
                $('#idvideo_duration').append($('<option></option>').attr('value', value).text(value));
            });
            var limit = 0;
            if ($('#idcomp_mode').length > 0 && $('#idfps').length > 0 && $('#idvideo_duration').length > 0) {
                sum = parseInt($('#idcomp_mode').val()) + parseInt($('#idfps').val());
                if (sum > 5) {
                    var limit = 10;
                }
                if ((sum > 3) && (sum <= 5)) {
                    var limit = 30;
                }
                if (limit > 0) {
                    var index;
                    for (index = $('#idvideo_duration > option').length - 1; index > 0; index--) {
                        if ($('#idvideo_duration option').eq(index).val() > limit) {
                            $('#idvideo_duration option:eq(' + index + ')').remove();
                        }
                    }
                }
            }
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
        updateId: function(model, value) {},
        updateEnable: function(model, value) {
            var av = this.model.get('enable');
            var avset = false;
            if (av == '0') {
                avset = true;
                var value = locale.this_page_used_to_configure_the_preevent_video_recording_configurationspreevent_video_recording_is_currently_disabled_to_enable_the_preevent_video_recording_in_a_hrefsettingsvideoautotrigger_settings_styletextdecoration_underlineb_triggers_configuration_ba_page_and_trigger_the_video_;
            }
            if (av == '1' || av =='2') {
                avset = true;
                var value = locale.this_page_used_to_configure_the_preevent_video_recording_configurations_;
            }
            if (avset == false) {
                var value = av;
            }
            $('#iddescription').html(value);
        },
        updateVideoQuality: function(model, value) {
            $('#idvideo_quality').val(value);
        },
        updateCompMode: function(model, value) {
            $('#idcomp_mode').val(value);
            this.change_video_duration();
        },
        updateFps: function(model, value) {
            $('#idfps').val(value);
            this.change_video_duration();
        },
        updateVideoDuration: function(model, value) {
            $('#idvideo_duration').val(value);
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
                'enable': parseInt($('#iddescription').val(), 10),
                'video_quality': parseInt($('#idvideo_quality').val(), 10),
                'comp_mode': parseInt($('#idcomp_mode').val(), 10),
                'fps': parseInt($('#idfps').val(), 10),
                'video_duration': parseInt($('#idvideo_duration').val(), 10)
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
