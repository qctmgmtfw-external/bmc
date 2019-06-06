define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//dns_settings', 'collection/dns_registration', 'collection/domain_options', 'collection/dns_options',
//localize
'i18n!strings/nls/dns',
//template
'text!templates/settings/dns.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, DnsSettingsModel, DnsRegistrationCollection, DomainOptionsCollection, DnsOptionsCollection, locale, DnsTemplate) {

    var view = Backbone.View.extend({

        template: _.template(DnsTemplate),

        initialize: function() {
            this.model = DnsSettingsModel;
            this.model.bind('change:dns_status', this.updateDnsStatus, this);
            this.model.bind('change:host_cfg', this.updateHostCfg, this);
            this.model.bind('change:host_name', this.updateHostName, this);
            this.model.bind('change:tsig_exists', this.updateTsigExists, this);
            this.model.bind('change:tsig_private', this.updateTsigPrivate, this);
            this.model.bind('change:tsig_privstat', this.updateTsigPrivstat, this);
            this.model.bind('change:tsig_newfile', this.updateTsigNewfile, this);
            this.model.bind('change:domain_manual', this.updateDomainManual, this);
            this.model.bind('change:domain_iface', this.updateDomainIface, this);
            this.model.bind('change:domain_name', this.updateDomainName, this);
            this.model.bind('change:dns_manual', this.updateDnsManual, this);
            this.model.bind('change:dns_iface', this.updateDnsIface, this);
            this.model.bind('change:dns_priority', this.updateDnsPriority, this);
            this.model.bind('change:dns_server1', this.updateDnsServer1, this);
            this.model.bind('change:dns_server2', this.updateDnsServer2, this);
            this.model.bind('change:dns_server3', this.updateDnsServer3, this);
            this.dns_registration = DnsRegistrationCollection;
            this.dns_registration.bind('add', this.addDnsRegistration, this);
            this.domain_options = DomainOptionsCollection;
            this.domain_options.bind('add', this.addDomainOptions, this);
            this.dns_options = DnsOptionsCollection;
            this.dns_options.bind('add', this.addDnsOptions, this);


        },

        events: {
            "ifChecked #iddns_status": "checkedDnsStatus",
            "ifUnchecked #iddns_status": "uncheckedDnsStatus",
            "ifChecked input[name='host_cfg']": "checkedHostCfg",
            "ifUnchecked input[name='host_cfg']": "uncheckedHostCfg",
            "change #filetsig_newfile": "chosenFileTsigNewfile",
            "ifChecked input[name='domain_manual']": "checkedDomainManual",
            "ifUnchecked input[name='domain_manual']": "uncheckedDomainManual",
            "ifChecked input[name='dns_manual']": "checkedDnsManual",
            "ifUnchecked input[name='dns_manual']": "uncheckedDnsManual",
            "ifChecked input[name='dns_priority']": "checkedDnsPriority",
            "ifUnchecked input[name='dns_priority']": "uncheckedDnsPriority",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.$el.find('#iddns_status,input[name="host_cfg"],input[name="domain_manual"],input[name="dns_manual"],input[name="dns_priority"]').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
            this.dns_registration.fetch();
            this.dns_registration.each(function(model) {
                this.addDnsRegistration(model, this.dns_registration);
            }, this);
            this.domain_options.fetch();
            this.domain_options.each(function(model) {
                this.addDomainOptions(model, this.domain_options);
            }, this);
            this.dns_options.fetch();
            this.dns_options.each(function(model) {
                this.addDnsOptions(model, this.dns_options);
            }, this);

            this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, this.model, value);
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

        checkedDnsStatus: function(ev) {
            this.model.set('dns_status', 1);
            $("#idhost_cfg,#idhost_name,.reg_ifc,.reg_enable,.reg_option,.reg_mdns,.reg_tsig,#idtsig_exists,#idtsig_private,#idtsig_privstat,#idtsig_newfile,#iddomain_manual,#iddomain_iface,#iddomain_name,#iddns_manual,#iddns_iface,#iddns_priority,#iddns_server1,#iddns_server2,#iddns_server3").removeAttr("disabled").iCheck("enable");
        },
        uncheckedDnsStatus: function(ev) {
            this.model.set('dns_status', 0);
            $("#idhost_cfg,#idhost_name,.reg_ifc,.reg_enable,.reg_option,.reg_mdns,.reg_tsig,#idtsig_exists,#idtsig_private,#idtsig_privstat,#idtsig_newfile,#iddomain_manual,#iddomain_iface,#iddomain_name,#iddns_manual,#iddns_iface,#iddns_priority,#iddns_server1,#iddns_server2,#iddns_server3").attr("disabled", "disabled").iCheck("disable");
        },
        checkedHostCfg: function(ev) {
            this.model.set('host_cfg', $(ev.target).filter(':checked').val());

            if ($(ev.target).filter(':checked').val() == '1') {
                $("#idhost_name").attr("disabled", "disabled");
            }
            if ($(ev.target).filter(':checked').val() == '0') {
                $("#idhost_name").removeAttr("disabled");
            }
        },
        uncheckedHostCfg: function() {},
        addDnsRegistration: function(model, collection) {
            var id = model.get('id');

            $('#display_registration').append($.parseHTML($.trim('<div class="form-group" >' +
                '<label for="reg_ifc'+id+'">'+locale.dns_settingsbmc_interface+'</label>' +
                '<div class="form-control-static" id="idreg_ifc'+id+'"></div>' +
                '</div><div class="form-group" >' +
                '<label>' +
                '<input class="reg_enable" type="checkbox" value="1" id="idreg_enable'+id+'" > '+locale.dns_settingsregister_bmc +
                '</label>' +
                '</div><div class="alert alert-info help-item hide"   role="alert">'+locale.dns_settingsul_libnsupdateb_register_with_the_dns_server_using_the_nsupdate_applicationli_libdhcp_client_fqdnb_register_with_the_dns_server_using_dhcp_option_81li_libhostnameb_register_with_the_dns_server_using_dhcp_option_12li_ul_bnoteb_hostname_option_should_be_selected_if_the_dhcp_server_does_not_support_option_81+'</div><div class="form-group" >' +
                '<label for="reg_option'+id+'">'+locale.dns_settingsregistration_method+'</label><br/>' +
                '<label>' +
                        '<input class="reg_option" type="radio" value="0" name="reg_option'+id+'" id="idreg_option'+id+'" > '+locale.dns_settingsnsupdate +
                    '</label>&nbsp;&nbsp;&nbsp;&nbsp;<label>' +
                        '<input class="reg_option" type="radio" value="1" name="reg_option'+id+'" id="idreg_option'+id+'" > '+locale.dns_settingsdhcp_client_fqdn +
                    '</label>&nbsp;&nbsp;&nbsp;&nbsp;<label>' +
                        '<input class="reg_option" type="radio" value="2" name="reg_option'+id+'" id="idreg_option'+id+'" > '+locale.dns_settingshostname +
                    '</label>&nbsp;&nbsp;&nbsp;&nbsp;' +
                '</div><div class="alert alert-info help-item hide"   role="alert">'+locale.dns_settingscheck_this_box_to_enable_multicast_dns+'</div><div class="form-group" >' +
                '<label>' +
                '<input class="reg_mdns" type="checkbox" value="1" id="idreg_mdns'+id+'" > '+locale.dns_settingsmdns_enabled +
                '</label>' +
                '</div><div class="alert alert-info help-item hide"   role="alert">'+locale.dns_settingscheck_this_box_to_enable_tsig_authentication_while_registering_dns_via_nsupdate+'</div><div class="form-group" >' +
                '<label>' +
                '<input class="reg_tsig" type="checkbox" value="1" id="idreg_tsig'+id+'" > '+locale.dns_settingstsig_authentication_enabled +
                '</label>' +
                '</div>')));

            $('#display_registration input').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            $('#idreg_ifc'+id).text(model.get('reg_ifc'));
            if (model.get('reg_enable')) {
                $('#idreg_enable'+id).iCheck('check');
            } else {
                $('#idreg_enable'+id).iCheck('uncheck');
            }
            $('input[name="reg_option'+id+'"]').iCheck('uncheck');
            $('input[name="reg_option'+id+'"][value="' + model.get('reg_option') + '"]').iCheck('check');
            if (model.get('reg_mdns')) {
                $('#idreg_mdns'+id).iCheck('check');
            } else {
                $('#idreg_mdns'+id).iCheck('uncheck');
            }
            if (model.get('reg_tsig')) {
                $('#idreg_tsig'+id).iCheck('check');
            } else {
                $('#idreg_tsig'+id).iCheck('uncheck');
            }

        },
        chosenFileTsigNewfile: function(ev) {
            var input = ev.target;
            $('#idtsig_newfile').val($('#filetsig_newfile')[0].files[0].name);
        },
        checkedDomainManual: function(ev) {
            this.model.set('domain_manual', $(ev.target).filter(':checked').val());
            if ($(ev.target).filter(':checked').val() == '0') {
                $("#iddomain_interface").parents(".form-group").show();
                $("#iddomain_name").parents(".form-group").hide();
            }
            if ($(ev.target).filter(':checked').val() == '1') {
                $("#iddomain_name").parents(".form-group").show();
                $("#iddomain_interface").parents(".form-group").hide();
            }

        },
        uncheckedDomainManual: function() {},
        addDomainOptions: function(model, collection) {
            var o = model.get('domain_iface');
            $('#iddomain_interface').append($('<option></option>').attr('value', o).text(o));
        },
        checkedDnsManual: function(ev) {
            this.model.set('dns_manual', $(ev.target).filter(':checked').val());
            if ($(ev.target).filter(':checked').val() == '0') {
                $("#iddns_interface,#iddns_priority").parents(".form-group").show();
                $("#iddns_server1,#iddns_server2,#iddns_server3").parents(".form-group").hide();
            }
            if ($(ev.target).filter(':checked').val() == '1') {
                $("#iddns_server1,#iddns_server2,#iddns_server3").parents(".form-group").show();
                $("#iddns_interface,#iddns_priority").parents(".form-group").hide();
            }

        },
        uncheckedDnsManual: function() {},
        addDnsOptions: function(model, collection) {
            var o = model.get('dns_iface');
            $('#iddns_interface').append($('<option></option>').attr('value', o).text(o));
        },
        checkedDnsPriority: function(ev) {
            this.model.set('dns_priority', $(ev.target).filter(':checked').val());


        },
        uncheckedDnsPriority: function() {},
        toggleHelp: function(e) {
            e.preventDefault();
            var help_items = this.$('.help-item').filter(function() {
                var f = $(this).data('feature');
                if (f) {
                    return (app.features.indexOf(f) == -1 ? false : true);
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
        updateDnsStatus: function(model, value) {
            console.log('val set', value);
            if (value) {
                $('#iddns_status').iCheck('check');
            } else {
                if (!$('#iddns_status').parent().hasClass('checked')) {
                    this.uncheckedDnsStatus();
                }
                $('#iddns_status').iCheck('uncheck');
            }
        },
        updateHostCfg: function(model, value) {
            $('input[name="host_cfg"]').iCheck('uncheck');
            $('input[name="host_cfg"][value="' + value + '"]').iCheck('check');
        },
        updateHostName: function(model, value) {
            $('#idhost_name').val(value);
        },
        updateTsigExists: function(model, value) {
            if(value) {
                $('#idtsig_privstat').parents(".form-group").show();
            } else {
                $('#idtsig_privstat').parents(".form-group").hide();
            }
        },
        updateTsigPrivate: function(model, value) {
            $('#idtsig_private').html(value);
        },
        updateTsigPrivstat: function(model, value) {
            $('#idtsig_privstat').html(value);
        },
        updateTsigNewfile: function(model, value) {
            $('#idtsig_newfile').val(value);
        },
        updateDomainManual: function(model, value) {
            $('input[name="domain_manual"]').iCheck('uncheck');
            $('input[name="domain_manual"][value="' + value + '"]').iCheck('check');
        },
        updateDomainIface: function(model, value) {
            $('#iddomain_interface').val(value);
        },
        updateDomainName: function(model, value) {
            $('#iddomain_name').val(value);
        },
        updateDnsManual: function(model, value) {
            $('input[name="dns_manual"]').iCheck('uncheck');
            $('input[name="dns_manual"][value="' + value + '"]').iCheck('check');
        },
        updateDnsIface: function(model, value) {
            $('#iddns_interface').val(value);
        },
        updateDnsPriority: function(model, value) {
            $('input[name="dns_priority"]').iCheck('uncheck');
            $('input[name="dns_priority"][value="' + value + '"]').iCheck('check');
        },
        updateDnsServer1: function(model, value) {
            $('#iddns_server1').val(value);
        },
        updateDnsServer2: function(model, value) {
            $('#iddns_server2').val(value);
        },
        updateDnsServer3: function(model, value) {
            $('#iddns_server3').val(value);
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

        saveConfirm: function() {
            if(!confirm(locale.saveConfirmLabel)) return;
            this.save.call(this);
        },

        saveAllModels: function() {

            var count = 0;
            var that = this;

            this.model.save({
                'host_name': $('#idhost_name').val(),
                'iface_name': $('#').val(),
                'domain_iface': $('#iddomain_interface').val(),
                'domain_name': $('#iddomain_name').val(),
                'dns_iface': $('#iddns_interface').val(),
                'dns_server1': $('#iddns_server1').val(),
                'dns_server2': $('#iddns_server2').val(),
                'dns_server3': $('#iddns_server3').val()
            }, {
                success: function() {
                    that.dns_registration.each(function(model) {
                        var id = model.get('id');
                        var enable, option, mdns, tsig;

                        enable = $('#idreg_enable'+id).parent().hasClass('checked') ? 1 : 0;
                        option = $('input[name="reg_option'+id+'"]').filter(':checked').val();
                        mdns = $('#idreg_mdns'+id).parent().hasClass('checked') ? 1 : 0;
                        tsig = $('#idreg_tsig'+id).parent().hasClass('checked') ? 1 : 0;

                        model.save({
                            'id': id,
                            'reg_ifc': model.get('reg_ifc'),
                            'reg_enable': enable,
                            'reg_option': option,
                            'reg_mdns': mdns,
                            'reg_tsig': tsig
                        }, {
                            success: function() {
                                count++;
                                if(count == (that.dns_registration.length)) {
                                    that.restartDNS.call(that);
                                }
                            },

                            error: function() {
                                $("#save-icon").removeClass().addClass("fa fa-save");
                                app.events.trigger('save_error', that);
                            }
                        });
                    }, this);                   
                },

                error: function() {
                    $("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', that);
                }
            });
        },

        save: function() {


            $("#save-icon").removeClass().addClass("ion-loading-c");


            if (this.$('#filetsig_newfile')[0].files.length) {
                var that = this;

                var data = new FormData();
                data.append('tsig_newfile', this.$('#filetsig_newfile')[0].files[0]);

                var file_upload_url = "/api/settings/dns/tsig-private-key";

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

                        context.saveAllModels.call(context);
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

                context.saveAllModels.call(context);
            }


        },

        restartDNS: function() {
            var that = this;
            $.ajax({
                url: "/api/settings/dns/restart",
                type: "PUT",
                dataType: "json",
                data: {},
                contentType: "application/json",
                success: function(data, status, xhr) {
                    $("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', that);
                },
                error : app.HTTPErrorHandler
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
