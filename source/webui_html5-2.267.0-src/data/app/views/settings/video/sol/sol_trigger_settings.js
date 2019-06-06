define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//sol_trigger_settings',
'models/datetime/date_time',
//localize
'i18n!strings/nls/sol_trigger_settings',
//template
'text!templates/settings/video/sol/sol_trigger_settings.html',
//plugins
'moment', 'datepicker', 'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, SolTriggerSettingsModel,DateTimeModel, locale, SolTriggerSettingsTemplate, moment) {

    var view = Backbone.View.extend({

        template: _.template(SolTriggerSettingsTemplate),

        initialize: function() {
			this.datetime = DateTimeModel;
            this.model = SolTriggerSettingsModel;		
            this.model.bind('change:critical_events_enable', this.updateCriticalEventsEnable, this);
            this.model.bind('change:non_critical_events_enable', this.updateNonCriticalEventsEnable, this);
            this.model.bind('change:non_recoverable_events_enable', this.updateNonRecoverableEventsEnable, this);
            this.model.bind('change:fan_state_events_enable', this.updateFanStateEventsEnable, this);
            this.model.bind('change:watchdog_events_enable', this.updateWatchdogEventsEnable, this);
            this.model.bind('change:chasis_power_on_events_enable', this.updateChasisPowerOnEventsEnable, this);
            this.model.bind('change:chasis_power_off_events_enable', this.updateChasisPowerOffEventsEnable, this);
            this.model.bind('change:chasis_reset_events_enable', this.updateChasisResetEventsEnable, this);
            this.model.bind('change:lpc_reset_events_enable', this.updateLpcResetEventsEnable, this);
            this.model.bind('change:date_time_events_enable', this.updateDateTimeEventsEnable, this);
            this.model.bind('change:date_time_to_record', this.updateDateTimeToRecord, this);

            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);
        },

        events: {
            "ifChecked #idcritical_events_enable": "checkedCriticalEventsEnable",
            "ifUnchecked #idcritical_events_enable": "uncheckedCriticalEventsEnable",
            "ifChecked #idnon_critical_events_enable": "checkedNonCriticalEventsEnable",
            "ifUnchecked #idnon_critical_events_enable": "uncheckedNonCriticalEventsEnable",
            "ifChecked #idnon_recoverable_events_enable": "checkedNonRecoverableEventsEnable",
            "ifUnchecked #idnon_recoverable_events_enable": "uncheckedNonRecoverableEventsEnable",
            "ifChecked #idfan_state_events_enable": "checkedFanStateEventsEnable",
            "ifUnchecked #idfan_state_events_enable": "uncheckedFanStateEventsEnable",
            "ifChecked #idwatchdog_events_enable": "checkedWatchdogEventsEnable",
            "ifUnchecked #idwatchdog_events_enable": "uncheckedWatchdogEventsEnable",
            "ifChecked #idchasis_power_on_events_enable": "checkedChasisPowerOnEventsEnable",
            "ifUnchecked #idchasis_power_on_events_enable": "uncheckedChasisPowerOnEventsEnable",
            "ifChecked #idchasis_power_off_events_enable": "checkedChasisPowerOffEventsEnable",
            "ifUnchecked #idchasis_power_off_events_enable": "uncheckedChasisPowerOffEventsEnable",
            "ifChecked #idchasis_reset_events_enable": "checkedChasisResetEventsEnable",
            "ifUnchecked #idchasis_reset_events_enable": "uncheckedChasisResetEventsEnable",
            "ifChecked #idlpc_reset_events_enable": "checkedLpcResetEventsEnable",
            "ifUnchecked #idlpc_reset_events_enable": "uncheckedLpcResetEventsEnable",
            "ifChecked #iddate_time_events_enable": "checkedDateTimeEventsEnable",
            "ifUnchecked #iddate_time_events_enable": "uncheckedDateTimeEventsEnable",
            "click #idpick-date": "showDatePickerPickDate",
            "click #idpick-time": "showTimePickerPickTime",
            "click #save": "save",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
			 var that = this;
            $('#idpick-date').datetimepicker({
                pickTime: false,
                format: 'YYYY-MM-DD'
            });
            $('#idpick-time').datetimepicker({
                pickDate: false,
                useSeconds: false,
                format: 'HH:mm'
            });
            that.$el.find('#idcritical_events_enable,#idnon_critical_events_enable,#idnon_recoverable_events_enable,#idfan_state_events_enable,#idwatchdog_events_enable,#idchasis_power_on_events_enable,#idchasis_power_off_events_enable,#idchasis_reset_events_enable,#idlpc_reset_events_enable,#iddate_time_events_enable').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            that.model.clear();
            that.model.set({});

            that.model.fetch();
            var _parent = that;
            _.each(that.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });
			that.datetime.fetch();
            if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();  
                app.disableAllinput();  
            }
        },

        load: function(model, collection, xhr) {

        },
        reload: function(model, value) {

        },

        checkedCriticalEventsEnable: function(ev) {
            $('#idcritical_events_enable').val(1);
            this.model.set('critical_events_enable', 1);
        },
        uncheckedCriticalEventsEnable: function(ev) {
            $('#idcritical_events_enable').val(0);
            this.model.set('critical_events_enable', 0);
        },
        checkedNonCriticalEventsEnable: function(ev) {
            $('#idnon_critical_events_enable').val(1);
            this.model.set('non_critical_events_enable', 1);
        },
        uncheckedNonCriticalEventsEnable: function(ev) {
            $('#idnon_critical_events_enable').val(0);
            this.model.set('non_critical_events_enable', 0);
        },
        checkedNonRecoverableEventsEnable: function(ev) {
            $('#idnon_recoverable_events_enable').val(1);
            this.model.set('non_recoverable_events_enable', 1);
        },
        uncheckedNonRecoverableEventsEnable: function(ev) {
            $('#idnon_recoverable_events_enable').val(0);
            this.model.set('non_recoverable_events_enable', 0);
        },
        checkedFanStateEventsEnable: function(ev) {
            $('#idfan_state_events_enable').val(1);
            this.model.set('fan_state_events_enable', 1);
        },
        uncheckedFanStateEventsEnable: function(ev) {
            $('#idfan_state_events_enable').val(0);
            this.model.set('fan_state_events_enable', 0);
        },
        checkedWatchdogEventsEnable: function(ev) {
            $('#idwatchdog_events_enable').val(1);
            this.model.set('watchdog_events_enable', 1);
        },
        uncheckedWatchdogEventsEnable: function(ev) {
            $('#idwatchdog_events_enable').val(0);
            this.model.set('watchdog_events_enable', 0);
        },
        checkedChasisPowerOnEventsEnable: function(ev) {
            $('#idchasis_power_on_events_enable').val(1);
            this.model.set('chasis_power_on_events_enable', 1);
        },
        uncheckedChasisPowerOnEventsEnable: function(ev) {
            $('#idchasis_power_on_events_enable').val(0);
            this.model.set('chasis_power_on_events_enable', 0);
        },
        checkedChasisPowerOffEventsEnable: function(ev) {
            $('#idchasis_power_off_events_enable').val(1);
            this.model.set('chasis_power_off_events_enable', 1);
        },
        uncheckedChasisPowerOffEventsEnable: function(ev) {
            $('#idchasis_power_off_events_enable').val(0);
            this.model.set('chasis_power_off_events_enable', 0);
        },
        checkedChasisResetEventsEnable: function(ev) {
            $('#idchasis_reset_events_enable').val(1);
            this.model.set('chasis_reset_events_enable', 1);
        },
        uncheckedChasisResetEventsEnable: function(ev) {
            $('#idchasis_reset_events_enable').val(0);
            this.model.set('chasis_reset_events_enable', 0);
        },
        checkedLpcResetEventsEnable: function(ev) {
            $('#idlpc_reset_events_enable').val(1);
            this.model.set('lpc_reset_events_enable', 1);
        },
        uncheckedLpcResetEventsEnable: function(ev) {
            $('#idlpc_reset_events_enable').val(0);
            this.model.set('lpc_reset_events_enable', 0);
        },
        checkedDateTimeEventsEnable: function(ev) {
            $('#iddate_time_events_enable').val(1);
            this.model.set('date_time_events_enable', 1);
            this.$("#idpick-date,#idpick-time").parents(".form-group").show();
        },
        uncheckedDateTimeEventsEnable: function(ev) {
            $('#iddate_time_events_enable').val(0);
            this.model.set('date_time_events_enable', 0);
            this.$("#idpick-date,#idpick-time").parents(".form-group").hide();
        },
        showDatePickerPickDate: function(ev) {
            $('#idpick-date').data('DateTimePicker').show();
        },
        showTimePickerPickTime: function(ev) {
            $('#idpick-time').data('DateTimePicker').show();
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
        updateCriticalEventsEnable: function(model, value) {
            if (value) {
                $('#idcritical_events_enable').iCheck('check');
            } else {
                if (!$('#idcritical_events_enable').parent().hasClass('checked')) {
                    this.uncheckedCriticalEventsEnable();
                }
                $('#idcritical_events_enable').iCheck('uncheck');
            }
        },
        updateNonCriticalEventsEnable: function(model, value) {
            if (value) {
                $('#idnon_critical_events_enable').iCheck('check');
            } else {
                if (!$('#idnon_critical_events_enable').parent().hasClass('checked')) {
                    this.uncheckedNonCriticalEventsEnable();
                }
                $('#idnon_critical_events_enable').iCheck('uncheck');
            }
        },
        updateNonRecoverableEventsEnable: function(model, value) {
            if (value) {
                $('#idnon_recoverable_events_enable').iCheck('check');
            } else {
                if (!$('#idnon_recoverable_events_enable').parent().hasClass('checked')) {
                    this.uncheckedNonRecoverableEventsEnable();
                }
                $('#idnon_recoverable_events_enable').iCheck('uncheck');
            }
        },
        updateFanStateEventsEnable: function(model, value) {
            if (value) {
                $('#idfan_state_events_enable').iCheck('check');
            } else {
                if (!$('#idfan_state_events_enable').parent().hasClass('checked')) {
                    this.uncheckedFanStateEventsEnable();
                }
                $('#idfan_state_events_enable').iCheck('uncheck');
            }
        },
        updateWatchdogEventsEnable: function(model, value) {
            if (value) {
                $('#idwatchdog_events_enable').iCheck('check');
            } else {
                if (!$('#idwatchdog_events_enable').parent().hasClass('checked')) {
                    this.uncheckedWatchdogEventsEnable();
                }
                $('#idwatchdog_events_enable').iCheck('uncheck');
            }
        },
        updateChasisPowerOnEventsEnable: function(model, value) {
            if (value) {
                $('#idchasis_power_on_events_enable').iCheck('check');
            } else {
                if (!$('#idchasis_power_on_events_enable').parent().hasClass('checked')) {
                    this.uncheckedChasisPowerOnEventsEnable();
                }
                $('#idchasis_power_on_events_enable').iCheck('uncheck');
            }
        },
        updateChasisPowerOffEventsEnable: function(model, value) {
            if (value) {
                $('#idchasis_power_off_events_enable').iCheck('check');
            } else {
                if (!$('#idchasis_power_off_events_enable').parent().hasClass('checked')) {
                    this.uncheckedChasisPowerOffEventsEnable();
                }
                $('#idchasis_power_off_events_enable').iCheck('uncheck');
            }
        },
        updateChasisResetEventsEnable: function(model, value) {
            if (value) {
                $('#idchasis_reset_events_enable').iCheck('check');
            } else {
                if (!$('#idchasis_reset_events_enable').parent().hasClass('checked')) {
                    this.uncheckedChasisResetEventsEnable();
                }
                $('#idchasis_reset_events_enable').iCheck('uncheck');
            }
        },
        updateLpcResetEventsEnable: function(model, value) {
            if (value) {
                $('#idlpc_reset_events_enable').iCheck('check');
            } else {
                if (!$('#idlpc_reset_events_enable').parent().hasClass('checked')) {
                    this.uncheckedLpcResetEventsEnable();
                }
                $('#idlpc_reset_events_enable').iCheck('uncheck');
            }
        },
        updateDateTimeEventsEnable: function(model, value) {
            if (value) {
                $('#iddate_time_events_enable').iCheck('check');
            } else {
                if (!$('#iddate_time_events_enable').parent().hasClass('checked')) {
                    this.uncheckedDateTimeEventsEnable();
                }
                $('#iddate_time_events_enable').iCheck('uncheck');
            }
        },
        updateDateTimeToRecord: function(model, value) {
            value = value+(new Date().getTimezoneOffset()*60);
            $('#idpick-date').data('DateTimePicker').setDate(moment.unix(value));
            $('#idpick-time').data('DateTimePicker').setDate(moment.unix(value));
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
            var elMap = {
                "date_time_to_record": "idpick-date,#idpick-time"
            }, jel;

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
            var date_time_to_recordtime = moment($("#idpick-date").data("DateTimePicker").getDate()).format("YYYY-MM-DD") + ' ' + moment($("#idpick-time").data("DateTimePicker").getDate()).format("HH:mm:ss"),
                date_time_to_record = (parseInt(moment(date_time_to_recordtime).format("X"), 10))-(new Date().getTimezoneOffset()*60);

            $("#save-icon").removeClass().addClass("ion-load-d");
            this.$(".alert-success,.alert-danger").addClass("hide");
            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }
			
			var datetimemodel = context.datetime;
			var NTPtimestamp = datetimemodel.get('localized_timestamp');
			context.model.save({
                'date_time_to_record': date_time_to_record,
				'NTPtimestamp': NTPtimestamp
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
