define(['jquery', 'underscore', 'backbone', 'app',
//data
'models/datetime/date_time',
'timezone-meta-data',
//localize
'i18n!strings/nls/date_time',
//template
'text!templates/settings/date_time.html',
//plugins
'moment-timezone', 'datepicker', 'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, DateTimeModel, tzMetaData, locale, DateTimeTemplate, moment) {

    var view = Backbone.View.extend({

        template: _.template(DateTimeTemplate),

        first : 0,
        last : 0,
        now : 0,
        elapsed : 0, 
        mapPoints : [],
        lastPoint : 0,
        clickPoint : 0,

        initialize: function() {
            this.model = DateTimeModel;
            this.model.bind('change:timestamp', this.updateTimestamp, this);
            this.model.bind('change:utc_minutes', this.updateUtcMinutes, this);
            this.model.bind('change:timezone', this.updateTimezone, this);
            this.model.bind('change:primary_ntp', this.updatePrimaryNtp, this);
            this.model.bind('change:secondary_ntp', this.updateSecondaryNtp, this);
            this.model.bind('change:ntp_server_status', this.updateNtpServerStatus, this);
            this.model.bind('change:auto_date', this.updateAutoDate, this);
            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);

            // var parent = this;
            // $(window).on('resize', function(){
            //     parent.resizeMap.call(parent);
            // });
        },

        events: {
            // "click #iddate-readonly": "showDatePickerPickDate",
            // "click #iddate-picker": "showTimePickerPickTime",
            "ifChecked #idauto_sync_ntp": "checkedAutoSyncNtp", 
            "ifUnchecked #idauto_sync_ntp": "uncheckedAutoSyncNtp",
            // "change #idtimezone": "changeTimeZone",
            "mousemove #id_map_inner": "updateMapPoint",
            "mouseleave #id_map_inner": "revertMapPoint",
            "click #id_map_inner": "clickMap",
            // "click #refresh": "refreshData",
            //"click #save": "save",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp",
            "dp.change": "updateDatePicker"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();  
                app.disableAllinput();  
            }
            $('#iddate-picker').datetimepicker({
                pickDate: true,
                useSeconds: true,
                format: 'll h:mm:ss A'
            });
            this.$el.find('#idauto_sync_ntp').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            this.model.clear();
            this.model.set({});
			var modelXHR = this.model.fetch({
				success: function(model, response, options){ 
					if(response['auto_date'] == 2){
						$(".alert-danger").removeClass("hide").html(locale.invalidserver);
					}
				}
			});
            
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });
            // enumerate available timezones
           /* this.tzNameList = moment.tz.names();
            var uscommons = ["America/Puerto_Rico",
                            "America/New_York",
                            "America/Chicago",
                            "America/Denver",
                            "America/Phoenix",
                            "America/Los_Angeles",
                            "America/Anchorage",
                            "Pacific/Honolulu"];

            for(i in this.tzNameList) {
                var zone = this.tzNameList[i];
                var group = zone.split('/')[0];
                var name = zone.split('/')[1];
                var zonestring = (name !== undefined) ? name.replace(/_/g, ' ') : zone;
                zonestring = '(GMT' + moment().tz(zone).format('Z z') + ') ' + zonestring;
                if(_.indexOf(uscommons, zone) > -1) {
                    $('#US_Common').append($('<option></option>').attr('value', zone).text(zonestring));
                } else {
                    $('#'+group).append($('<option></option>').attr('value', zone).text(zonestring));
                }
            }*/

            this.map = $('#id_map_inner');

            this.width = this.map.outerWidth();
            this.height = this.map.outerHeight();  

            var that = this;

            function Point (data) {
                this.name = data.name;
                this.x = (180 + data.long) / 360;
                this.y = (90 - data.lat) / 180;
                this.dom = $("<span>").appendTo(that.map).css({
                    'left': this.x * 100 + '%',
                    'top': this.y * 100 + '%'  
                });
            }

            Point.prototype = {
                distance : function(x,y) {
                    var dx = this.x - x;
                    var dy = this.y - y;
                    return dx * dx + dy * dy;
                },
                activate : function() {
                    var m = moment().tz(this.name);
                    // $('#id_tzmap_name').text(this.name);
                    // $('#id_tzmap_label').text(m.format('YYYY-MM-DD HH:mm:ss Z z'));
                    that.model.set('timezone', this.name);
                    $('.tz-clicked').removeClass('tz-clicked');
                    this.dom.addClass('tz-clicked');
                    $('#map_x').css('left', this.x * 100 + '%');
                    $('#map_y').css('top', this.y * 100 + '%');
                },
                deactivate : function() {
                    this.dom.removeClass('active');
                }
            }

            function timestampLoad () {
                for(var i in that.mapPoints) {
                    if(that.mapPoints[i].name == that.model.get('timezone')) {
                        that.newPoint(that.mapPoints[i]);
                        that.clickPoint = that.mapPoints[i];
                    }
                }
            }

            for(var tz in tzMetaData.zones) {
                this.mapPoints.push(new Point(tzMetaData.zones[tz]));
            }

            if(modelXHR.readyState == 4) {
                timestampLoad();
            } else {
                modelXHR.done(function() {
                    timestampLoad();
                });
            }

            this.first = Date.now() / 1000;
            this.updateSeconds.call(this);
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

        showDatePickerPickDate: function(ev) {
            if(!this.model.get('auto_date')) {
                $('#iddate-readonly').data('DateTimePicker').show();
            }
        },
        showTimePickerPickTime: function(ev) {
            $('#iddate-picker').data('DateTimePicker').show();
        },
        checkedAutoSyncNtp: function(ev) {
            this.model.set('auto_date', 1);
            $('#iddate-picker').data('DateTimePicker').disable();
            this.$(".clock-icon").addClass('hide');
            this.$("#iddate-picker :input").addClass("tsReadOnly");
            this.$("#idprimary_ntp,#idsecondary_ntp").removeAttr("disabled");
            this.$("#idprimary_ntp,#idsecondary_ntp").iCheck("enable");
        },
        uncheckedAutoSyncNtp: function(ev) {
            this.model.set('auto_date', 0);
            $('#iddate-picker').data('DateTimePicker').enable();
            this.$("#iddate-picker :input").removeClass("tsReadOnly");
            this.$(".clock-icon").removeClass('hide');
            this.$("#idprimary_ntp,#idsecondary_ntp").attr("disabled", "disabled");
            this.$("#idprimary_ntp,#idsecondary_ntp").iCheck("disable");
        },
        changeTimeZone: function(ev) {
            // this.model.set("timezone", $('#idtimezone').val());
        },
        refreshData: function(e) {
            e.preventDefault();
            if (this.model) {
                var that = this;
                this.model.fetch({
                    success : function () {
                        for(var i in that.mapPoints) {
                            if(that.mapPoints[i].name == that.model.get('timezone')) {
                                that.newPoint(that.mapPoints[i]);
                                that.clickPoint = that.mapPoints[i];
                            }
                        }
                    }
                });
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
        updateTimestamp: function(model, value) {
            var tzone = model.get("timezone");
            var m = moment.unix(value);
            if(tzone !== undefined) {
                m = m.tz(tzone);
            }
            if(this.model.get('auto_date')) {
                $('#iddate-picker').data('DateTimePicker').setDate(m);
            }
        },
        updateUtcMinutes: function(model, value) {},
        updateTimezone: function(model, value) {
            // $('#idtimezone').val(value);
            var tstamp = model.get("timestamp");
            if(tstamp !== undefined) {
                if(!this.model.get('auto_date')) {
                    //var m = $('#iddate-picker').data('DateTimePicker').getDate();
                    var m = moment.unix(tstamp);
                } else {
                    //var m = moment.unix(tstamp);
                    var m = $('#iddate-picker').data('DateTimePicker').getDate();
                }
                var group = value.split('/')[0];
                var name = value.split('/')[1];
                var zonestring = (name !== undefined) ? name.replace(/_/g, ' ') : value;
                m.tz(value);
                zonestring = 'll h:mm:ss A [(GMT' + m.format('Z z') + ')  -  ' + group + '/' + zonestring+']';
                $('#iddate-picker').data('DateTimePicker').format = zonestring;
                $('#iddate-picker').data('DateTimePicker').setDate(m);
            }
        },
        updatePrimaryNtp: function(model, value) {
            $('#idprimary_ntp').val(value);
        },
        updateSecondaryNtp: function(model, value) {
            $('#idsecondary_ntp').val(value);
        },
        updateNtpServerStatus: function(model, value) {},
        updateAutoDate: function(model, value) {
            if (value) {
                $('#idauto_sync_ntp').iCheck('check');
            } else {
                if (!$('#idauto_sync_ntp').parent().hasClass('checked')) {
                    this.uncheckedAutoSyncNtp();
                }
                $('#idauto_sync_ntp').iCheck('uncheck');
            }
        },

        updateDatePicker: function(ev) {
            if(!this.model.get('auto_date')) {
                this.model.set('timestamp', ev.date.valueOf() / 1000);
            }
        },

        updateMapPoint: function(ev) {
            var offset = this.map.offset(),
                x = ev.pageX - offset.left,
                y = ev.pageY - offset.top,
                px = x / this.map.outerWidth(),
                py = y / this.map.outerHeight(),
                dist,
                closestDist = 100,
                closestPoint,
                i;

            for(i = 0; i < this.mapPoints.length; i++) {
                dist = this.mapPoints[i].distance(px, py);
                if(dist < closestDist) {
                    closestPoint = this.mapPoints[i];
                    closestDist = dist;
                }
            }

            if(closestPoint) {
                this.newPoint(closestPoint);
            }
        },

        clickMap: function(ev) {
            this.clickPoint = this.lastPoint;
            console.log(this.clickPoint.name);
        },

        revertMapPoint: function(ev) {
            this.newPoint(this.clickPoint);
        },

        newPoint: function (point) {
            if(point === this.lastPoint) {
                return;
            }
            if(this.lastPoint) {
                this.lastPoint.deactivate();
            }
            point.activate();
            this.lastPoint = point;
        },

        resizeMap: function () {
            this.width = this.map.outerWidth();
            this.height = this.map.outerHeight(); 
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
                "primary_ntp": "idprimary_ntp",
                "secondary_ntp":"idsecondary_ntp"
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

        saveConfirm: function() {
            if (!confirm(locale.saveConfirmLabel)) return;
            this.save.call(this);
        },

        save: function() {
	    $('#Validationerror').css('display','none');
            $("#save-icon").removeClass().addClass("ion-load-d");
            this.$(".alert-success,.alert-danger").addClass("hide");

            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }
	    var primary_ntp = $('#idprimary_ntp').val();
            var secondary_ntp = $('#idsecondary_ntp').val();

		/*if(primary_ntp == secondary_ntp )
		{
		   $('#Validationerror').html(locale.samentpserver);
		   $('#Validationerror').css('display','block');
		   context.$("#save-icon").removeClass().addClass("fa fa-save");
			return false;
		}*/
            context.model.save({
                'utc_minutes': -1 * moment.tz.zone(context.model.get('timezone')).offset(context.model.get('timestamp')),
                'primary_ntp': $('#idprimary_ntp').val(),
                'secondary_ntp': $('#idsecondary_ntp').val()
            }, {
                success: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    alert(locale.strongSuccessMsg);
                    $("body").addClass("disable_a_href");
                    app.logout();
                    document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                    app.router.navigate("login");
                    location.reload();
                },

                error: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', context);
                },
                silent: true
            });


        },

        serialize: function() {
            return _.extend({
                locale: locale
            }, this.model.toJSON());
        },

        updateSeconds: function() {
            this.last = this.elapsed;
            this.now = Date.now() / 1000;
            this.elapsed = this.now - this.first;
            if (this.model) {
                if(this.elapsed > 60) {
                    var bkpModel = this.model.clone();
                    var that = this;
                    bkpModel.fetch({
                        success : function(model, response, options) {
                            that.model.set('timestamp', model.get('timestamp'));
                        }
                    });
                    this.last = 0;
                    this.elapsed = 0;
                    this.now = 0;
                    this.first = Date.now() / 1000;
                } else {
                    this.model.set("timestamp", this.model.get("timestamp") - Math.round(this.last) + Math.round(this.elapsed));
                }
            }
            this.timer = setTimeout(_.bind(this.updateSeconds, this), 1000);
        }

    });

    return view;

});
