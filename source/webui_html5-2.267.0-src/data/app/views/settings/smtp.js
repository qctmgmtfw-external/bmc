define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//smtp',
//localize
'i18n!strings/nls/smtp',
//template
'text!templates/settings/smtp.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, SmtpCollection, locale, SmtpTemplate) {

    var view = Backbone.View.extend({

        template: _.template(SmtpTemplate),

        initialize: function() {
            this.collection = SmtpCollection;
            this.collection.bind('add', this.add, this);
            this.collection.bind('reset', this.reset, this);
            this.collection.bind('remove', this.remove, this);
            this.collection.bind('change', this.change, this);
            this.collection.bind('change add reset remove', this.affect, this);

            this.collection.bind('validated:valid', this.validData, this);
            this.collection.bind('validated:invalid', this.invalidData, this);

        },

        events: {
            "change #idchannel_interface": "switchModel",
            "ifChecked #idprimary_smtp_enable": "checkedPrimarySmtpEnable",
            "ifUnchecked #idprimary_smtp_enable": "uncheckedPrimarySmtpEnable",
            "ifChecked #idprimary_smtp_authentication": "checkedPrimarySmtpAuthentication",
            "ifUnchecked #idprimary_smtp_authentication": "uncheckedPrimarySmtpAuthentication",
            "ifChecked #idprimary_ssltls_enable": "checkedPrimarySsltlsEnable",
            "ifUnchecked #idprimary_ssltls_enable": "uncheckedPrimarySsltlsEnable",
            "ifChecked #idprimary_starttls_enable": "checkedPrimaryStarttlsEnable",
            "ifUnchecked #idprimary_starttls_enable": "uncheckedPrimaryStarttlsEnable",
            "change #fileupload_cacert1": "chosenFileUploadCacert1",
            "change #fileupload_cert1": "chosenFileUploadCert1",
            "change #fileupload_key1": "chosenFileUploadKey1",
            "ifChecked #idsecondary_smtp_enable": "checkedSecondarySmtpEnable",
            "ifUnchecked #idsecondary_smtp_enable": "uncheckedSecondarySmtpEnable",
            "ifChecked #idsecondary_smtp_authentication": "checkedSecondarySmtpAuthentication",
            "ifUnchecked #idsecondary_smtp_authentication": "uncheckedSecondarySmtpAuthentication",
            "ifChecked #idsecondary_ssltls_enable": "checkedSecondarySsltlsEnable",
            "ifUnchecked #idsecondary_ssltls_enable": "uncheckedSecondarySsltlsEnable",
            "ifChecked #idsecondary_starttls_enable": "checkedSecondaryStarttlsEnable",
            "ifUnchecked #idsecondary_starttls_enable": "uncheckedSecondaryStarttlsEnable",
            "change #fileupload_cacert2": "chosenFileUploadCacert2",
            "change #fileupload_cert2": "chosenFileUploadCert2",
            "change #fileupload_key2": "chosenFileUploadKey2",
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
            this.$el.find('#idprimary_smtp_enable,#idprimary_smtp_authentication,#idprimary_ssltls_enable,#idprimary_starttls_enable,#idsecondary_smtp_enable,#idsecondary_smtp_authentication,#idsecondary_ssltls_enable,#idsecondary_starttls_enable').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            this.collection.fetch();
            this.collection.each(function(model) {
                this.add(model, this.collection);
            }, this);

        },

        add: function(model, collection, options) {
            $('#idchannel_interface').append($('<option></option>').attr('value', model.get('channel_interface')).text(model.get('channel_interface')));
            this.switchModel({
                target: $('#idchannel_interface').get(0)
            });
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
            $('#idemail_id').val(this.model.get('email_id'));
            if (this.model.get('primary_smtp_enable')) {
                $('#idprimary_smtp_enable').iCheck('check');
            } else {
                if (!$('#idprimary_smtp_enable').parent().hasClass('checked')) {
                    this.uncheckedPrimarySmtpEnable();
                }
                $('#idprimary_smtp_enable').iCheck('uncheck');
            }
            this.primary_data_reset.call(this);
            if (this.model.get('secondary_smtp_enable')) {
                $('#idsecondary_smtp_enable').iCheck('check');
            } else {
                if (!$('#idsecondary_smtp_enable').parent().hasClass('checked')) {
                    this.uncheckedSecondarySmtpEnable();
                }
                $('#idsecondary_smtp_enable').iCheck('uncheck');
            }
            this.secondary_data_reset.call(this);
            if ($('#fileupload_cacert1')[0].files.length) {
                $('#idupload_cacert1').val($('#fileupload_cacert1')[0].files[0].name);
            }
            else if (this.model.get('ca_info1').length) {
                $('#idupload_cacert1').val("Modified: "+this.model.get('ca_info1'));
            }
            if ($('#fileupload_cert1')[0].files.length) {
                $('#idupload_cert1').val($('#fileupload_cert1')[0].files[0].name);
            }
            else if (this.model.get('cert_info1').length) {
                $('#idupload_cert1').val("Modified: "+this.model.get('cert_info1'));
            }
            if ($('#fileupload_key1')[0].files.length) {
                $('#idupload_key1').val($('#fileupload_key1')[0].files[0].name);
            }
            else if (this.model.get('key_info1').length) {
                $('#idupload_key1').val("Modified: "+this.model.get('key_info1'));
            }
            if ($('#fileupload_cacert2')[0].files.length) {
                $('#idupload_cacert2').val($('#fileupload_cacert2')[0].files[0].name);
            }
            else if (this.model.get('ca_info2').length) {
                $('#idupload_cacert2').val("Modified: "+this.model.get('ca_info2'));
            }
            if ($('#fileupload_cert2')[0].files.length) {
                $('#idupload_cert2').val($('#fileupload_cert2')[0].files[0].name);
            }
            else if (this.model.get('cert_info2').length) {
                $('#idupload_cert2').val("Modified: "+this.model.get('cert_info2'));
            }
            if ($('#fileupload_key2')[0].files.length) {
                $('#idupload_key2').val($('#fileupload_key2')[0].files[0].name);
            }
            else if (this.model.get('key_info2').length) {
                $('#idupload_key2').val("Modified: "+this.model.get('key_info2'));
            }
        },
        primary_data_reset : function(ev){
            $('#idprimary_server_name').val(this.model.get('primary_server_name'));
            $('#idprimary_server_ip').val(this.model.get('primary_server_ip'));
            $('#idprimary_smtp_port').val(this.model.get('primary_smtp_port'));
            $('#idprimary_smtp_secure_port').val(this.model.get('primary_smtp_secure_port'));
            if (this.model.get('primary_smtp_authentication')) {  
                $('#idprimary_smtp_authentication').iCheck('check');
            } else {
                if (!$('#idprimary_smtp_authentication').parent().hasClass('checked')) {
                    this.uncheckedPrimarySmtpAuthentication();
                }
                $('#idprimary_smtp_authentication').iCheck('uncheck');
            }
            $('#idprimary_username').val(this.model.get('primary_username'));
            $('#idprimary_password').val(this.model.get('primary_password'));
            if (this.model.get('primary_ssltls_enable')) {
                $('#idprimary_ssltls_enable').iCheck('check');
            } else {
                if (!$('#idprimary_ssltls_enable').parent().hasClass('checked')) {
                    this.uncheckedPrimarySsltlsEnable();
                }
                $('#idprimary_ssltls_enable').iCheck('uncheck');
            }
            if (this.model.get('primary_starttls_enable')) {
                $('#idprimary_starttls_enable').iCheck('check');
            } else {
                if (!$('#idprimary_starttls_enable').parent().hasClass('checked')) {
                    this.uncheckedPrimaryStarttlsEnable();
                }
                $('#idprimary_starttls_enable').iCheck('uncheck');
            }
           if (!this.model.get('primary_smtp_enable')) {
            this.uncheckedPrimarySmtpEnable();
            }
        },
        secondary_data_reset : function(ev){
             $('#idsecondary_server_name').val(this.model.get('secondary_server_name'));
            $('#idsecondary_server_ip').val(this.model.get('secondary_server_ip'));
            $('#idsecondary_smtp_port').val(this.model.get('secondary_smtp_port'));
            $('#idsecondary_smtp_secure_port').val(this.model.get('secondary_smtp_secure_port'));
            if (this.model.get('secondary_smtp_authentication')) {
                $('#idsecondary_smtp_authentication').iCheck('check');
            } else {
                if (!$('#idsecondary_smtp_authentication').parent().hasClass('checked')) {
                    this.uncheckedSecondarySmtpAuthentication();
                }
                $('#idsecondary_smtp_authentication').iCheck('uncheck');
            }
            $('#idsecondary_username').val(this.model.get('secondary_username'));
            $('#idsecondary_password').val(this.model.get('secondary_password'));
            if (this.model.get('secondary_ssltls_enable')) {
                $('#idsecondary_ssltls_enable').iCheck('check');
            } else {
                if (!$('#idsecondary_ssltls_enable').parent().hasClass('checked')) {
                    this.uncheckedSecondarySsltlsEnable();
                }
                $('#idsecondary_ssltls_enable').iCheck('uncheck');
            }
            if (this.model.get('secondary_starttls_enable')) {
                $('#idsecondary_starttls_enable').iCheck('check');
            } else {
                if (!$('#idsecondary_starttls_enable').parent().hasClass('checked')) {
                    this.uncheckedSecondaryStarttlsEnable();
                }
                $('#idsecondary_starttls_enable').iCheck('uncheck');
            }
        },
        switchModel: function(ev) {
            this.model = this.collection.find(function(model) {
                return model.get('channel_interface') == $(ev.target).val();
            });
            this.modelSwitched.call(this);
        },
        checkedPrimarySmtpEnable: function(ev) {
            this.model.set('primary_smtp_enable', 1);
            $('#idprimary_smtp_enable').val(1);
            $("#idprimary_server_name,#idprimary_smtp_port,#idprimary_smtp_secure_port,#idprimary_smtp_authentication,#idprimary_server_ip,#idprimary_username,#idprimary_password,#idprimary_ssltls_enable,#idprimary_starttls_enable,#idupload_cacert1,#idupload_cert1,#idupload_key1").parents(".form-group").show();
  if ((!this.model.get('primary_starttls_enable')) && (!this.model.get('primary_ssltls_enable'))) {
                $("#idupload_cacert1,#idupload_cert1,#idupload_key1").parents(".form-group").hide();
            }


        },
        uncheckedPrimarySmtpEnable: function(ev) {
            this.primary_data_reset.call(this);
            this.model.set('primary_smtp_enable', 0);
            $('#idprimary_smtp_enable').val(0);
            $("#idprimary_server_name,#idprimary_smtp_port,#idprimary_smtp_secure_port,#idprimary_smtp_authentication,#idprimary_server_ip,#idprimary_username,#idprimary_password,#idprimary_ssltls_enable,#idprimary_starttls_enable,#idupload_cacert1,#idupload_cert1,#idupload_key1").parents(".form-group").hide();
        },
        checkedPrimarySmtpAuthentication: function(ev) {
            this.model.set('primary_smtp_authentication', 1);
            $("#idprimary_username,#idprimary_password").removeAttr("disabled");
            $("#idprimary_username,#idprimary_password").iCheck("enable");
        },
        uncheckedPrimarySmtpAuthentication: function(ev) {
            this.model.set('primary_smtp_authentication', 0);
            $("#idprimary_username,#idprimary_password").attr("disabled", "disabled");
            $("#idprimary_username,#idprimary_password").iCheck("disable");
        },
        checkedPrimarySsltlsEnable: function(ev) {
            this.model.set('primary_ssltls_enable', 1);
            $("#idupload_cacert1,#idupload_cert1,#idupload_key1,#idprimary_ssltls_enable").parents(".form-group").show();
            $("#idprimary_starttls_enable").parents(".form-group").hide();
            $("#idprimary_smtp_secure_port").removeAttr("disabled");
            $("#idprimary_smtp_secure_port").iCheck("enable");
            $("#idprimary_smtp_port").attr("disabled", "disabled");
            $("#idprimary_smtp_port").iCheck("disable");
        },
        uncheckedPrimarySsltlsEnable: function(ev) {
            this.model.set('primary_ssltls_enable', 0);
            if (this.model.get('primary_smtp_enable')) {
                $("#idprimary_starttls_enable").parents(".form-group").show();
            }
            if (!this.model.get('primary_starttls_enable')) {
                $("#idupload_cacert1,#idupload_cert1,#idupload_key1").parents(".form-group").hide();
            }
            $("#idprimary_smtp_port").removeAttr("disabled");
            $("#idprimary_smtp_port").iCheck("enable");
            $("#idprimary_smtp_secure_port").attr("disabled", "disabled");
            $("#idprimary_smtp_secure_port").iCheck("disable");
	    
        },
        checkedPrimaryStarttlsEnable: function(ev) {
            this.model.set('primary_starttls_enable', 1);
            $("#idupload_cacert1,#idupload_cert1,#idupload_key1,#idprimary_starttls_enable").parents(".form-group").show();
            $("#idprimary_ssltls_enable").parents(".form-group").hide();
            $("#idprimary_smtp_port").removeAttr("disabled");
            $("#idprimary_smtp_port").iCheck("enable");
            $("#idprimary_smtp_secure_port").attr("disabled", "disabled");
            $("#idprimary_smtp_secure_port").iCheck("disable");
        },
        uncheckedPrimaryStarttlsEnable: function(ev) {
            this.model.set('primary_starttls_enable', 0);
            if (this.model.get('primary_smtp_enable')) {
                $("#idprimary_ssltls_enable").parents(".form-group").show();
            }
            if (!this.model.get('primary_ssltls_enable')) {
                $("#idupload_cacert1,#idupload_cert1,#idupload_key1").parents(".form-group").hide();
            }
        },
        chosenFileUploadCacert1: function(ev) {
            var input = ev.target;
            $('#idupload_cacert1').val($('#fileupload_cacert1')[0].files[0].name);
        },
        chosenFileUploadCert1: function(ev) {
            var input = ev.target;
            $('#idupload_cert1').val($('#fileupload_cert1')[0].files[0].name);
        },
        chosenFileUploadKey1: function(ev) {
            var input = ev.target;
            $('#idupload_key1').val($('#fileupload_key1')[0].files[0].name);
        },
        checkedSecondarySmtpEnable: function(ev) {
            this.model.set('secondary_smtp_enable', 1);
            $('#idsecondary_smtp_enable').val(1);
            $("#idsecondary_server_name,#idsecondary_smtp_port,#idsecondary_smtp_secure_port,#idsecondary_smtp_authentication,#idsecondary_server_ip,#idsecondary_username,#idsecondary_password,#idsecondary_ssltls_enable,#idsecondary_starttls_enable,#idupload_cacert2,#idupload_cert2,#idupload_key2").parents(".form-group").show();

	if ((!this.model.get('secondary_starttls_enable')) && (!this.model.get('secondary_ssltls_enable'))) {
                $("#idupload_cacert2,#idupload_cert2,#idupload_key2").parents(".form-group").hide();
            }


        },
        uncheckedSecondarySmtpEnable: function(ev) {
            this.secondary_data_reset.call(this);
            this.model.set('secondary_smtp_enable', 0);
            $('#idsecondary_smtp_enable').val(0);
            $("#idsecondary_server_name,#idsecondary_smtp_port,#idsecondary_smtp_secure_port,#idsecondary_smtp_authentication,#idsecondary_server_ip,#idsecondary_username,#idsecondary_password,#idsecondary_ssltls_enable,#idsecondary_starttls_enable,#idupload_cacert2,#idupload_cert2,#idupload_key2").parents(".form-group").hide();
        },
        checkedSecondarySmtpAuthentication: function(ev) {
            this.model.set('secondary_smtp_authentication', 1);
            $("#idsecondary_username,#idsecondary_password").removeAttr("disabled");
            $("#idsecondary_username,#idsecondary_password").iCheck("enable");
        },
        uncheckedSecondarySmtpAuthentication: function(ev) {
            this.model.set('secondary_smtp_authentication', 0);
            $("#idsecondary_username,#idsecondary_password").attr("disabled", "disabled");
            $("#idsecondary_username,#idsecondary_password").iCheck("disable");
        },
        checkedSecondarySsltlsEnable: function(ev) {
            this.model.set('secondary_ssltls_enable', 1);
            $("#idupload_cacert2,#idupload_cert2,#idupload_key2,#idsecondary_ssltls_enable").parents(".form-group").show();
            $("#idsecondary_starttls_enable").parents(".form-group").hide();
            $("#idsecondary_smtp_secure_port").removeAttr("disabled");
            $("#idsecondary_smtp_secure_port").iCheck("enable");
            $("#idsecondary_smtp_port").attr("disabled", "disabled");
            $("#idsecondary_smtp_port").iCheck("disable");
        },
        uncheckedSecondarySsltlsEnable: function(ev) {
            this.model.set('secondary_ssltls_enable', 0);
            if (this.model.get('secondary_smtp_enable')) {
                $("#idsecondary_starttls_enable").parents(".form-group").show();
            }
            if (!this.model.get('secondary_starttls_enable')) {
                $("#idupload_cacert2,#idupload_cert2,#idupload_key2").parents(".form-group").hide();
            }
            $("#idsecondary_smtp_port").removeAttr("disabled");
            $("#idsecondary_smtp_port").iCheck("enable");
            $("#idsecondary_smtp_secure_port").attr("disabled", "disabled");
            $("#idsecondary_smtp_secure_port").iCheck("disable");
        },
        checkedSecondaryStarttlsEnable: function(ev) {
            this.model.set('secondary_starttls_enable', 1);
            $("#idupload_cacert2,#idupload_cert2,#idupload_key2,#idsecondary_starttls_enable").parents(".form-group").show();
            $("#idsecondary_ssltls_enable").parents(".form-group").hide();
            $("#idsecondary_smtp_port").removeAttr("disabled");
            $("#idsecondary_smtp_port").iCheck("enable");
            $("#idsecondary_smtp_secure_port").attr("disabled", "disabled");
            $("#idsecondary_smtp_secure_port").iCheck("disable");
        },
        uncheckedSecondaryStarttlsEnable: function(ev) {
            this.model.set('secondary_starttls_enable', 0);
            if (this.model.get('secondary_smtp_enable')) {
                $("#idsecondary_ssltls_enable").parents(".form-group").show();
            }
            if (!this.model.get('secondary_ssltls_enable')) {
                $("#idupload_cacert2,#idupload_cert2,#idupload_key2").parents(".form-group").hide();
            }

        },
        chosenFileUploadCacert2: function(ev) {
            var input = ev.target;
            $('#idupload_cacert2').val($('#fileupload_cacert2')[0].files[0].name);
        },
        chosenFileUploadCert2: function(ev) {
            var input = ev.target;
            $('#idupload_cert2').val($('#fileupload_cert2')[0].files[0].name);
        },
        chosenFileUploadKey2: function(ev) {
            var input = ev.target;
            $('#idupload_key2').val($('#fileupload_key2')[0].files[0].name);
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
                "email_id": "idemail_id",
                "primary_smtp_enable": "idprimary_smtp_enable",
                "primary_server_name": "idprimary_server_name",
                "primary_smtp_port": "idprimary_smtp_port",
                "primary_smtp_secure_port": "idprimary_smtp_secure_port",
                "primary_server_ip": "idprimary_server_ip",
                "primary_username": "idprimary_username",
                "primary_password": "idprimary_password",
                "secondary_server_name": "idsecondary_server_name",
                "secondary_smtp_port": "idsecondary_smtp_port",
                "secondary_smtp_secure_port": "idsecondary_smtp_secure_port",
                "secondary_server_ip": "idsecondary_server_ip",
                "secondary_username": "idsecondary_username",
                "secondary_password": "idsecondary_password"
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


            $("#save-icon").removeClass().addClass("ion-loading-c");
            this.$(".alert-success,.alert-danger").addClass("hide");

            // check for TLS cert/key files
            var pCA, pCert, pKey, sCA, sCert, sKey;

            pCA = this.$('#fileupload_cacert1')[0].files.length;
            pCert = this.$('#fileupload_cert1')[0].files.length;
            pKey = this.$('#fileupload_key1')[0].files.length;
            sCA = this.$('#fileupload_cacert2')[0].files.length;
            sCert = this.$('#fileupload_cert2')[0].files.length;
            sKey = this.$('#fileupload_key2')[0].files.length;


            // if proper file groups exist, add them to a form for uploading
            var doUpload = pCA || pCert || pKey || sCA || sCert || sKey;
            var data = new FormData();

            if(pCA) {
                data.append('upload_cacert1', $('#fileupload_cacert1')[0].files[0]);
            }
            if(pCert) {
                data.append('upload_cert1', $('#fileupload_cert1')[0].files[0]);
            }
            if(pKey) {
                data.append('upload_key1', $('#fileupload_key1')[0].files[0]);
            }
            if(sCA) {
                data.append('upload_cacert2', $('#fileupload_cacert2')[0].files[0]);
            }
            if(sCert) {
                data.append('upload_cert2', $('#fileupload_cert2')[0].files[0]);
            }
            if(sKey) {
                data.append('upload_key2', $('#fileupload_key2')[0].files[0]);
            }


            if (doUpload) {
                var that = this;


                var file_upload_url = "/api/settings/smtp/smtpupload";

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

                        context.model.save({
                            'email_id': $('#idemail_id').val(),
                            'primary_smtp_enable': $('#idprimary_smtp_enable').val(),
                            'primary_server_name': $('#idprimary_server_name').val(),
                            'primary_smtp_port': parseInt($('#idprimary_smtp_port').val(), 10),
                            'primary_smtp_secure_port': parseInt($('#idprimary_smtp_secure_port').val(), 10),
                            'primary_server_ip': $('#idprimary_server_ip').val(),
                            'primary_username': $('#idprimary_username').val(),
                            'primary_password': $('#idprimary_password').val(),
                            'secondary_smtp_enable': $('#idsecondary_smtp_enable').val(),
                            'secondary_server_name': $('#idsecondary_server_name').val(),
                            'secondary_smtp_port': parseInt($('#idsecondary_smtp_port').val(), 10),
                            'secondary_smtp_secure_port': parseInt($('#idsecondary_smtp_secure_port').val(), 10),
                            'secondary_server_ip': $('#idsecondary_server_ip').val(),
                            'secondary_username': $('#idsecondary_username').val(),
                            'secondary_password': $('#idsecondary_password').val()
                        }, {
                            success: function() {

                                context.$("#save-icon").removeClass().addClass("fa fa-save");
                                app.events.trigger('save_success', context);
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
                    'email_id': $('#idemail_id').val(),
                    'primary_server_name': $('#idprimary_server_name').val(),
                    'primary_smtp_port': parseInt($('#idprimary_smtp_port').val(), 10),
                    'primary_smtp_secure_port': parseInt($('#idprimary_smtp_secure_port').val(), 10),
                    'primary_server_ip': $('#idprimary_server_ip').val(),
                    'primary_username': $('#idprimary_username').val(),
                    'primary_password': $('#idprimary_password').val(),
                    'secondary_server_name': $('#idsecondary_server_name').val(),
                    'secondary_smtp_port': parseInt($('#idsecondary_smtp_port').val(), 10),
                    'secondary_smtp_secure_port': parseInt($('#idsecondary_smtp_secure_port').val(), 10),
                    'secondary_server_ip': $('#idsecondary_server_ip').val(),
                    'secondary_username': $('#idsecondary_username').val(),
                    'secondary_password': $('#idsecondary_password').val()
                }, {
                    success: function() {

                        context.$("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_success', context);
                    },

                    error: function() {
                        context.$("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_error', context);
                    }
                });
            }
        },

        saveValidateFiles: function() {


            $("#save-icon").removeClass().addClass("ion-loading-c");
            this.$(".alert-success,.alert-danger").addClass("hide");

            // check for TLS cert/key files
            var pCA, pCert, pKey, sCA, sCert, sKey;

            pCA = this.$('#fileupload_cacert1')[0].files.length;
            pCert = this.$('#fileupload_cert1')[0].files.length;
            pKey = this.$('#fileupload_key1')[0].files.length;
            sCA = this.$('#fileupload_cacert2')[0].files.length;
            sCert = this.$('#fileupload_cert2')[0].files.length;
            sKey = this.$('#fileupload_key2')[0].files.length;

            // files grouped by primary/secondary SMTP server
            var primaryAuthFiles = pCA && pCert && pKey;
            var secondaryAuthFiles = sCA && sCert && sKey;

            // validate settings based on existence of cert/key files
            // invalidate file inputs if they are empty and a corresponding authorization setting is enabled
            var invalid = false;
            var jel;
            if (this.model.get('primary_starttls_enable') || this.model.get('primary_ssltls_enable')) {
                if(!pCA) {
                    jel = this.$el.find('#idupload_cacert1');
                    jel.tooltip({
                        animation: false,
                        title: locale.must_upload_files,
                        trigger: 'manual',
                        placement: 'top'
                    });
                    jel.tooltip('show');
                    this.errorFields.push(jel);
                    invalid = true;
                    $("#save-icon").removeClass().addClass("fa fa-save");
                }
                if(!pCert) {
                    jel = this.$el.find('#idupload_cert1');
                    jel.tooltip({
                        animation: false,
                        title: locale.must_upload_files,
                        trigger: 'manual',
                        placement: 'top'
                    });
                    jel.tooltip('show');
                    this.errorFields.push(jel);
                    invalid = true;
                    $("#save-icon").removeClass().addClass("fa fa-save");
                }
                if(!pKey) {
                    jel = this.$el.find('#idupload_key1');
                    jel.tooltip({
                        animation: false,
                        title: locale.must_upload_files,
                        trigger: 'manual',
                        placement: 'top'
                    });
                    jel.tooltip('show');
                    this.errorFields.push(jel);
                    invalid = true;
                    $("#save-icon").removeClass().addClass("fa fa-save");
                }
            }
            if (this.model.get('secondary_starttls_enable') || this.model.get('secondary_ssltls_enable')) {
                if(!sCA) {
                    jel = this.$el.find('#idupload_cacert2');
                    jel.tooltip({
                        animation: false,
                        title: locale.must_upload_files,
                        trigger: 'manual',
                        placement: 'top'
                    });
                    jel.tooltip('show');
                    this.errorFields.push(jel);
                    invalid = true;
                    $("#save-icon").removeClass().addClass("fa fa-save");
                }
                if(!sCert) {
                    jel = this.$el.find('#idupload_cert2');
                    jel.tooltip({
                        animation: false,
                        title: locale.must_upload_files,
                        trigger: 'manual',
                        placement: 'top'
                    });
                    jel.tooltip('show');
                    this.errorFields.push(jel);
                    invalid = true;
                    $("#save-icon").removeClass().addClass("fa fa-save");
                }
                if(!sKey) {
                    jel = this.$el.find('#idupload_key2');
                    jel.tooltip({
                        animation: false,
                        title: locale.must_upload_files,
                        trigger: 'manual',
                        placement: 'top'
                    });
                    jel.tooltip('show');
                    this.errorFields.push(jel);
                    invalid = true;
                    $("#save-icon").removeClass().addClass("fa fa-save");
                }
            }
            // if any file inputs were invalidated, then abort the save
            if(invalid) {
                return;
            }

            // if proper file groups exist, add them to a form for uploading
            var data = new FormData();

            if (primaryAuthFiles) {
                data.append('upload_cacert1', $('#fileupload_cacert1')[0].files[0]);
                data.append('upload_cert1', $('#fileupload_cert1')[0].files[0]);
                data.append('upload_key1', $('#fileupload_key1')[0].files[0]);
            }
            if (secondaryAuthFiles) {
                data.append('upload_cacert2', $('#fileupload_cacert2')[0].files[0]);
                data.append('upload_cert2', $('#fileupload_cert2')[0].files[0]);
                data.append('upload_key2', $('#fileupload_key2')[0].files[0]);
            }

            if (primaryAuthFiles || secondaryAuthFiles) {
                var that = this;


                var file_upload_url = "/api/settings/smtp/smtpupload";

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

                        context.model.save({
                            'email_id': $('#idemail_id').val(),
                            'primary_server_name': $('#idprimary_server_name').val(),
                            'primary_smtp_port': parseInt($('#idprimary_smtp_port').val(), 10),
                            'primary_smtp_secure_port': parseInt($('#idprimary_smtp_secure_port').val(), 10),
                            'primary_server_ip': $('#idprimary_server_ip').val(),
                            'primary_username': $('#idprimary_username').val(),
                            'primary_password': $('#idprimary_password').val(),
                            'secondary_server_name': $('#idsecondary_server_name').val(),
                            'secondary_smtp_port': parseInt($('#idsecondary_smtp_port').val(), 10),
                            'secondary_smtp_secure_port': parseInt($('#idsecondary_smtp_secure_port').val(), 10),
                            'secondary_server_ip': $('#idsecondary_server_ip').val(),
                            'secondary_username': $('#idsecondary_username').val(),
                            'secondary_password': $('#idsecondary_password').val()
                        }, {
                            success: function() {

                                context.$("#save-icon").removeClass().addClass("fa fa-save");
                                app.events.trigger('save_success', context);
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
                    'email_id': $('#idemail_id').val(),
                    'primary_server_name': $('#idprimary_server_name').val(),
                    'primary_smtp_port': parseInt($('#idprimary_smtp_port').val(), 10),
                    'primary_smtp_secure_port': parseInt($('#idprimary_smtp_secure_port').val(), 10),
                    'primary_server_ip': $('#idprimary_server_ip').val(),
                    'primary_username': $('#idprimary_username').val(),
                    'primary_password': $('#idprimary_password').val(),
                    'secondary_server_name': $('#idsecondary_server_name').val(),
                    'secondary_smtp_port': parseInt($('#idsecondary_smtp_port').val(), 10),
                    'secondary_smtp_secure_port': parseInt($('#idsecondary_smtp_secure_port').val(), 10),
                    'secondary_server_ip': $('#idsecondary_server_ip').val(),
                    'secondary_username': $('#idsecondary_username').val(),
                    'secondary_password': $('#idsecondary_password').val()
                }, {
                    success: function() {

                        context.$("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_success', context);
                    },

                    error: function() {
                        context.$("#save-icon").removeClass().addClass("fa fa-save");
                        app.events.trigger('save_error', context);
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
