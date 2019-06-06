define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'models/chassis_status',
        'i18n!strings/nls/power_control',
        //template
        'text!templates/power-control.html',
        //plugins
        'iCheck', 'tooltip', 'alert'
    ],

    function($, _, Backbone, app, ChassisModel, locale, PowerTemplate) {


        var view = Backbone.View.extend({
            feature_server : 0,
            feature_switch : 0,

            template: _.template(PowerTemplate),

            initialize: function() {
                this.model = ChassisModel;
		this.powerstatus;
		this.state_to_expect= 0xf ;
		this.max_retries = 3;
                this.model.bind('change:power_status', this.updatePowerStatus, this);
                feature_server = app.features.indexOf('SERVER_SUPPORT');
                feature_switch = app.features.indexOf('SWITCH_SUPPORT');
            },
            events: {
                "click #save": "perform",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

            afterRender: function() {
                if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                    app.disableAllbutton();
                    app.disableAllinput();
                 }
                 if(feature_server == -1)
                 {
                     this.$el.find('input[name="power_command_switch"]').iCheck({
                         checkboxClass: 'icheckbox_square-blue',
                         radioClass: 'iradio_square-blue',
                         increaseArea: '20%'
                     });
                 }
                 if(feature_switch == -1)
                 {
                     this.$el.find('input[name="power_command_server"]').iCheck({
                         checkboxClass: 'icheckbox_square-blue',
                         radioClass: 'iradio_square-blue',
                         increaseArea: '20%'
                         });
                 }

                if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                   $("#save").addClass("disable_a_href");
                }
                this.model.fetch();
                var _parent = this;
                _.each(this.model.attributes, function(value, attr, list) {
                    _parent.model.trigger('change:' + attr, this.model, value);
                });
            },

            load: function(model, collection, xhr) {

            },

            updatePowerStatus: function(model, value) {
	    if(value == 0) {
	    	$("#chassis_status").html(locale.server_hostoff);
            if(feature_server == -1)
            {
                $('input[name="power_command_switch"]').iCheck('disable');
                $('input[name="power_command_switch"][value="1"]').iCheck('enable');
                $('input[name="power_command_switch"][value="1"]').iCheck('check');
            }
            if(feature_switch == -1)
            {
                $('input[name="power_command_server"]').iCheck('disable');
                $('input[name="power_command_server"][value="1"]').iCheck('enable');
                $('input[name="power_command_server"][value="1"]').iCheck('check');
            }

                this.powerstatus = 1 ;

            } else {
                if(feature_server== -1)
                {
                    $("#chassis_status").html(locale.server_hoston);
                    $('input[name="power_command_switch"]').iCheck('enable');
                    $('input[name="power_command_switch"][value="1"]').iCheck('disable');
        		    $('input[name="power_command_switch"][value="0"]').iCheck('check');
                }
                if(feature_switch == -1)
                {
                    $("#chassis_status").html(locale.server_hoston);
                    $('input[name="power_command_server"]').iCheck('enable');
                    $('input[name="power_command_server"][value="1"]').iCheck('disable');
        		    $('input[name="power_command_server"][value="0"]').iCheck('check');
                }
                this.powerstatus = 3 ;
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
            perform: function(ev) {
	    	var that = this;
                if(!confirm(locale.confirm)) {
                    ev.stopPropagation();
                    ev.preventDefault();
                    return false;
                }
		$("#save-icon").removeClass().addClass("ion-load-d");
		$("#save").addClass("disable_a_href");
        if(feature_server == -1){
            that.powerstatus = parseInt($('input[name="power_command_switch"]').filter(':checked').val());
        }
        else if(feature_switch == -1)
        {
            that.powerstatus = parseInt($('input[name="power_command_server"]').filter(':checked').val());
        }


                switch(that.powerstatus)
		{
			case 0:
				that.state_to_expect = 0;
				break;
			case 1:
			case 2:
			case 3:
				that.state_to_expect = 1;
				break;
			case 5:
				that.state_to_expect = 0;
				break;
		}
                this.model.power(that.powerstatus, {
                    success: function(model){
                       	$(".alert-success").removeClass("hide").html("").html(locale.power_action_wait);
                       	$(".alert-danger").addClass("hide");
						if(that.powerstatus == 2)
							inittmout = 20000; //for power cycle we check if host is powered on again after 10 secs
						else if(that.powerstatus == 5)
							inittmout = 30000;
						else
							inittmout = 10000;
						that.max_retries = 3;
						setTimeout(function(){
							that.getStatus();
						},inittmout);
                    },
                    error: function(model){
                    	$(".alert-danger").removeClass("hide").html(locale.str_server_setaction);
                    	$("#save").removeClass("disable_a_href");
                        $("#save-icon").removeClass().addClass("fa fa-power-off");
                    }
                });
            },
			getStatus : function(){
				var that = this;
				this.model.fetch({
                    			global : false ,
					success : function(model, response, options) {
						that.getHostStatusRes(model);
					}
				});
			},
			getHostStatusRes : function(res){
				current_state = res.get('power_status');
				var that = this;
				if(this.state_to_expect != 0xf)
				{
					if(current_state != this.state_to_expect)
					{
						this.max_retries--;
						$(".alert-success").removeClass("hide").html("").html(locale.host_retrying+this.max_retries);
						if(this.powerstatus == 5){
							timeout = 20000;
							error_text = locale.str_server_err1;
						}
						else{
							timeout = 10000;
							error_text = locale.str_server_err2;
						}
						if(this.max_retries == 0)
						{
							$(".alert-success").addClass("hide");
							$(".alert-danger").removeClass("hide").html(error_text);
							$("#save-icon").removeClass().addClass("fa fa-power-off");
							$("#save").removeClass("disable_a_href");
							return;
						}

						setTimeout(function()
						{
							that.getStatus();
						},timeout);

						return;
					}else{
						$(".alert-success,.alert-danger").addClass("hide");
						$("#save").removeClass("disable_a_href");
						$("#save-icon").removeClass().addClass("fa fa-power-off");
					}
				}
				$("#host_status i").removeClass("text-success text-muted");
				if(current_state) {
					$("#host_status i").addClass("text-success");
					$("#host_status span").html(locale.online);
					$("#chassis_status").html(locale.server_hoston);
				} else {
					$("#host_status i").addClass("text-muted");
					$("#chassis_status").html(locale.server_hostoff);
					$("#host_status span").html(locale.offline);
				}

			},
            serialize: function() {
                return {
                    locale: locale
                };
            }

        });

        return view;

    });
