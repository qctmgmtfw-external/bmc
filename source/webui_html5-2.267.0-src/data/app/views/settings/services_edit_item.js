define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//services', 'collection//network_interfaces',
//localize
'i18n!strings/nls/services',
//template
'text!templates/settings/services_edit_item.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, ServicesCollection, NetworkInterfacesCollection, locale, ServicesEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(ServicesEditItemTemplate),

        initialize: function() {
            ServicesCollection.fetch({
                async: false
            });
            NetworkInterfacesCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = ServicesCollection.get(id);
                this.oldmodel = this.model
            } else {
                this.model = new ServicesCollection.model;
                this.model.url = ServicesCollection.url;
            };
            this.model.bind('change:service_id', this.updateServiceId, this);
            this.model.bind('change:service_name', this.updateServiceName, this);
            this.model.bind('change:state', this.updateState, this);
            this.model.bind('change:interface_name', this.updateInterfaceName, this);
            this.model.bind('change:non_secure_port', this.updateNonSecurePort, this);
            this.model.bind('change:secure_port', this.updateSecurePort, this);
            this.model.bind('change:time_out', this.updateTimeOut, this);
            this.model.bind('change:maximum_sessions', this.updateMaximumSessions, this);
            this.model.bind('change:active_session', this.updateActiveSession, this);
            this.network_interfaces = NetworkInterfacesCollection;
            this.network_interfaces.bind('add', this.addNetworkInterfaces, this);

            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);
        },

        events: {
            "ifChecked #idstate": "checkedState",
            "ifUnchecked #idstate": "uncheckedState",
            "click #idstate":"enableServiceCfg",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {
            //$('#idnon_secure_port').attr("maxlength","5");
        },

        afterRender: function() {
            this.$el.find('#idstate').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
            //this.network_interfaces.fetch();
            this.network_interfaces.each(function(model) {
                this.addNetworkInterfaces(model, this.network_interfaces);
            }, this);

            //this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });

            if(app.features.indexOf("SESSION_MANAGEMENT") == -1){
                $('#divactive_session').removeClass("hide");
            }
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

        checkedState: function(ev) {
            $('#idstate').val(1);
            this.model.set('state', 1);
            this.$("#idservice_interface,#idnon_secure_port,#idsecure_port,#idtime_out,#idmaximum_sessions").removeAttr("disabled");
            this.$("#idservice_interface,#idnon_secure_port,#idsecure_port,#idtime_out,#idmaximum_sessions").iCheck("enable");

            this.enableServiceCfg();
        },
        uncheckedState: function(ev) {
            $('#idstate').val(0);
            this.model.set('state', 0);

            var that = this;
            var non_secure_port = that.oldmodel.get('non_secure_port');
            var secure_port = that.oldmodel.get('secure_port');
            var time_out = that.oldmodel.get('time_out');

            $('#idnon_secure_port').val(non_secure_port & 0x7FFFFFFF);
            $('#idsecure_port').val(secure_port & 0x7FFFFFFF);
            $('#idtime_out').val(time_out & 0x7FFFFFFF);

            this.$("#idservice_interface,#idnon_secure_port,#idsecure_port,#idtime_out,#idmaximum_sessions").attr("disabled", "disabled");
            this.$("#idservice_interface,#idnon_secure_port,#idsecure_port,#idtime_out,#idmaximum_sessions").iCheck("disable");
            this.enableServiceCfg();
        },
        enableServiceCfg:function(){
            //console.log("inside enableServiceCfg ");
            var bopt;
            bopt = !$('#idstate').parent().hasClass('checked');
            if(bopt){
               // console.log("inside if enableServiceCfg");
                $('#idnon_secure_port').val(this.model.get("non_secure_port") & 0x7FFFFFFF);
                $('#idsecure_port').val(this.model.get("secure_port") & 0x7FFFFFFF);
                $('#idtime_out').val(this.model.get("time_out") & 0x7FFFFFFF);
            }
            this.doSinglePortCfg();
        },
        doSinglePortCfg:function(){
            var single_port=this.model.get('singleport_status');
            var service_id=this.model.get('service_id');
           // console.log(single_port);
            if(single_port){
                if (this.getbitsval(service_id, 1, 1) ||
                    this.getbitsval(service_id, 2, 2) ||
                    this.getbitsval(service_id, 3, 3) ||
                    this.getbitsval(service_id, 4, 4)) {
                   // console.log("inside single port disabled");
                    $("#idservice_interface").attr("disabled", "disabled");
                }
            }
        },
        getbitsval:function(orig,startbit,endbit)
        {
            var temp;
            temp = this.getbits(orig,startbit,endbit);
            temp = temp >> endbit;
            return temp;
        },
        getbits:function(orig,startbit,endbit){
            var temp = orig;
            var mask =0x00;

            for(var i=startbit;i>=endbit;i--)
            {
                mask = mask | (1 << i);
            }
            return (temp & mask);
        },
        addNetworkInterfaces: function(model, collection) {
            var o = model.get('interface_name');
            $('#idservice_interface').append($('<option></option>').attr('value', o).text(o));

            if (collection.length > 1) {
                if($("#idservice_interface option[value='both']").length==0){
                    $('#idservice_interface').append(new Option("both","both"));
                }
            }
        },
        saveConfirm: function() {
            var notecontent = '';
            if ($('#idservice_name').length > 0) {
                var textval = $('#idservice_name').text();
                if (textval != '' && textval != undefined && textval != null) {
                    if (textval == 'web') {
                        notecontent = 'NOTE: Login session will be logged out.';
                    } else if (textval == 'ssh') {
                        notecontent = 'NOTE: Configured timeout value is applied to Telnet service also.';
                    } else if (textval == 'telnet') {
                        notecontent = 'NOTE:Configured timeout value is applied to SSH service also.';
                    }
                }
            }
            if (!confirm(locale.saveConfirmLabel + notecontent)) return;
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
            $(".tooltip").hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },
        updateServiceId: function(model, value) {},
        updateServiceName: function(model, value) {
            $('#idservice_name').html(value.toUpperCase());
        },
        updateState: function(model, value) {
            if (value) {
                $('#idstate').iCheck('check');
                this.checkedState();
            } else {
                if (!$('#idstate').parent().hasClass('checked')) {
                    this.uncheckedState();
                }
                $('#idstate').iCheck('uncheck');
            }
        },
        updateInterfaceName: function(model, value) {

           // console.log("inside updateInterfaceName"+value);

            if(value=="both"){
	    	if($("#idservice_interface option[value='both']").length==0){
                    $('#idservice_interface').append(new Option("both","both"));
                }
               $('#idservice_interface option[value="both"]').prop('selected', true);
            }
            else{
                $('#idservice_interface').val(value);
            }
            if (value == 'NA' || value === null || value == 'N/A') {
                $('#idservice_interface').parents('.form-group').hide();
            } else {
                $('#idservice_interface').parents('.form-group').show();
            }
        },
        updateNonSecurePort: function(model, value) {
            var port_val = (value == 0xFFFFFFFF) ? 'N/A' : value & 0x7FFFFFFF;
            $('#idnon_secure_port').val(port_val);
            if (value == -1 || value === null || value == 'N/A' || value == 2147483647) {
                $('#idnon_secure_port').parents('.form-group').hide();
            } else {
                $('#idnon_secure_port').parents('.form-group').show();
            }
        },
        updateSecurePort: function(model, value) {
            var port_val = (value == 0xFFFFFFFF) ? 'N/A' : value & 0x7FFFFFFF;
            $('#idsecure_port').val(port_val);
            if (value == -1 || value === null || value == 'N/A' || value == 2147483647) {
                $('#idsecure_port').parents('.form-group').hide();
            } else {
                $('#idsecure_port').parents('.form-group').show();
            }
            if ($('#idservice_name').length > 0) {
                var textval = $('#idservice_name').text();
                if (textval == 'cd-media' || textval == 'fd-media' || textval == 'hd-media' || textval == 'kvm') {
                    var interfaceval = $('#idservice_interface').val();
		    var single_port=this.model.get('singleport_status');
                    if(interfaceval != null  && (interfaceval.indexOf('bond') > -1)) {
                        if ($('#idservice_interface').length > 0) {
                            $('#idservice_interface').attr('disabled', 'disabled');
                        }
                    }
		    if(single_port!=0){
                        if ($('#idnon_secure_port').length > 0) {
                            $('#idnon_secure_port').attr('readonly', 'readonly');
                        }
                        if ($('#idsecure_port').length > 0) {
                            $('#idsecure_port').attr('readonly', 'readonly');
                        }
                    }
                }
            }
        },
        updateTimeOut: function(model, value) {
            $('#idtime_out').val(value);
            if (value == -1 || value === null || value == 'N/A' || value == 2147483647) {
                $('#idtime_out').parents('.form-group').hide();
            } else {
                $('#idtime_out').parents('.form-group').show();
            }
        },
        updateMaximumSessions: function(model, value) {
            var port_val = (value == 0xFF || value == 255) ? 'N/A' : value & 0x7F;
            $('#idmaximum_sessions').html(port_val);
            if (port_val == 'N/A') {
                $('#idmaximum_sessions').parents('.form-group').hide();
            } else {
                $('#idmaximum_sessions').parents('.form-group').show();
            }
        },
        updateActiveSession: function(model, value) {
            var port_val = (value == 0xFF || value == 255) ? 'N/A' : value & 0x7F;
            $('#idactive_session').html(port_val);
            if (port_val == 'N/A') {
                $('#idactive_session').parents('.form-group').hide();
            } else {
                $('#idactive_session').parents('.form-group').show();
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
           // console.log('invalid', errors);
            var elMap = {
                "time_out": "idtime_out",
                "non_secure_port": "idnon_secure_port",
                "secure_port": "idsecure_port"
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
            var servicename = $('#idservice_name').text();
            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }

            var active=$('#idstate').parent().hasClass('checked')? 1 : 0;

            context.model.save({
                'state': active,
                'interface_name': $('#idservice_interface').val(),
                'non_secure_port': $('#idnon_secure_port').val(),
                'secure_port': $('#idsecure_port').val(),
                'time_out': $('#idtime_out').val(),
                'maximum_sessions': $('#idmaximum_sessions').val(),
                'active_session': $('#idactive_session').val()
            }, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', context);
                    if(servicename == "web"){
                        $("body").addClass("disable_a_href");
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload();
                    }else{
                        var back_url = location.hash.split('/').slice(0, - 2);
                        //back_url.pop();
                        app.router.navigate(back_url.join('/'), {
                            trigger: true
                        });
                    }

                },
                error: function(xhr, response, options) {
                    var errorcode = '';
                    var errormsg = '';

                    if(servicename == "web" && response.status == 401 ){
                        $("body").addClass("disable_a_href");
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload();
                    }else{
                        context.$("#save-icon").removeClass().addClass("fa fa-save");

                        app.events.trigger('save_error', context);
                    }

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
