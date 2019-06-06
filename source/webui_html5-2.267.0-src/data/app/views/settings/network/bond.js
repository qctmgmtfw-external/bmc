define(['jquery', 'underscore', 'backbone', 'app',
//data
'models//bond',
//localize
'i18n!strings/nls/bond',
//template
'text!templates/settings/network/bond.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, BondModel, locale, BondTemplate) {

    var view = Backbone.View.extend({

        template: _.template(BondTemplate),

        initialize: function() {
            /*if(sessionStorage.privilege_id < CONSTANTS["OPERATOR"]){
                app.hidePage();
            }*/
            this.model = BondModel;
            this.model.bind('change:bond_enable', this.updateBondEnable, this);
            this.model.bind('change:bond_mode', this.updateBondMode, this);
            this.model.bind('change:bond_ifc', this.updateBondIfc, this);
            this.model.bind('change:auto_configuration_enable', this.updateAutoConfigurationEnable, this);

            this.previous_bondenable = -1;
            this.previous_bondifc = "";
        },

        events: {
            "ifChecked #idbond_enable": "checkedBondEnable",
            "ifUnchecked #idbond_enable": "uncheckedBondEnable",
            "ifChecked #idauto_configuration_enable": "checkedAutoConfigurationEnable",
            "ifUnchecked #idauto_configuration_enable": "uncheckedAutoConfigurationEnable",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.$el.find('#idbond_enable,#idauto_configuration_enable').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });

            //this.model.clear();
            //this.model.set({});

            this.model.fetch();
            /*this.model.fetch({
                error: function(xhr, status, err) {
                    var errorResponse = status;
                    var errorJsonResponse = errorResponse.responseJSON;
                    if(errorResponse.status == 500 && errorJsonResponse.code == 8000){
                        app.hidePage();
                    }
                }
            });*/
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, _parent.model, value);
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

        },

        checkedBondEnable: function(ev) {
            $('#idbond_enable').val(1);
            console.log("this.previous_bondenable", this.previous_bondenable);
            if(this.previous_bondenable == 1){
                this.$("#idauto_configuration_enable").attr("disabled", "disabled");
                this.$("#idauto_configuration_enable").iCheck("disable"); 
            }else{
                this.$("#idauto_configuration_enable").removeAttr("disabled");
                this.$("#idauto_configuration_enable").iCheck("enable");
            }
            this.$("#idbond_ifc,#idbond_mode").removeAttr("disabled");
            this.$("#idbond_ifc,#idbond_mode").iCheck("enable");
        },
        uncheckedBondEnable: function(ev) {
            $('#idbond_enable').val(0);
            if(this.previous_bondenable == 0){
                this.$("#idauto_configuration_enable").attr("disabled", "disabled");
                this.$("#idauto_configuration_enable").iCheck("disable"); 
            }else{
                this.$("#idauto_configuration_enable").removeAttr("disabled");
                this.$("#idauto_configuration_enable").iCheck("enable");
            }
            this.$("#idbond_ifc,#idbond_mode").attr("disabled", "disabled");
            this.$("#idbond_ifc,#idbond_mode").iCheck("disable");
        },
        checkedAutoConfigurationEnable: function(ev) {
            $('#idauto_configuration_enable').val(1);
        },
        uncheckedAutoConfigurationEnable: function(ev) {
            $('#idauto_configuration_enable').val(0);
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
        updateBondEnable: function(model, value) {
            this.previous_bondenable = value;
            if (value) {
                $('#idbond_enable').iCheck('check');
            } else {
                if (!$('#idbond_enable').parent().hasClass('checked')) {
                    this.uncheckedBondEnable();
                }
                $('#idbond_enable').iCheck('uncheck');
            }
        },
        updateBondMode: function(model, value) {
            $('#idbond_mode').html(value);
        },
        updateBondIfc: function(model, value) {
            this.previous_bondifc = value;
            $('#idbond_ifc').val(value);
        },
        updateAutoConfigurationEnable: function(model, value) {
            if (value) {
                $('#idauto_configuration_enable').iCheck('check');
            } else {
                if (!$('#idauto_configuration_enable').parent().hasClass('checked')) {
                    this.uncheckedAutoConfigurationEnable();
                }
                $('#idauto_configuration_enable').iCheck('uncheck');
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
            $('#Validationerror').css('display','none');
            /*console.log("saveConfirm previous", this.previous_enable);
            console.log("saveConfirm current value", $('#idbond_enable').val());*/
            if( ($('#idbond_enable').val() != this.previous_bondenable) || (($('#idbond_enable').val()==1) && ($('#idbond_ifc').val() != this.previous_bondifc))){
                if (!confirm(locale.saveConfirmLabel)) return;
                this.save.call(this);
            }else{
                $('#Validationerror').html(locale.same_bond_settings);
                //$('#Validationerror').css('display','block');
                return false;
            }
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
                'bond_enable': parseInt($('#idbond_enable').val(), 10),
                'auto_configuration_enable': parseInt($('#idauto_configuration_enable').val(), 10),
                'bond_ifc': $('#idbond_ifc').val()
            }, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success_alert', context);
                },

                error: function(collection, response, options) {
                    /*var errorcode = '';
                    var errormsg = '';
                    if (response.responseJSON.code != undefined) errorcode = response.responseJSON.code;
                    else if (response.responseJSON.cc != undefined) errorcode = response.responseJSON.cc;
                    if (response.responseJSON.error != undefined) errormsg = response.responseJSON.error;

                    if (typeof context.getStdErrMsg == 'function') {
                        context.getStdErrMsg(errorcode, errormsg);
                    }


                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error_alert', context);*/
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
