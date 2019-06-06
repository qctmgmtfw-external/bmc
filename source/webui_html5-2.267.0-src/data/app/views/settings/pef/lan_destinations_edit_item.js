define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//lan_destinations', 'collection//users_read_only',
//localize
'i18n!strings/nls/lan_destinations',
//template
'text!templates/settings/pef/lan_destinations_edit_item.html',
//plugins
'iCheck', 'tooltip', 'alert'],

function($, _, Backbone, app, LanDestinationsCollection, UsersReadOnlyCollection, locale, LanDestinationsEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(LanDestinationsEditItemTemplate),

        initialize: function() {
            LanDestinationsCollection.fetch({
                async: false
            });
            UsersReadOnlyCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            this.userFormat = [];
            this.email_id = [];
            if (!isNaN(id)) {
                this.model = LanDestinationsCollection.get(id);
            } else {
                this.model = new LanDestinationsCollection.model;
                this.model.url = LanDestinationsCollection.url;
            };
            this.model.bind('change:lan_channel', this.updateLanChannel, this);
            this.model.bind('change:channel_id', this.updateChannelId, this);
            this.model.bind('change:destination_type', this.updateDestinationType, this);
            this.model.bind('change:destination_address', this.updateDestinationAddress, this);
            this.model.bind('change:name', this.updateName, this);
            this.model.bind('change:subject', this.updateSubject, this);
            this.model.bind('change:message', this.updateMessage, this);
            this.users_read_only = UsersReadOnlyCollection;
            this.users_read_only.bind('add', this.addUsersReadOnly, this);

            this.model.bind('validated:valid', this.validData, this);
            this.model.bind('validated:invalid', this.invalidData, this);
        },

        events: {
            "ifChecked input[name='destination_type']": "checkedDestinationType",
            "ifUnchecked input[name='destination_type']": "uncheckedDestinationType",
            "change #idname": "changeFormat",
            "click #delete": "deleteConfirm",
            "click #save": "saveConfirm",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            this.$el.find('input[name="destination_type"]').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
            //this.users_read_only.fetch();
            //console.log(this.users_read_only);
            if(this.users_read_only.length > 0){
            this.users_read_only.each(function(model) {
                this.addUsersReadOnly(model, this.users_read_only);
            }, this);
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

        checkedDestinationType: function(ev) {
        	var context = this;
            this.model.set('destination_type', $(ev.target).filter(':checked').val());
            if ($(ev.target).filter(':checked').val() == 'snmp') {
                this.$("#iddestination_address").removeAttr("disabled");
                this.$("#iddestination_address").iCheck("enable");
                this.$("#idname,#idsubject,#idmessage").attr("disabled", "disabled");
                this.$("#idname,#idsubject,#idmessage").iCheck("disable");
            }
            if ($(ev.target).filter(':checked').val() == 'email') {
                this.$("#idname").removeAttr("disabled");
                this.$("#idname").iCheck("enable");
                this.$("#iddestination_address").attr("disabled", "disabled");
                this.$("#iddestination_address").iCheck("disable");
                if(context.userFormat[$("#idname").val()] == "ami_format"){
                	this.$("#idsubject,#idmessage").attr("disabled", "disabled");
                    this.$("#idsubject,#idmessage").iCheck("disable");
                }else{
                	this.$("#idsubject,#idmessage").removeAttr("disabled");
                    this.$("#idsubject,#idmessage").iCheck("enable");
                }
            }
        },
        uncheckedDestinationType: function() {},
        changeFormat: function(ev) {
        	var context = this;
        	var user = $(ev.target).val();
        	if(context.userFormat[user] == "ami_format"){
            	this.$("#idsubject,#idmessage").attr("disabled", "disabled");
                this.$("#idsubject,#idmessage").iCheck("disable");
            }else{
            	this.$("#idsubject,#idmessage").removeAttr("disabled");
                this.$("#idsubject,#idmessage").iCheck("enable");
            }
        	
        },
        addUsersReadOnly: function(model, collection) {
        	var context = this;
        	var o = model.get('name');
            $('#idname').append($('<option></option>').attr('value', o).text(o));
            var email_format = model.get('email_format');
            context.userFormat[o] = email_format;
            var emailid = model.get('email_id');
            context.email_id[o] = emailid;
        },
        deleteConfirm: function() {
            if (!confirm(locale.deleteConfirmLabel)) return;
            this.delete.call(this);
        },
        saveConfirm: function() {
        	var context = this;
        	var name = $('#idname').val();
        	if (($('input[name=destination_type]:checked').val()) == "email")
        	{
        		if(context.email_id[name] == "")
        			{
        				if(!confirm(locale.emailConfirmLabel)) return;
        			}
        	}
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
        
        updateLanChannel: function(model, value) {
            $('#idlan_channel').html(value);
        },
        updateChannelId: function(model, value) {
            $('#idchannel_id').html(value);
        },
        updateDestinationType: function(model, value) {
            $('input[name="destination_type"]').iCheck('uncheck');
            $('input[name="destination_type"][value="' + value + '"]').iCheck('check');
        },
        updateDestinationAddress: function(model, value) {
            $('#iddestination_address').val(value);
        },
        updateName: function(model, value) {
        	var context = this;
        	$('#idname').val(value);
        },
        updateSubject: function(model, value) {
            $('#idsubject').val(value);
        },
        updateMessage: function(model, value) {
            $('#idmessage').val(value);
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
                "destination_address": "iddestination_address",
                "name": "idname",
                "subject": "idsubject",
                "message": "idmessage"
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
                'destination_address': $('#iddestination_address').val(),
                'name': $('#idname').val(),
                'format': context.userFormat[$('#idname').val()],
                'subject': $('#idsubject').val(),
                'message': $('#idmessage').val()
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

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
