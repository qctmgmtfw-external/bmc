define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection/notifications',
'models/notification',
'collection//users',
//localize
'i18n!strings/nls/users',
//template
'text!templates/settings/users_edit_item.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, NotificationCollection, NotificationModel, UsersCollection, locale, UsersEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(UsersEditItemTemplate),
        kvm_enable : 0,
        vm_enable : 0,
        initialize: function() {

            if(sessionStorage.privilege_id < CONSTANTS["OPERATOR"]){
                NotificationCollection.add(new NotificationModel({
                    id: 8000+(new Date()).getTime(),
                    message_group: "failure",
                    message: "Insufficient Privilege \n [code:8000]",
                    severity: "danger"
                }));
                app.hidePage();
            }else{
                UsersCollection.fetch({
                    async: false
                });
            }
            var id = location.hash.split('/').pop();
            this.userid = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = UsersCollection.get(id);
            } else {
                this.model = new UsersCollection.model;
                this.model.url = UsersCollection.url;
            };
            this.UserOperation = 0;
            this.addStatus = false;
            this.model.bind('change:name', this.updateName, this);
            this.model.bind('change:password', this.updatePassword, this);
            this.model.bind('change:confirm_password', this.updateConfirmPassword, this);
            this.model.bind('change:password_size', this.updatePasswordSize, this);
            this.model.bind('change:access', this.updateAccess, this);
            this.model.bind('change:kvm', this.updateKvm, this);
            this.model.bind('change:vmedia', this.updateVmedia, this);
            this.model.bind('change:snmp', this.updateSnmp, this);
            this.model.bind('change:network_privilege', this.updateNetworkPrivilege, this);
            this.model.bind('change:privilege_limit_serial', this.updatePrivilegeLimitSerial, this);
            this.model.bind('change:fixed_user_count', this.updateFixedUserCount, this);
            this.model.bind('change:snmp_access', this.updateSnmpAccess, this);
            this.model.bind('change:snmp_authentication_protocol', this.updateSnmpAuthenticationProtocol, this);
            this.model.bind('change:snmp_privacy_protocol', this.updateSnmpPrivacyProtocol, this);
            this.model.bind('change:email_id', this.updateEmailId, this);
            this.model.bind('change:email_format', this.updateEmailFormat, this);
            this.model.bind('change:ssh_status', this.updateSshStatus, this);
            this.model.bind('change:ssh_key', this.updateSshKey, this);

            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);
            kvm_enable = 0;
            vm_enable = 0;
        },

        events: {
        	"ifChecked #idchangepassword": "checkedChangePassword",
            "ifUnchecked #idchangepassword": "uncheckedChangePassword",
            "ifChecked #idaccess": "checkedAccess",
            "ifUnchecked #idaccess": "uncheckedAccess",
            "ifChecked #idkvm": "checkedKvm",
            "ifUnchecked #idkvm": "uncheckedKvm",
            "ifChecked #idvmedia": "checkedVmedia",
            "ifUnchecked #idvmedia": "uncheckedVmedia",
            "ifChecked #idsnmp": "checkedSnmp",
            "ifUnchecked #idsnmp": "uncheckedSnmp",
            "change #fileupload_ssh_key": "chosenFileUploadSshKey",
            "click #delete": "deleteConfirm",
            "click #save": "save",
            "click .help-link": "toggleHelp",
            //Quanta---
            //"blur #idpassword": "preValidPassword"
            //---Quanta
        },

        beforeRender: function() {

        },

        afterRender: function() {
            if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();
                app.disableAllinput();
		app.disableAlldropdown();
            }
            this.$el.find('#idchangepassword,#idaccess,#idkvm,#idvmedia,#idsnmp').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
            var OemProp_Priv_support=this.model.get('OEMProprietary_level_Privilege');
            if(OemProp_Priv_support)
            	{
            		$('#idnetwork_privilege').append("<option value='oem'>OEM Proprietary</option>");
            		$('#idprivilege_limit_serial').append("<option value='oem'>OEM Proprietary</option>");
            	}
            $("#delete").hide();
            $("#idname").attr("readonly", true);
            $("#idaccess").attr("disabled", true);
            if(app.isAdmin() && sessionStorage.id != this.userid && this.userid > CONSTANTS["FIXED_USER_COUNT"]){
                $("#delete").show();
                $("#idname").removeAttr("readonly");
            }
            if(this.userid > CONSTANTS["FIXED_USER_COUNT"]-1 && sessionStorage.id != this.userid){
                $("#idaccess").removeAttr("disabled");
            }
            //this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },
        checkedChangePassword: function(ev) {
            $('#idchangepassword').val(1);
            this.model.set('changepassword', 1);
            $('#idchangepassword').iCheck('check');
            this.$("#idpassword_size,#idpassword,#idconfirm_password").removeAttr("disabled");
        },
        uncheckedChangePassword: function(ev) {
            $('#idchangepassword').val(0);
            this.model.set('changepassword', 0);
            $('#idchangepassword').iCheck('uncheck');
            this.$("#idpassword").val("");//Quanta
            this.$("#idconfirm_password").val("");//Quanta
            this.$("#idpassword_size,#idpassword,#idconfirm_password").attr("disabled", "disabled");
        },

        checkedAccess: function(ev) {
            this.model.set('access', 1);
        },
        uncheckedAccess: function(ev) {
            this.model.set('access', 0);
        },
        compareName: function(){
            var session_name = window.sessionStorage.getItem('username');
            var doc_name = document.getElementById("idname").value;
            return (session_name==doc_name);
        },
        checkedKvm: function(ev) {
            this.model.set('kvm', 1);
            if(this.compareName())
            {
                kvm_enable=1;
            }
        },
        uncheckedKvm: function(ev) {
            this.model.set('kvm', 0);
            if(this.compareName())
            {
                kvm_enable=0;
            }
        },
        checkedVmedia: function(ev) {
            this.model.set('vmedia', 1);
            if(this.compareName())
            {
                vm_enable=1;
            }
        },
        uncheckedVmedia: function(ev) {
            this.model.set('vmedia', 0);
            if(this.compareName())
            {
                vm_enable=0;
            }
        },
        checkedSnmp: function(ev) {
            this.model.set('snmp', 1);
        },
        uncheckedSnmp: function(ev) {
            this.model.set('snmp', 0);
        },
        chosenFileUploadSshKey: function(ev) {
            var input = ev.target;
            $('#idupload_ssh_key').val($('#fileupload_ssh_key')[0].files[0].name);
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
        updateName: function(model, value) {
            this.UserOperation = (value == "")? 0 : 1;
            $('#idname').val(value);
            if(value != "" && !(this.addStatus)){
                this.UserOperation = 1;
                $("#delete").show();
                $("#idchangepassword").parents('.form-group').show();
                $('#idchangepassword').iCheck('uncheck');
            	this.$("#idpassword_size,#idpassword,#idconfirm_password").attr("disabled", "disabled");
            }
            else{
                this.UserOperation = 0;
                this.addStatus = true;
                $("#delete").hide();
                $("#idchangepassword").parents(".form-group").hide();
                this.$("#idpassword_size,#idpassword,#idconfirm_password").removeAttr("disabled");
                $('#idchangepassword').val(1);
                this.model.set('changepassword', 1);
            }
        },
        updatePassword: function(model, value) {
            $('#idpassword').val(value);
        },
        updateConfirmPassword: function(model, value) {
            $('#idconfirm_password').val(value);
        },
        updatePasswordSize: function(model, value) {
            $('#idpassword_size').val(value);
        },
        updateAccess: function(model, value) {
            if (value) {
                $('#idaccess').iCheck('check');
            } else {
                if (!$('#idaccess').parent().hasClass('checked')) {
                    this.uncheckedAccess();
                }
                $('#idaccess').iCheck('uncheck');
            }
        },
        updateKvm: function(model, value) {
            if (value) {
                $('#idkvm').iCheck('check');
            } else {
                if (!$('#idkvm').parent().hasClass('checked')) {
                    this.uncheckedKvm();
                }
                $('#idkvm').iCheck('uncheck');
            }
        },
        updateVmedia: function(model, value) {
            if (value) {
                $('#idvmedia').iCheck('check');
            } else {
                if (!$('#idvmedia').parent().hasClass('checked')) {
                    this.uncheckedVmedia();
                }
                $('#idvmedia').iCheck('uncheck');
            }
        },
        updateSnmp: function(model, value) {
        	//Quanta---
        	$('#snmp_label').tooltip('destroy');
        	//---Quanta
            if (value) {
            	//Quanta---
            	$('#snmp_label').tooltip({
                    animation: false,
                    title: "If enable SNMP, password should be at least 8 character length.",
                    trigger: 'manual',
                    placement: 'right'
                });
            	$('#snmp_label').tooltip('show');
            	//---Quanta
                $('#idsnmp').iCheck('check');
            } else {
                if (!$('#idsnmp').parent().hasClass('checked')) {
                    this.uncheckedSnmp();
                }
                $('#idsnmp').iCheck('uncheck');
            }
        },
        updateNetworkPrivilege: function(model, value) {
            $('#idnetwork_privilege').val(value);
        },
        updatePrivilegeLimitSerial: function(model, value) {
            $('#idprivilege_limit_serial').val(value);
        },
        updateFixedUserCount: function(model, value) {},
        updateSnmpAccess: function(model, value) {
            $('#idsnmp_access').val(value);
        },
        updateSnmpAuthenticationProtocol: function(model, value) {
            $('#idsnmp_authentication_protocol').val(value);
        },
        updateSnmpPrivacyProtocol: function(model, value) {
            $('#idsnmp_privacy_protocol').val(value);
        },
        updateEmailId: function(model, value) {
            $('#idemail_id').val(value);
        },
        updateEmailFormat: function(model, value) {
            $('#idemail_format').val(value);
        },
        updateSshStatus: function(model, value) {},
        updateSshKey: function(model, value) {
            $('#idexisting_ssh_key').val(value);
        },
        deleteConfirm: function() {
            if(sessionStorage.privilege_id > CONSTANTS["OPERATOR"] && this.userid > CONSTANTS["FIXED_USER_COUNT"] && this.userid != sessionStorage.id){
                if (!confirm(locale.delete_confirmation_message)) return;
                this.delete.call(this);
            }
            else{
                alert(locale.non_privilege_user_delete)
            }
        },
        delete: function() {
            this.model.destroy({
                success: function() {
                    var back_url = location.hash.split('/').slice(0, - 2);
                    //back_url.pop(); //back_url.pop();
                    app.router.navigate(back_url.join('/'), {
                        trigger: true
                    });
                }
            });
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
                "name": "idname",
                "password": "idpassword",
                "confirm_password": "idconfirm_password",
                "email_id": "idemail_id"
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
// Quanta---
        /*preValidPassword: function(){
            var $el = $("#idpassword");
            if($el.val().match(/^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])(?=.*[!@#\$%\^\&*\)\(+=._-])[0-9a-zA-Z!@#\$%\^\&*\)\(+=._-]{8,}$/)){
        	    //PassWord is valid
        	    console.log("Pass Word is valid:" + $el.val());
        	    $el.tooltip('hide');
        	    $el.data("invalid", false);
           } else {
        	    //Password is invalid
        	    console.log("Pass Word is invalid:" + $el.val());
        	    $el.tooltip({
      			    animation: false,
    			    title: "Invalid password, at least 8 characters which must include upper case/lower case/special character/numerical digits",
    			    trigger: 'manual',
    			    placement: 'top'});
        	    $el.tooltip('show');
        	    $el.data("invalid", true);
        	    //When click, hide the tooltip
        	    $el.click(function(){
        		    $el.tooltip('hide');
        	    });
           }
           return true;
        }, */
// ---Quanta
        save: function() {
// Quanta---
        	/*if($("#idpassword").trigger('blur').data("invalid"))
        		return false;*/
// ---Quanta
            $("#save-icon").removeClass().addClass("ion-load-d");
            this.$(".alert-success,.alert-danger").addClass("hide");


            if (this.$('#fileupload_ssh_key')[0].files.length) {
                var that = this;

                var data = new FormData();
                data.append('upload_ssh_key', this.$('#fileupload_ssh_key')[0].files[0]);

                var file_upload_url = "/api/settings/user/ssh-key-upload/:id".replace(':id', that.model.get('id'));

                $.ajax({
                    url: file_upload_url,
                    type: "POST",
                    contentType: false,
                    processData: false,
                    dataType: "json",
                    cache: false,
                    data: data,
                    success: function(data, status, xhr) {
                        var context = this;
                        try {
                            context = that || this;
                        } catch (e) {
                            context = this;
                        }
                            window.sessionStorage.setItem('kvm_access', kvm_enable);
                            window.sessionStorage.setItem('vmedia_access', vm_enable);
                        context.model.save({
                            'name': $('#idname').val(),
                            'UserOperation': context.UserOperation,
                            'changepassword': $('#idchangepassword').val(),
                            'password': $('#idpassword').val(),
                            'confirm_password': $('#idconfirm_password').val(),
                            'password_size': $('#idpassword_size').val(),
                            'network_privilege': $('#idnetwork_privilege').val(),
                            'snmp_access': $('#idsnmp_access').val(),
                            'snmp_authentication_protocol': $('#idsnmp_authentication_protocol').val(),
                            'snmp_privacy_protocol': $('#idsnmp_privacy_protocol').val(),
                            'privilege_limit_serial': $('#idprivilege_limit_serial').val(),
                            'email_id': $('#idemail_id').val(),
                            'email_format': $('#idemail_format').val(),
                            'ssh_key': $('#idexisting_ssh_key').val()
                        }, {
                            success: function() {

                                context.$("#save-icon").removeClass().addClass("fa fa-save");
                                app.events.trigger('save_success', context);
                                var back_url = location.hash.split('/').slice(0, - 2);
                                //back_url.pop();
                                app.router.navigate(back_url.join('/'), {
                                    trigger: true
                                });
                            },

                            error: function() {
                                context.$("#save-icon").removeClass().addClass("fa fa-save");
                                app.events.trigger('save_error', context);
                            }
                        });
                    },
                    error: function() {
                        $("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_error', context);
                    }
                });
            } else {
                var context = this;
                try {
                    context = that || this;
                } catch (e) {
                    context = this;
                }
                context.model.save({
                    'name': $('#idname').val(),
                    'UserOperation': context.UserOperation,
                    'password': $('#idpassword').val(),
                    'confirm_password': $('#idconfirm_password').val(),
                    'password_size': $('#idpassword_size').val(),
                    'network_privilege': $('#idnetwork_privilege').val(),
                    'snmp_access': $('#idsnmp_access').val(),
                    'snmp_authentication_protocol': $('#idsnmp_authentication_protocol').val(),
                    'snmp_privacy_protocol': $('#idsnmp_privacy_protocol').val(),
                    'privilege_limit_serial': $('#idprivilege_limit_serial').val(),
                    'email_id': $('#idemail_id').val(),
                    'email_format': $('#idemail_format').val(),
                    'ssh_key': $('#idexisting_ssh_key').val()
                }, {
                    success: function() {

                        context.$("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_success', context);
                        var back_url = location.hash.split('/').slice(0, - 2);
                        //back_url.pop();
                        app.router.navigate(back_url.join('/'), {
                            trigger: true
                        });
                        if(window.sessionStorage.getItem('username')==$('#idname').val())
                        {
                            window.sessionStorage.setItem('kvm_access', kvm_enable);
                            window.sessionStorage.setItem('vmedia_access', vm_enable);
                        }
                    },

                    error: function() {
                        context.$("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_error', context);;
                        context.UserOperation = context.addStatus ? 0 : 1;
                    }
                });
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
