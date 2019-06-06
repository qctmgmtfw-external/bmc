define(['jquery', 'underscore', 'backbone', 'app',
//data
'models/media/general',
//localize
'i18n!strings/nls/general',
//template
'text!templates/settings/media/general.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, GeneralModel, locale, GeneralTemplate) {

    var view = Backbone.View.extend({

        template: _.template(GeneralTemplate),

        initialize: function() {
            this.model = GeneralModel;
            this.model.bind('change:local_media_support', this.updateLocalMediaSupport, this);
            this.model.bind('change:remote_media_support', this.updateRemoteMediaSupport, this);
            this.model.bind('change:cd_remote_server_address', this.updateCdRemoteServerAddress, this);
            this.model.bind('change:cd_remote_source_path', this.updateCdRemoteSourcePath, this);
            this.model.bind('change:cd_remote_share_type', this.updateCdRemoteShareType, this);
            this.model.bind('change:cd_remote_domain_name', this.updateCdRemoteDomainName, this);
            this.model.bind('change:cd_remote_user_name', this.updateCdRemoteUserName, this);
            this.model.bind('change:cd_remote_password', this.updateCdRemotePassword, this);
            this.model.bind('change:fd_remote_server_address', this.updateFdRemoteServerAddress, this);
            this.model.bind('change:fd_remote_source_path', this.updateFdRemoteSourcePath, this);
            this.model.bind('change:fd_remote_share_type', this.updateFdRemoteShareType, this);
            this.model.bind('change:fd_remote_domain_name', this.updateFdRemoteDomainName, this);
            this.model.bind('change:fd_remote_user_name', this.updateFdRemoteUserName, this);
            this.model.bind('change:fd_remote_password', this.updateFdRemotePassword, this);
            this.model.bind('change:hd_remote_server_address', this.updateHdRemoteServerAddress, this);
            this.model.bind('change:hd_remote_source_path', this.updateHdRemoteSourcePath, this);
            this.model.bind('change:hd_remote_share_type', this.updateHdRemoteShareType, this);
            this.model.bind('change:hd_remote_domain_name', this.updateHdRemoteDomainName, this);
            this.model.bind('change:hd_remote_user_name', this.updateHdRemoteUserName, this);
            this.model.bind('change:hd_remote_password', this.updateHdRemotePassword, this);
            this.model.bind('change:mount_cd', this.updateMountCd, this);
            this.model.bind('change:mount_fd', this.updateMountFd, this);
            this.model.bind('change:mount_hd', this.updateMountHd, this);
            this.model.bind('change:same_settings', this.updateSameSettings, this);

            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);
        },

        events: {
            "ifChecked #idlocal_media_support": "checkedLocalMediaSupport",
            "ifUnchecked #idlocal_media_support": "uncheckedLocalMediaSupport",
            "ifChecked #idremote_media_support": "checkedRemoteMediaSupport",
            "ifUnchecked #idremote_media_support": "uncheckedRemoteMediaSupport",
            "ifChecked #idmount_cd": "checkedMountCd",
            "ifUnchecked #idmount_cd": "uncheckedMountCd",
            "ifChecked input[name='cd_remote_share_type']": "checkedCdRemoteShareType",
            "ifUnchecked input[name='cd_remote_share_type']": "uncheckedCdRemoteShareType",
            "ifChecked #idsame_settings": "checkedSameSettings",
            "ifUnchecked #idsame_settings": "uncheckedSameSettings",
            "ifChecked #idmount_fd": "checkedMountFd",
            "ifUnchecked #idmount_fd": "uncheckedMountFd",
            "ifChecked input[name='fd_remote_share_type']": "checkedFdRemoteShareType",
            "ifUnchecked input[name='fd_remote_share_type']": "uncheckedFdRemoteShareType",
            "ifChecked #idmount_hd": "checkedMountHd",
            "ifUnchecked #idmount_hd": "uncheckedMountHd",
            "ifChecked input[name='hd_remote_share_type']": "checkedHdRemoteShareType",
            "ifUnchecked input[name='hd_remote_share_type']": "uncheckedHdRemoteShareType",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.$el.find('#idlocal_media_support,#idremote_media_support,#idmount_cd,#idsame_settings,#idmount_fd,#idmount_hd,input[name="cd_remote_share_type"],input[name="fd_remote_share_type"],input[name="hd_remote_share_type"]').iCheck({
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

        checkedLocalMediaSupport: function(ev) {
            $('#idlocal_media_support').val(1);
            this.model.set('local_media_support', 1);
        },
        uncheckedLocalMediaSupport: function(ev) {
            $('#idlocal_media_support').val(0);
            this.model.set('local_media_support', 0);
        },
        checkedRemoteMediaSupport: function(ev) {
            $('#idremote_media_support').val(1);
            this.model.set('remote_media_support', 1);
            //this.$("#idsame_settings,#idcd_remote_server_address,#idcd_remote_source_path,#idcd_remote_share_type,#idmount_cd,#idmount_fd,#idmount_hd").parents(".form-group").show();
            this.$("#rmedia_parts").show();
        },
        uncheckedRemoteMediaSupport: function(ev) {
            $('#idremote_media_support').val(0);
            this.model.set('remote_media_support', 0);
            //this.$("#idsame_settings,#idcd_remote_server_address,#idcd_remote_source_path,#idcd_remote_share_type,#idcd_remote_domain_name,#idcd_remote_user_name,#idcd_remote_password,#idfd_remote_server_address,#idfd_remote_source_path,#idfd_remote_share_type,#idfd_remote_domain_name,#idfd_remote_user_name,#idfd_remote_password,#idhd_remote_server_address,#idhd_remote_source_path,#idhd_remote_share_type,#idhd_remote_domain_name,#idhd_remote_user_name,#idhd_remote_password,#idmount_cd,#idmount_fd,#idmount_hd").parents(".form-group").hide();
            this.$("#rmedia_parts").hide();
        },
        checkedMountCd: function(ev) {
            $('#idmount_cd').val(1);
            this.model.set('mount_cd', 1);
            //this.$("#idcd_remote_server_address,#idcd_remote_source_path,#idcd_remote_share_type").parents(".form-group").show();
            this.$("#rmedia_cd_details").show();
            this.$("#idsame_settings").removeAttr("disabled");
            this.$("#idsame_settings").iCheck("enable");
        },
        uncheckedMountCd: function(ev) {
            $('#idmount_cd').val(0);
            this.model.set('mount_cd', 0);
            //this.$("#idcd_remote_server_address,#idcd_remote_source_path,#idcd_remote_share_type,#idcd_remote_domain_name,#idcd_remote_user_name,#idcd_remote_password").parents(".form-group").hide();
            this.$("#rmedia_cd_details").hide();
            this.$("#idsame_settings").attr("disabled", "disabled");
            this.$("#idsame_settings").iCheck("disable");
        },
        checkedCdRemoteShareType: function(ev) {
            console.log('checked radio', $(ev.target).filter(':checked').val());
            this.model.set('cd_remote_share_type', $(ev.target).filter(':checked').val());
            if ($(ev.target).filter(':checked').val() == 'nfs') {
                this.$("#idcd_remote_domain_name,#idcd_remote_user_name,#idcd_remote_password").parents(".form-group").hide();
            }
            if ($(ev.target).filter(':checked').val() == 'cifs') {
                this.$("#idcd_remote_domain_name,#idcd_remote_user_name,#idcd_remote_password").parents(".form-group").show();
            }

        },
        uncheckedCdRemoteShareType: function() {},
        checkedSameSettings: function(ev) {
            $('#idsame_settings').val(1);
            this.model.set('same_settings', 1);
            /*this.$("#idfd_remote_server_address,#idfd_remote_source_path,#idfd_remote_share_type,#idfd_remote_domain_name,#idfd_remote_user_name,#idfd_remote_password,#idhd_remote_server_address,#idhd_remote_source_path,#idhd_remote_share_type,#idhd_remote_domain_name,#idhd_remote_user_name,#idhd_remote_password").parents(".form-group").hide();
            this.$("#idmount_fd,#idmount_hd").attr("disabled", "disabled");
            this.$("#idmount_fd,#idmount_hd").iCheck("disable");*/
            this.$("#rmedia_fd_part, #rmedia_hd_part").hide();
        },
        uncheckedSameSettings: function(ev) {
            $('#idsame_settings').val(0);
            this.model.set('same_settings', 0);
            /*this.$("#idfd_remote_server_address,#idfd_remote_source_path,#idfd_remote_share_type,#idhd_remote_server_address,#idhd_remote_source_path,#idhd_remote_share_type").parents(".form-group").show();
            this.$("#idmount_fd,#idmount_hd").removeAttr("disabled");
            this.$("#idmount_fd,#idmount_hd").iCheck("enable");*/
            this.$("#rmedia_fd_part, #rmedia_hd_part").show();
        },
        checkedMountFd: function(ev) {
            $('#idmount_fd').val(1);
            this.model.set('mount_fd', 1);
            //this.$("#idfd_remote_server_address,#idfd_remote_source_path,#idfd_remote_share_type").parents(".form-group").show();
            this.$("#rmedia_fd_details").show();
        },
        uncheckedMountFd: function(ev) {
            $('#idmount_fd').val(0);
            this.model.set('mount_fd', 0);
            //this.$("#idfd_remote_server_address,#idfd_remote_source_path,#idfd_remote_share_type,#idfd_remote_domain_name,#idfd_remote_user_name,#idfd_remote_password").parents(".form-group").hide();
            this.$("#rmedia_fd_details").hide();
        },
        checkedFdRemoteShareType: function(ev) {
            console.log('checked radio', $(ev.target).filter(':checked').val());
            this.model.set('fd_remote_share_type', $(ev.target).filter(':checked').val());
            if ($(ev.target).filter(':checked').val() == 'nfs') {
                this.$("#idfd_remote_domain_name,#idfd_remote_user_name,#idfd_remote_password").parents(".form-group").hide();
            }
            if ($(ev.target).filter(':checked').val() == 'cifs') {
                this.$("#idfd_remote_domain_name,#idfd_remote_user_name,#idfd_remote_password").parents(".form-group").show();
            }

        },
        uncheckedFdRemoteShareType: function() {},
        checkedMountHd: function(ev) {
            console.log("updtatemounthd show");
            $('#idmount_hd').val(1);
            this.model.set('mount_hd', 1);
            //this.$("#idhd_remote_server_address,#idhd_remote_source_path,#idhd_remote_share_type").parents(".form-group").show();
            this.$("#rmedia_hd_details").show();
        },
        uncheckedMountHd: function(ev) {
            console.log("updtatemounthd hide");
            $('#idmount_hd').val(0);
            this.model.set('mount_hd', 0);
            //this.$("#idhd_remote_server_address,#idhd_remote_source_path,#idhd_remote_share_type,#idhd_remote_domain_name,#idhd_remote_user_name,#idhd_remote_password").parents(".form-group").hide();
            this.$("#rmedia_hd_details").hide();
        },
        checkedHdRemoteShareType: function(ev) {
            console.log('checked radio', $(ev.target).filter(':checked').val());
            this.model.set('hd_remote_share_type', $(ev.target).filter(':checked').val());
            if ($(ev.target).filter(':checked').val() == 'nfs') {
                this.$("#idhd_remote_domain_name,#idhd_remote_user_name,#idhd_remote_password").parents(".form-group").hide();
            }
            if ($(ev.target).filter(':checked').val() == 'cifs') {
                this.$("#idhd_remote_domain_name,#idhd_remote_user_name,#idhd_remote_password").parents(".form-group").show();
            }

        },
        uncheckedHdRemoteShareType: function() {},
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
        updateLocalMediaSupport: function(model, value) {
            if (value) {
                $('#idlocal_media_support').iCheck('check');
            } else {
                if (!$('#idlocal_media_support').parent().hasClass('checked')) {
                    this.uncheckedLocalMediaSupport();
                }
                $('#idlocal_media_support').iCheck('uncheck');
            }
        },
        updateRemoteMediaSupport: function(model, value) {
            if (value) {
                $('#idremote_media_support').iCheck('check');
            } else {
                if (!$('#idremote_media_support').parent().hasClass('checked')) {
                    this.uncheckedRemoteMediaSupport();
                }
                $('#idremote_media_support').iCheck('uncheck');
            }
        },
        updateCdRemoteServerAddress: function(model, value) {
            $('#idcd_remote_server_address').val(value);
        },
        updateCdRemoteSourcePath: function(model, value) {
            $('#idcd_remote_source_path').val(value);
        },
        updateCdRemoteShareType: function(model, value) {
            $('input[name="cd_remote_share_type"]').iCheck('uncheck');
            $('input[name="cd_remote_share_type"][value="' + value + '"]').iCheck('check');
        },
        updateCdRemoteDomainName: function(model, value) {
            $('#idcd_remote_domain_name').val(value);
        },
        updateCdRemoteUserName: function(model, value) {
            $('#idcd_remote_user_name').val(value);
        },
        updateCdRemotePassword: function(model, value) {
            $('#idcd_remote_password').val(value);
        },
        updateFdRemoteServerAddress: function(model, value) {
            $('#idfd_remote_server_address').val(value);
        },
        updateFdRemoteSourcePath: function(model, value) {
            $('#idfd_remote_source_path').val(value);
        },
        updateFdRemoteShareType: function(model, value) {
            $('input[name="fd_remote_share_type"]').iCheck('uncheck');
            $('input[name="fd_remote_share_type"][value="' + value + '"]').iCheck('check');
        },
        updateFdRemoteDomainName: function(model, value) {
            $('#idfd_remote_domain_name').val(value);
        },
        updateFdRemoteUserName: function(model, value) {
            $('#idfd_remote_user_name').val(value);
        },
        updateFdRemotePassword: function(model, value) {
            $('#idfd_remote_password').val(value);
        },
        updateHdRemoteServerAddress: function(model, value) {
            $('#idhd_remote_server_address').val(value);
        },
        updateHdRemoteSourcePath: function(model, value) {
            $('#idhd_remote_source_path').val(value);
        },
        updateHdRemoteShareType: function(model, value) {
            $('input[name="hd_remote_share_type"]').iCheck('uncheck');
            $('input[name="hd_remote_share_type"][value="' + value + '"]').iCheck('check');
        },
        updateHdRemoteDomainName: function(model, value) {
            $('#idhd_remote_domain_name').val(value);
        },
        updateHdRemoteUserName: function(model, value) {
            $('#idhd_remote_user_name').val(value);
        },
        updateHdRemotePassword: function(model, value) {
            $('#idhd_remote_password').val(value);
        },
        updateMountCd: function(model, value) {
            if (value) {
                $('#idmount_cd').iCheck('check');
            } else {
                if (!$('#idmount_cd').parent().hasClass('checked')) {
                    this.uncheckedMountCd();
                }
                $('#idmount_cd').iCheck('uncheck');
            }
        },
        updateMountFd: function(model, value) {
            if (value) {
                $('#idmount_fd').iCheck('check');
            } else {
                if (!$('#idmount_fd').parent().hasClass('checked')) {
                    this.uncheckedMountFd();
                }
                $('#idmount_fd').iCheck('uncheck');
            }
        },
        updateMountHd: function(model, value) {
            if (value) {
                $('#idmount_hd').iCheck('check');
            } else {
                if (!$('#idmount_hd').parent().hasClass('checked')) {
                    this.uncheckedMountHd();
                }
                $('#idmount_hd').iCheck('uncheck');
            }
        },
        updateSameSettings: function(model, value) {
            if (value) {
                $('#idsame_settings').iCheck('check');
                this.$("#rmedia_fd_part, #rmedia_hd_part").hide();
            } else {
                if (!$('#idsame_settings').parent().hasClass('checked')) {
                    this.uncheckedSameSettings();
                }
                $('#idsame_settings').iCheck('uncheck');
                this.$("#rmedia_fd_part, #rmedia_hd_part").show();
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
                "mount_cd": "idmount_cd",
                "cd_remote_server_address": "idcd_remote_server_address",
                "cd_remote_source_path": "idcd_remote_source_path",
                "cd_remote_user_name": "idcd_remote_user_name",
                "cd_remote_password": "idcd_remote_password",
                "fd_remote_user_name": "idfd_remote_user_name",
                "fd_remote_password": "idfd_remote_password",
                "fd_remote_server_address": "idfd_remote_server_address",
                "hd_remote_user_name": "idhd_remote_user_name",
                "hd_remote_server_address": "idhd_remote_server_address",
                "hd_remote_password": "idhd_remote_password"
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
                'cd_remote_server_address': $('#idcd_remote_server_address').val(),
                'cd_remote_source_path': $('#idcd_remote_source_path').val(),
                'cd_remote_domain_name': $('#idcd_remote_domain_name').val(),
                'cd_remote_user_name': $('#idcd_remote_user_name').val(),
                'cd_remote_password': $('#idcd_remote_password').val(),
                'fd_remote_server_address': $('#idfd_remote_server_address').val(),
                'fd_remote_source_path': $('#idfd_remote_source_path').val(),
                'fd_remote_domain_name': $('#idfd_remote_domain_name').val(),
                'fd_remote_user_name': $('#idfd_remote_user_name').val(),
                'fd_remote_password': $('#idfd_remote_password').val(),
                'hd_remote_server_address': $('#idhd_remote_server_address').val(),
                'hd_remote_source_path': $('#idhd_remote_source_path').val(),
                'hd_remote_domain_name': $('#idhd_remote_domain_name').val(),
                'hd_remote_user_name': $('#idhd_remote_user_name').val(),
                'hd_remote_password': $('#idhd_remote_password').val()
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

        serialize: function() {
            return _.extend({
                locale: locale
            }, this.model.toJSON());
        }

    });

    return view;

});
