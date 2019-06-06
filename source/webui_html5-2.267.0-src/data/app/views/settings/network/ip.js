define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//ip', 'collection//ipv6_static',
//localize
'i18n!strings/nls/ip',
//template
'text!templates/settings/network/ip.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, IpCollection, Ipv6StaticCollection, locale, IpTemplate) {

    var view = Backbone.View.extend({

        template: _.template(IpTemplate),

        initialize: function() {
            this.collection = IpCollection;
            this.cltn_ipv6_static = Ipv6StaticCollection;
            this.lanInfo = [];
            this.collection.bind('add', this.add, this);
            this.collection.bind('reset', this.reset, this);
            this.collection.bind('remove', this.remove, this);
            this.collection.bind('change', this.change, this);
            this.collection.bind('change add reset remove', this.affect, this);
            this.cltn_ipv6_static.bind('add', this.add_ipv6_static, this);
            this.cltn_ipv6_static.bind('reset', this.reset_ipv6_static, this);
            this.cltn_ipv6_static.bind('remove', this.remove_ipv6_static, this);
            this.cltn_ipv6_static.bind('change', this.change_ipv6_static, this);
            this.cltn_ipv6_static.bind('change add reset remove', this.affect_ipv6_static, this);

            this.collection.bind('validated:valid', this.validData, this);
            this.collection.bind('validated:invalid', this.invalidData, this);

        },

        events: {
            "ifChecked #idlan_enable": "checkedLanEnable",
            "ifUnchecked #idlan_enable": "uncheckedLanEnable",
            "change #idinterface_name": "switchModel",
            "ifChecked #idipv4_enable": "checkedIpv4Enable",
            "ifUnchecked #idipv4_enable": "uncheckedIpv4Enable",
            "ifChecked #idipv4_dhcp_enable": "checkedIpv4DhcpEnable",
            "ifUnchecked #idipv4_dhcp_enable": "uncheckedIpv4DhcpEnable",
            "ifChecked #idipv6_enable": "checkedIpv6Enable",
            "ifUnchecked #idipv6_enable": "uncheckedIpv6Enable",
            "ifChecked #idipv6_dhcp_enable": "checkedIpv6DhcpEnable",
            "ifUnchecked #idipv6_dhcp_enable": "uncheckedIpv6DhcpEnable",
            "change #idipv6_index": "switchModelSecondary",
            "ifChecked #idvlan_enable": "checkedVlanEnable",
            "ifUnchecked #idvlan_enable": "uncheckedVlanEnable",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.$el.find('#idlan_enable,#idipv4_enable,#idipv4_dhcp_enable,#idipv6_enable,#idipv6_dhcp_enable,#idvlan_enable').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
            this.cltn_ipv6_static.fetch();
            if(!(sessionStorage.privilege_id < CONSTANTS["ADMIN"])){
                this.collection.fetch();
                this.collection.each(function(model) {
                    this.add(model, this.collection);
                }, this);
            }
        },

        add: function(model, collection, options) {
            var addr;
            var index = 0;
            var v6DhcpEnable = model.get('ipv6_dhcp_enable');
            this.lanInfo[model.get('interface_name')] = model;
            //$('#idinterface_name').append($('<option></option>').attr('value', model.get('interface_name')).text(model.get('interface_name')));
            $('#idinterface_name').append($('<option></option>').attr('value', model.get('interface_name')).text(model.get('interface_name').split(" ")[0]));
            if(v6DhcpEnable == 1)
            	index = 3;
            else
            	index = 16;
            //console.log("index "+ index + " v6DhcpEnable " + v6DhcpEnable);
			for(addr = 1;addr<=index;addr++){
				if(model.get('ipv6_address').split(" ")[addr] != "::")
					$('#mutiIpv6').append($('<option></option>').attr('value',model.get('ipv6_address')).text(model.get('ipv6_address').split(" ")[addr]));
			}
            if($('#idinterface_name option').length == this.collection.length){
                this.switchModel({
                    target: $('#idinterface_name').get(0)
                });
            }
        },

        reset: function(model, collection, options) {

        },

        remove: function(model, collection, options) {

        },

        change: function(model, collection, options) {

        },

        affect: function(model, collection, options) {

        },

        reload: function(model, value) {

        },

        modelSwitched: function() {
            var interfaceName = $("#idinterface_name").val();
            var interfaceString = {"ETH0": "eth0", "ETH1": "eth1", "Undefined": "undefined"};
            if((this.collection.length == 1) || (typeof(this.lanInfo[interfaceString.ETH0]) == interfaceString.Undefined) || (typeof(this.lanInfo[interfaceString.ETH1]) == interfaceString.Undefined) || (interfaceName == interfaceString.ETH0 && this.lanInfo[interfaceString.ETH1].get("lan_enable") == 0) || (interfaceName == interfaceString.ETH1 && this.lanInfo[interfaceString.ETH0].get("lan_enable") == 0)) {
                $('#idlan_enable').iCheck('check');
                $('#idlan_enable').attr("disabled", "disabled");
                $('#idlan_enable').parent().addClass("disabled");
            }else{
                $('#idlan_enable').removeAttr("disabled");
                $('#idlan_enable').parent().removeClass("disabled");
                if (this.model.get('lan_enable')) {
                    $('#idlan_enable').iCheck('check');
                } else {
                    if (!$('#idlan_enable').parent().hasClass('checked')) {
                        this.uncheckedLanEnable();
                    }
                    $('#idlan_enable').iCheck('uncheck');
                }
            }

            $('#idmac_address').html(this.model.get('mac_address'));
            if (this.model.get('ipv4_enable')) {
                $('#idipv4_enable').iCheck('check');
            } else {
                if (!$('#idipv4_enable').parent().hasClass('checked')) {
                    this.uncheckedIpv4Enable();
                }
                $('#idipv4_enable').iCheck('uncheck');
            }
            if (this.model.get('ipv4_dhcp_enable')) {
                $('#idipv4_dhcp_enable').iCheck('check');
            } else {
                if (!$('#idipv4_dhcp_enable').parent().hasClass('checked')) {
                    this.uncheckedIpv4DhcpEnable();
                }
                $('#idipv4_dhcp_enable').iCheck('uncheck');
            }
            $('#idipv4_address').val(this.model.get('ipv4_address'));
            $('#idipv4_subnet').val(this.model.get('ipv4_subnet'));
            $('#idipv4_gateway').val(this.model.get('ipv4_gateway'));
            if (this.model.get('ipv6_enable')) {
                $('#idipv6_enable').iCheck('check');
            } else {
                if (!$('#idipv6_enable').parent().hasClass('checked')) {
                    this.uncheckedIpv6Enable();
                }
                $('#idipv6_enable').iCheck('uncheck');
            }
            if (this.model.get('ipv6_dhcp_enable')) {
                $('#idipv6_dhcp_enable').iCheck('check');
            } else {
                if (!$('#idipv6_dhcp_enable').parent().hasClass('checked')) {
                    this.uncheckedIpv6DhcpEnable();
                }
                $('#idipv6_dhcp_enable').iCheck('uncheck');
            }
            $('#idipv6_index').html('');
            _.each(this.cltn_ipv6_static.filter(function(model) {
                return true && model.get('interface_name') == $('#idinterface_name').val();
            }), function(model) {
                $('#idipv6_index').append($('<option></option>').attr('value', model.get('ipv6_index')).text(model.get('ipv6_index')));
            });
            $('#idipv6_index').val(this.model.get('ipv6_index'));
            //$('#idipv6_address').val(this.model.get('ipv6_address'));
            $('#idipv6_address').val(this.model.get('ipv6_address').split(" ")[1]);
            $('#idipv6_prefix').val(this.model.get('ipv6_prefix'));
            if (this.model.get('vlan_enable')) {
                $('#idvlan_enable').iCheck('check');
            } else {
                if (!$('#idvlan_enable').parent().hasClass('checked')) {
                    this.uncheckedVlanEnable();
                }
                $('#idvlan_enable').iCheck('uncheck');
            }
            $('#idvlan_id').val(this.model.get('vlan_id'));
            $('#idvlan_priority').val(this.model.get('vlan_priority'));
            //Quanta---
            // The div's display will be set to 'block' even we have set 'none' by .hide() method in some case. Add a timout(recursive) timer to correct it.
            function checkItemDisabled(){
            	if($('#idipv6_enable').val()==0){
            		if($("#idipv6_dhcp_enable").parents(".form-group").css("display") != "none"){
            			this.$("#idipv6_dhcp_enable,#idipv6_address,#idipv6_index,#idipv6_prefix,#mutiIpv6").parents(".form-group").hide();
            			setTimeout(checkItemDisabled,100);
            		}
            	}
            }
            setTimeout(checkItemDisabled, 100);
            //---Quanta
        },

        checkedLanEnable: function(ev) {
            $('#idlan_enable').val(1);
            //this.model.set('lan_enable', 1);//Quanta
            this.$("#idipv4_enable,#idipv6_enable,#idipv4_dhcp_enable,#idipv4_address,#idipv4_subnet,#idipv4_gateway,#idipv6_address,#idipv6_index,#idipv6_prefix,#idipv6_dhcp_enable,#idvlan_enable,#idvlan_id,#idvlan_priority,#idmac_address").parents(".form-group").show();
        },
        uncheckedLanEnable: function(ev) {
            $('#idlan_enable').val(0);
            //this.model.set('lan_enable', 0);//Quanta
            this.$("#idipv4_enable,#idipv6_enable,#idipv4_dhcp_enable,#idipv4_address,#idipv4_subnet,#idipv4_gateway,#idipv6_address,#idipv6_index,#idipv6_prefix,#idipv6_dhcp_enable,#idvlan_enable,#idvlan_id,#idvlan_priority,#idmac_address").parents(".form-group").hide();
        },
        switchModel: function(ev) {
            this.model = this.collection.find(function(model) {
                return model.get('interface_name') == $(ev.target).val();
            });
            this.modelSwitched.call(this);
        },
        checkedIpv4Enable: function(ev) {
            $('#idipv4_enable').val(1);
            //this.model.set('ipv4_enable', 1);//Quanta
            this.$("#idipv4_dhcp_enable,#idipv4_address,#idipv4_subnet,#idipv4_gateway").parents(".form-group").show();
        },
        uncheckedIpv4Enable: function(ev) {
            $('#idipv4_enable').val(0);
            //this.model.set('ipv4_enable', 0);//Quanta
            this.$("#idipv4_dhcp_enable,#idipv4_address,#idipv4_subnet,#idipv4_gateway").parents(".form-group").hide();
        },
        checkedIpv4DhcpEnable: function(ev) {
            $('#idipv4_dhcp_enable').val(1);
            //this.model.set('ipv4_dhcp_enable', 1);//Quanta
            this.$("#idipv4_address,#idipv4_subnet,#idipv4_gateway").attr("disabled", "disabled");
            this.$("#idipv4_address,#idipv4_subnet,#idipv4_gateway").iCheck("disable");
        },
        uncheckedIpv4DhcpEnable: function(ev) {
            $('#idipv4_dhcp_enable').val(0);
            //this.model.set('ipv4_dhcp_enable', 0);//Quanta
            this.$("#idipv4_address,#idipv4_subnet,#idipv4_gateway").removeAttr("disabled");
            this.$("#idipv4_address,#idipv4_subnet,#idipv4_gateway").iCheck("enable");
        },
        checkedIpv6Enable: function(ev) {
            $('#idipv6_enable').val(1);
            //this.model.set('ipv6_enable', 1);//Quanta
            //this.$("#idipv6_dhcp_enable,#idipv6_address,#idipv6_index,#idipv6_prefix").parents(".form-group").show();//Quanta
            this.$("#idipv6_dhcp_enable,#idipv6_address,#idipv6_index,#idipv6_prefix,#mutiIpv6").parents(".form-group").show();//Quanta
        },
        uncheckedIpv6Enable: function(ev) {
            $('#idipv6_enable').val(0);
            //this.model.set('ipv6_enable', 0);//Quanta
            //this.$("#idipv6_dhcp_enable,#idipv6_address,#idipv6_index,#idipv6_prefix").parents(".form-group").hide();//Quanta
            this.$("#idipv6_dhcp_enable,#idipv6_address,#idipv6_index,#idipv6_prefix,#mutiIpv6").parents(".form-group").hide();//Quanta
        },
        checkedIpv6DhcpEnable: function(ev) {
            $('#idipv6_dhcp_enable').val(1);
            var interfaceName = $("#idinterface_name").val();

            if(this.lanInfo[interfaceName].get("ipv6_dhcp_enable") == 1){
                this.switchModel({
                    target: $('#idinterface_name').get(0)
                });
            }
            //this.model.set('ipv6_dhcp_enable', 1);//Quanta
            this.$("#idipv6_address,#idipv6_index,#idipv6_prefix").attr("disabled", "disabled");
            this.$("#idipv6_address,#idipv6_index,#idipv6_prefix").iCheck("disable");
        },
        uncheckedIpv6DhcpEnable: function(ev) {
            $('#idipv6_dhcp_enable').val(0);
            //this.model.set('ipv6_dhcp_enable', 0);//Quanta
            this.$("#idipv6_address,#idipv6_index,#idipv6_prefix").removeAttr("disabled");
            this.$("#idipv6_address,#idipv6_index,#idipv6_prefix").iCheck("enable");
        },
        switchModelSecondary: function(ev) {
            var interfaceName = $("#idinterface_name").val();
            if(this.lanInfo[interfaceName].get("ipv6_address") && $(ev.target).val() == 0){
                //$("#idipv6_address").val(this.lanInfo[interfaceName].get("ipv6_address"));
                $("#idipv6_address").val(this.lanInfo[interfaceName].get("ipv6_address").split(" ")[1]);
            }else{
                var changed_model = this.cltn_ipv6_static.find(function(model) {
                    return model.get('ipv6_index') == $(ev.target).val() && model.get('interface_name') == $('#idinterface_name').val();
                });
                this.modelSwitchedSecondary.call(this, changed_model, this.cltn_ipv6_static);
            }
        },
        add_ipv6_static: function(model, collection, option) {},
        reset_ipv6_static: function(model, collection, option) {},
        affect_ipv6_static: function(model, collection, option) {},
        change_ipv6_static: function(model, collection, option) {},
        modelSwitchedSecondary: function(model, collection) {
            $('#idipv6_address').val(model.get('ipv6_address'));
        },
        checkedVlanEnable: function(ev) {
            $('#idvlan_enable').val(1);
            //this.model.set('vlan_enable', 1);//Quanta
            this.$("#idvlan_id,#idvlan_priority").removeAttr("disabled");
            this.$("#idvlan_id,#idvlan_priority").iCheck("enable");
        },
        uncheckedVlanEnable: function(ev) {
            $('#idvlan_enable').val(0);
            //this.model.set('vlan_enable', 0);//Quanta
            this.$("#idvlan_id,#idvlan_priority").attr("disabled", "disabled");
            this.$("#idvlan_id,#idvlan_priority").iCheck("disable");
        },
        saveConfirm: function() {
            if (!confirm(locale.saveConfirmLabel)) return;
            this.save.call(this);
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
            $('.tooltip').hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
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
                "ipv4_enable": "idipv4_enable",
                "ipv6_enable": "idipv6_enable",
                "ipv4_address": "idipv4_address",
                "ipv4_subnet": "idipv4_subnet",
                "ipv4_gateway": "idipv4_gateway",
                "ipv6_address": "idipv6_address",
                "ipv6_prefix": "idipv6_prefix",
                "vlan_id": "idvlan_id",
                "vlan_priority": "idvlan_priority"
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


            $("#save-icon").removeClass().addClass("ion-load-d");
            this.$(".alert-success,.alert-danger").addClass("hide");



            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }

            context.model.save({
                'interface_name': $('#idinterface_name').val(),
                'lan_enable': parseInt($('#idlan_enable').val(), 10),
                'mac_address': $('#idmac_address').val(),
                'ipv4_enable': parseInt($('#idipv4_enable').val(), 10),
                'ipv4_dhcp_enable': parseInt($('#idipv4_dhcp_enable').val(), 10),
                'ipv4_address': $('#idipv4_address').val(),
                'ipv4_subnet': $('#idipv4_subnet').val(),
                'ipv4_gateway': $('#idipv4_gateway').val(),
                'ipv6_enable': parseInt($('#idipv6_enable').val(), 10),
                'ipv6_dhcp_enable': parseInt($('#idipv6_dhcp_enable').val(), 10),
                'ipv6_index': parseInt($('#idipv6_index').val(), 10),
                'ipv6_address': $('#idipv6_address').val(),
                'ipv6_prefix': $('#idipv6_prefix').val(),
                'vlan_enable': parseInt($('#idvlan_enable').val(), 10),
                'vlan_id': $('#idvlan_id').val(),
                'vlan_priority': $('#idvlan_priority').val()
            }, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success_alert', context);
                },

                error: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error_alert', context);
                }
            });


        },

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
