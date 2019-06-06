define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'collection/users',
        'text!templates/login.html',
        //localize
        'i18n!strings/nls/common',
        //plugins
        'tooltip'
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, app, UsersCollection, LoginTemplate, CommonStrings) {

        //BIT manipulation for privilege
        var KVM_ENABLED = 0, VMEDIA_ENABLED = 1; //Extended privilege

        var view = Backbone.View.extend({

            template: _.template(LoginTemplate),

            initialize: function() {

            },

            events: {
                "submit form": "login",
                "click #remember_me": "updateremember_me",
                //"click #btn-login": "login",
                "click #forgot-password": "forgotPassword"
            },

            beforeRender: function() {
            	// Quanta++
                var oemImage = new Image();
                var imageURL = "/images/oem_login.jpg"
                oemImage.src = imageURL;
                oemImage.onerror = function(){
                	 //Okey, we could not find the oem logo, so..add qci selector for background image.
                	$("#login-box").addClass("img-qci");
                	$("#copyRightID").show();//Display copyright
                };
                oemImage.onload = function(){
                	$("#login-box").addClass("img-cus");
                	$("#copyRightID").remove();//remove copy right, avoid customer can find it in DOM.
                };
            	// +++Quanta
                this.$el.addClass("form-box").attr("id", "login-box");
                this.$el.removeClass("animated fadeOutDown").addClass("animated fadeInDown");
                $("html,body").addClass("bg-black");
                $("#copyRightID").hide();
            },

            afterRender: function() {
                document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
	            var that = this;
	            _.defer(function(){
                    that.$('#userid').focus();
                });

               this.remember_me_username();
            },
            
            updateremember_me: function() {

                if ($('#remember_me').is(':checked')) {
                    localStorage.remember_usrname = $('#userid').val();
                    localStorage.remember_chkbx = true;
                } else {
                    localStorage.remember_usrname = '';
                    localStorage.remember_chkbx = false;
                }

            },
            
            remember_me_username: function() {
                if (localStorage.remember_chkbx != undefined) {
                     if (localStorage.remember_chkbx == "true") {
                        $('#remember_me').attr('checked', 'checked');
                         $('#userid').val(localStorage.remember_usrname);
                     } else { 
                         $('#remember_me').removeAttr('checked');
                         $('#userid').val('');
                     }                  
                 }
            },
			
            login: function() {
                var context = this;
                this.updateremember_me();
                $("#processing_maindiv").show();
                window.sessionStorage.setItem('username', $("#userid").val());
                $.ajax({
                    type: "POST",
                    url: "/api/session",
                    dataType: "json",
                    data: {
                        username: $("#userid").val(),
                        password: $("#password").val()
                    },
                    success: function(data) {
                        document.cookie = "refresh_disable=1";
                    	var userPrivilege = ['User', 'OEM', 'Operator', 'Administrator'];
                    	if (data.ok === 0 && /*Quanta*/data.privilege != CONSTANTS["USER"]) {
                            context.$('#userid').tooltip('destroy');
                            context.$('#userid').tooltip('hide');
                            context.$el.removeClass("animated fadeInDown").addClass("animated fadeOutDown");
                            $("html,body").removeClass("bg-black");
                            $.ajaxSetup({
                                headers: {'X-CSRFTOKEN': data.CSRFToken}
                            });

                            //window.sessionStorage.setItem('privilege', data.privilege);
                            //garc - csrftoken.. just a name change here
                            //storing to keep the session active across reload
                            //a csrf attack form cannot access sessionstorage of the primary domain
                            window.sessionStorage.setItem("garc", data.CSRFToken);
                            window.sessionStorage.setItem("privilege_id", data.privilege);
                            window.sessionStorage.setItem("extended_privilege", data.extendedpriv);
                            window.sessionStorage.setItem("session_id", data.racsession_id);


                            var ex_priv = parseInt(window.sessionStorage.extended_privilege);

                            //if the 0th bit in kvm privilege is 1, then KVM is enabled
                            var isKVMEnabled = (ex_priv & (1 << 0)) != 0;
                            //if the 1st bit in kvm privilege is 1, then VMedia is enabled
                            var isVMediaEnabled = (ex_priv & (1 << 1)) != 0;

                            if(data.privilege > CONSTANTS["USER"]){
                                UsersCollection.fetch({
                                    success: function(){
                                        $("#processing_maindiv").hide();
                                        var ourGuy = UsersCollection.where({"name": $("#userid").val()}, true);
                                        /* Enabling KVM / VMedia access */
                                        window.sessionStorage.setItem('kvm_access', 1); /* 1 = ENABLE */
                                        window.sessionStorage.setItem('vmedia_access', 1);

                                        if(typeof ourGuy != "undefined") {
                                        	window.sessionStorage.setItem('privilege', ourGuy.get('network_privilege').charAt(0).toUpperCase()+ourGuy.get('network_privilege').slice(1));
                                            window.sessionStorage.setItem('id', ourGuy.attributes.id);
                                            window.sessionStorage.setItem('since', ourGuy.attributes.creation_time);
                                        }
                                        else {
                                        	window.sessionStorage.setItem('privilege', userPrivilege[data.privilege-1]);
                                        	window.sessionStorage.setItem('id', 0);
                                            window.sessionStorage.setItem('since', Math.floor(Date.now() / 1000));
                                        }

                                        /* If the user has no KVM privilege, then disable kvm access */
                                        if(!isKVMEnabled){
                                            window.sessionStorage.setItem('kvm_access', 0); /* 0 = DISABLE */
                                        }

                                        /* If the user has no VMedia privilege, then disable vmedia access */
                                        if(!isVMediaEnabled){
                                            window.sessionStorage.setItem('vmedia_access', 0);
                                        }

                                        setTimeout(function() {
                                                //app.useLayout("layouts/main");
                                            app.initialize();
                                            app.router.navigate("dashboard", {
                                                trigger: true
                                            });
                                        }, 1000);
                                    },
                                    error: function() {               
                                    	$("#processing_maindiv").hide();
                                    	window.sessionStorage.setItem('privilege', userPrivilege[data.privilege-1]);
                                    	window.sessionStorage.setItem('id', 0);
                                        window.sessionStorage.setItem('since', Math.floor(Date.now() / 1000));
                                        
                                    	setTimeout(function() {
    	                                        //app.useLayout("layouts/main");
    	                                    app.initialize();
    	                                    app.router.navigate("dashboard", {
    	                                        trigger: true
    	                                    });
    	                                }, 1000);
                                    }
                                });
                            }
                            else{
                                $("#processing_maindiv").hide();
                                window.sessionStorage.setItem('privilege', 'User');
                                //window.sessionStorage.setItem('id', data.userid);
                                window.sessionStorage.setItem('id', 0);
                                window.sessionStorage.setItem('since', Math.floor(Date.now() / 1000));
                                window.sessionStorage.setItem('kvm_access', 0);
                                window.sessionStorage.setItem('vmedia_access', 0);

                                /* If the user has KVM privilege, then enable kvm access */
                                if(isKVMEnabled){
                                    window.sessionStorage.setItem('kvm_access', 1);
                                }

                                /* If the user has VMedia privilege, then enable vmedia access */
                                if(isVMediaEnabled){
                                    window.sessionStorage.setItem('vmedia_access', 1);
                                }

                                setTimeout(function() {
                                        //app.useLayout("layouts/main");
                                    app.initialize();
                                    app.router.navigate("dashboard", {
                                        trigger: true
                                    });
                                }, 1000);
                            }
                        } else {
                        	//Quanta++
                        	if(data.privilege == CONSTANTS["USER"]){
                                $("#processing_maindiv").hide();
                               	context.$('#userid').tooltip('destroy');
                            	context.$('#userid').tooltip({
                                    animation: false,
                                    title: "User privilege could not login",
                                    trigger: 'manual',
                                    placement: 'top'
                                });
                                context.$('#userid').tooltip('show').removeClass("animated shake").addClass("animated shake");
                        	} else {
                        	//Quanta==
                        		$("#processing_maindiv").hide();
                        		context.$('#userid').tooltip('destroy');
                        		context.$('#userid').tooltip({
                        			animation: false,
                        			title: CommonStrings._nvalid_username_or_password,
                        			trigger: 'manual',
                        			placement: 'top'
                        		});
                        		context.$('#userid').tooltip('show');
                        		context.$el.find("#userid,#password")
                                .removeClass("animated shake")
                                .addClass("animated shake");
                        	}
                        }
                    },
                    error: function(xhr, textStatus, errorThrown) {
                        var errorResponse = xhr;
                        var errorJsonResponse = errorResponse.responseJSON;
                        $("#processing_maindiv").hide();
                        context.$('#userid').tooltip('destroy');

                        if(errorJsonResponse.code != 1009){
                            var errorString = "login_error_code_"+errorJsonResponse.code;
                            context.$('#userid').tooltip({
                                animation: false,
                                title: CommonStrings[errorString],
                                trigger: 'manual',
                                placement: 'top'
                            });	
                            context.$('#userid').tooltip('show');
                            context.$el.find("#userid,#password")
                                .removeClass("animated shake")
                                .addClass("animated shake");
                        }
                        else {
                            context.$('#userid').tooltip({
                                animation: false,
                                title: CommonStrings._nvalid_username_or_password,
                                trigger: 'manual',
                                placement: 'top'
                            });

                            context.$('#userid').tooltip('show');
                            context.$el.find("#userid,#password")
                            .removeClass("animated shake")
                            .addClass("animated shake");
                        }
                    }
                });
            },

            forgotPassword: function() {
                if($("#userid").val() == "") {
                    alert(CommonStrings.missing_username);
                    return;
                }
                    
                if (confirm(CommonStrings.pw_reset_confirm)) {
                    $.ajax({
                        type: "POST",
                        url: "/api/reset-pass",
                        dataType: "json",
                        data: {
                            username: $("#userid").val()
                        },
                        complete: function(data) {
                            var returncode = data.responseJSON.code & 0xff;
                            
                            switch (returncode)
                            {
                                case 0x0:
                                    alert(CommonStrings.pw_reset_success);
                                    break;
                                case 0x11:
                                    alert(CommonStrings.pw_reset_smtp_disabled);
                                    break;
                                case 0x12:
                                case 0x13:
                                case 0x14:
                                    alert(CommonStrings["pw_reset_smtp_fail_" + (returncode - 0x11)]);
                                    break;
                                case 0x3:
                                    alert(CommonStrings.pw_reset_email_fail_1);
                                    break;
                                case 0x10:
                                    alert(CommonStrings.pw_reset_email_fail_2);
                                    break;
                                case 0x15:
                                    alert(CommonStrings.pw_reset_fail_email);
                                    break;
                                default:
                                    alert(CommonStrings.pw_reset_fail + ": " + returncode);
                            }
                        }
                    });
                }
            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });