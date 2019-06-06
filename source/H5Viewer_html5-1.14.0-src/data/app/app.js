/*define(function(require, exports, module) {
  "use strict";

  // External dependencies.
  var _ = require("underscore");
  var $ = require("jquery");
  var Backbone = require("backbone");

  // Alias the module for easier identification.
  var app = module.exports;

  // The root path to run the application through.
  app.root = "/";
});
*/
define([
        // Libraries.
        "jquery",
        "underscore",
        "backbone",
        "HTTPErrorHandler",        
        "collection/project-configurations",
        "collection/runtime-configurations",
        //views
        //locale
        "i18n!strings/nls/common",
        //template        
        'text!templates/layouts/withoutmenu.html',
        // Plugins.
        //"db",
        "layoutmanager",
        "backbone-validation",
        "dropdown",
        "slimscroll"
        //"ydn.db"
    ],

    function($, _, Backbone, HTTPErrorHandler, ProjectConfigCollection, RuntimeConfigCollection, CommonStrings, WithoutMenuTemp,Layout, Validation) {

        "use strict";

        Backbone.Model.prototype = _.extend(Backbone.Model.prototype, Validation.mixin);

        //Constants
        window.CONSTANTS = {};
        //Privilege
        window.CONSTANTS["CALLBACK"] = 1;
        window.CONSTANTS["USER"] = 2;
        window.CONSTANTS["OPERATOR"] = 3;
        window.CONSTANTS["ADMIN"] = 4;
        window.CONSTANTS["OEM"] = 5;
        //Fixed User count
        window.CONSTANTS["FIXED_USER_COUNT"] = 2;

        //Localize or create a new JavaScript Constants like Global variables.
        var CONSTANTS = window.CONSTANTS = window.CONSTANTS || {};

        // Provide a global location to place configuration settings and module
        // creation.
        var app = {
            root: "./",

            /* Function used to copy the values from localStorage to sessionStorage object */
            updateStorage: function(){
                var sessionStorageItems = [
                    'extended_privilege',
                    'features',
                    'garc',
                    'id',
                    'kvm_access',
                    'privilege',
                    'privilege_id',
                    'session_id',
                    'since',
                    'username',
                    'vmedia_access'
                ];
                sessionStorageItems.forEach( function(item){
                    if ( sessionStorage.getItem(item) == null ) { // update only if null values found
                        sessionStorage.setItem(item, localStorage.getItem(item));
                    }
                    localStorage.removeItem(item); // clear the localStorage
                });
            },
            initialize: function() {
                /* IE11 Specific fix.
                ** When opening H5Viewer popup window,
                ** sessionStorage object is not transferred from the WebUI to H5Viewer window.
                ** So copying the values to localStorage.
                ** When H5Viewer window is opened, check for sessionStorage object.
                ** If it contains null values, then update the required values from localStorage.
                ** Clear the localStorage afterwards.*/
                if ( navigator.userAgent.indexOf('.NET') !== -1 ) { // If navigator.userAgent contains .NET then the browser is IE
                    this.updateStorage();
                }

                console.log("App initialized...");

                document.title = CommonStrings.title;

                locale = CommonStrings;

                //this.layout.on("afterRender", );

                if (!document.cookie) {
                    return;
                }
                var thats = this ;
                 //garc - csrftoken.. just a name change here
                $.ajaxSetup({
                    headers: {'X-CSRFTOKEN': sessionStorage.getItem("garc")}
                });

                this.useLayout("layouts/main");

                $(document).ajaxStart(thats.showLoader).ajaxStop(thats.hideLoader);

                $(".username").text(window.sessionStorage.getItem('username'));
                //$(".privilege").text("Administrator");
                $(".privilege").text(window.sessionStorage.getItem('privilege'));
                //$(".since").text("Apr. 2014");
                $(".bmc-name").text("SPX BMC RR11.5");

                $("#main").addClass("skin-black");

                var ipmi_events_schema = {
                    name: 'ipmi_events',
                    keyPath: 'id',
                    autoIncrement: false,
                    indexes: [{
                        name: 'timestamp'
                    }, {
                        name: 'sensor_number'
                    }, {
                        name: 'sensor_name'
                    }, {
                        name: 'event_direction'
                    }, {
                        name: 'record_type'
                    }, {
                        name: 'system_software_type'
                    }, {
                        name: 'generator_type'
                    }, {
                        name: 'channel_number'
                    }, {
                        name: 'sensor_type'
                    }, {
                        name: 'event_description'
                    }, {
                        name: 'mark_read'
                    }, {
                        name: 'sensor_timestamp',
                        keyPath: ['sensor_number', 'timestamp']
                    }]
                };

                var schema = {
                    stores: [ipmi_events_schema]
                };

                //this.db = new ydn.db.Storage('bmc', schema);

                //this.db.clear();

                //EventLogCollection.on("add", this.addLogs, this);
                //EventLogCollection.on("loaded", this.notify, this);

                var context = this;

                ProjectConfigCollection.fetch({
                    success: function() {
                        sessionStorage.setItem("features", JSON.stringify(ProjectConfigCollection.toJSON()));
                        context.features = ProjectConfigCollection.pluck('feature');
                        ProjectConfigCollection.trigger('features_loaded');
                        context.featureFilter();
                    }
                });

                this.featureFilter = function(){
                    if(!app.features) return;
                    //hide features
                    $("*[data-feature]").each(function(c, el){
                        if(app.features.indexOf($(el).attr("data-feature")) == -1) {
                            $(el).hide();
                        } else {
                            if($(el).hasClass('hide') && !$(el).hasClass('help-item')) { $(el).removeClass('hide'); }
                            $(el).not('.help-item').show();
                        }
                    });

                    $("*[data-not-feature]").each(function(c, el){
                        if(app.features.indexOf($(el).attr("data-not-feature")) != -1) {
                            $(el).hide();
                        } else {
                            if($(el).hasClass('hide') && !$(el).hasClass('help-item')) { $(el).removeClass('hide'); }
                            $(el).not('.help-item').show();
                        }
                    });
                }

                RuntimeConfigCollection.fetch({
                    success: function() {
                        RuntimeConfigCollection.trigger('runtime_features_loaded');
                        context.runtimeFeatureFilter();
                    }
                });

                this.configurations = ProjectConfigCollection;

                this.runtime_configurations = RuntimeConfigCollection;

                this.runtimeFeatureFilter = function(ev) {
                    if(!context.runtime_configurations.length) return;

                    $("*[data-runtime-feature]").each(function(c, el){
                        if(!context.runtime_configurations.isFeatureEnabled($(el).data("runtime-feature"))) {
                            $(el).hide();
                        } else {
                            if($(el).hasClass('hide') && !$(el).hasClass('help-item')) { $(el).removeClass('hide'); }
                            $(el).not('.help-item').show();
                        }
                    });

                    $("*[data-runtime-not-feature]").each(function(c, el){
                        console.log($(el).data("runtime-not-feature"), context.runtime_configurations.isFeatureEnabled($(el).data("runtime-not-feature")));
                        if(context.runtime_configurations.isFeatureEnabled($(el).data("runtime-not-feature"))) {
                            $(el).hide();
                        } else {
                            if($(el).hasClass('hide') && !$(el).hasClass('help-item')) { $(el).removeClass('hide'); }
                            $(el).not('.help-item').show();
                        }
                    });
                }

                this.runtime_configurations.bind("runtime_feature_update", this.runtimeFeatureFilter, this);


                //this.EventLogCollection = EventLogCollection;

                //this.EventLogCollection.fetch();

                //this.SensorCollection = SensorCollection;

                //this.SensorCollection.fetch();

                //this.Chassis = ChassisStatusModel;

                /*this.Chassis.bind("change:power_status", function(model, value){
                    $("#host_status i").removeClass("text-success text-muted");
                    if(value) {
                        $("#host_status i").addClass("text-success");
                        $("#host_status span").html(locale.online);
                    } else {
                        $("#host_status i").addClass("text-muted");
                        $("#host_status span").html(locale.offline);
                    }
                }, this);

                this.Chassis.fetch();*/


                //Use app.poll to start listening on a timer
                //Currently timer events come as 2sec, 5sec, 10sec, 15sec
                //Look below in this file to add more if needed
                //this.poll = _.extend({}, Backbone.Events);

                //Use this events for handling save/error events of generated pages from webui_scaffold package
                //Possible events are save_success, save_error. The handler function for this error receives the view context as first arg
                //The associated model/collection can be traversed from the view context
                this.events = _.extend({}, Backbone.Events);

                this.events.on("save_success", function(){
                    var alertText = $(".alert-success").text().split("---");
                    var str =alertText[0];
                    $(".alert-success").text(str.replace(/[^a-zA-Z0-9.\- ]/g, "").trim());
                    $(".alert-success").removeClass("hide");
                    setTimeout(function(){
                        $(".alert-success").addClass("hide");
                    }, 5000);
                    if(typeof(alertText[1]) != "undefined"){
                        $("body").addClass("disable_a_href");
                        setTimeout(function(){ 
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload(); 
                        },60000);
                        
                    }
                });

                this.events.on("save_error", function(){
                    $(".alert-danger").removeClass("hide");
                    /*setTimeout(function(){
                        $(".alert-danger").addClass("hide");
                    }, 10000);*/
                });
                
                this.events.on("save_success_alert", function(){
                    //alert($(".alert-success").text());
                    var alertText = $(".alert-success").text().split("---");
                    var str =alertText[0];
                    alert(str.replace(/[^a-zA-Z0-9.\- ]/g, "").trim());
                    if(typeof(alertText[1]) != "undefined"){
                        $("body").addClass("disable_a_href");
                        setTimeout(function(){ 
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload(); 
                        },60000);
                        
                    }
                });
                this.events.on("save_error_alert", function(){
                    var alertText = $(".alert-danger").text().split("---");
                    var str =alertText[0];
                    alert(str.replace(/[^a-zA-Z0-9.\- ]/g, "").trim());
                    if(typeof(alertText[1])!="undefined" && alertText[1].indexOf("Logout") > -1){
                        $("body").addClass("disable_a_href");
                        setTimeout(function(){
                            app.logout();
                            document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                            app.router.navigate("login");
                            location.reload();
                        },60000);

                    }
                });


                //this.setPolls();
                
                window.onbeforeunload = function(e) {
                    if(window.cdrom) {
                        return CommonStrings.media_refresh_warning;
                    }
                    else if(window.fw_upgrade) {
                        return CommonStrings.flashing_refresh_warning;
                    }
                    else if(window._video && window._video.running) {
                        window._video.sock.close();
                    }
                    if(window.opener != null) {
                        window.opener.$("#download").removeAttr("disabled");
                    }
                };

                /*db.open({
                    server: 'bmc',
                    version: 1,
                    schema: {
                        ipmi_events: {
                            key: {
                                keyPath: 'id',
                                autoIncrement: false
                            },
                            indexes: {
                                timestamp: {
                                    unique: true
                                },
                                sensor_number: {},
                                sensor_name: {},
                                event_direction: {},
                                record_type: {},
                                system_software_type: {},
                                generator_type: {},
                                channel_number: {},
                                sensor_type: {},
                                event_description: {}
                            }
                        }
                    }
                }).done(function(s) {
                    context.db = s;

                    app.timer = setInterval(function() {
                        EventLogCollection.fetch({
                            success: function() {
                                EventLogCollection.trigger('loaded');
                            }
                        });
                    }, 5000);
                });*/





            },
/*
            addLogs: function(model, collection) {
                //console.log(model);
                // this.db.ipmi_events.add(_.extend(model.toJSON(), {
                //     mark_read: false
                // }));
                this.db.put('ipmi_events', _.extend(model.toJSON(), {
                    mark_read: 0
                }));

                EventLogCollection.trigger('added');

                this.notify();

            },

            notify: function() {
                this.db.from('ipmi_events').where('mark_read', '=', 0)
                    .list()
                    .done(function(results) {
                        if (results.length) {
                            $(".ipmi-log-count").show().removeClass("hide").html(results.length);
                        } else {
                            $(".ipmi-log-count").hide();
                        }
                    });
            },
*/
            showLoader : function(){
                $("#processing_maindiv").show();
            },
            hideLoader : function(){
               $("#processing_maindiv").hide();
            },
            notify: function() {
                /*if(EventLogCollection.length) {
                    $(".ipmi-log-count").show().removeClass("hide").html(EventLogCollection.length);
                } else {
                    $(".ipmi-log-count").hide();
                }*/
            },
            logout: function(callback) {
                app.trigger("beforeLogout");
                app.clearPolls();
                $.ajax({
                    url: "/api/session",
                    type: "DELETE",
                    success: callback,
                    error: function() {
                        callback();
                        throw new Error("Cannot perform a clean logout");
                    }
                });
            },

            blockAnchors: function() {
                $("a").each(function(){
                    if($(this).attr('id') != "start") {
                        $(this).click(function(event) {
                            console.log($(this).attr('id'));
                            event.preventDefault();
                            event.stopPropagation();
                            alert(CommonStrings.flashing_alert);
                        });
                    }
                });
            },

            confirmMediaNavigation: function() {
                $("a").click(function(event) {
                    if(!$(this).parents().hasClass("content") && window.cdrom) {
                        alert(CommonStrings.media_nav_alert);
                        event.preventDefault();
                        event.stopPropagation();
                    }
                });
            },

            cancelConfirm: function() {
                $("a").unbind();
            }, 

            isAdmin: function(){
                console.log("isAdmin function", sessionStorage.privilege_id);
                console.log("isAdmin function", CONSTANTS["ADMIN"]);
                return (sessionStorage.privilege_id == CONSTANTS["ADMIN"]);
            },
            isOperator: function(){
                return (sessionStorage.privilege_id == CONSTANTS["OPERATOR"]);
            },
            isUser: function(){
                return (sessionStorage.privilege_id == CONSTANTS["USER"]);
            },

            isKVM: function(){
                return parseInt(sessionStorage.kvm_access) ? true:false;
            },

            isVMedia: function(){
                return parseInt(sessionStorage.vmedia_access) ? true:false;
            },
			urlPath:function(){
                var protocol= location.protocol;
                var hostname= location.hostname;
                return protocol +"//" + hostname + "/";
            },
            //Hide Page Constraints
            hidePage: function(){
                alert(CommonStrings.non_privilege_user_error_message);
                setTimeout(function() {
                        //app.useLayout("layouts/main");
                    app.router.navigate("dashboard", {
                        trigger: true
                    });
                }, 500);
            },
            disableAllbutton: function(){
                $(".content .btn").each(function(){
                    $(this).addClass("disable_a_href");
                });
            },
			disableAllinput: function(){
                $("input").attr('disabled','disabled');
            },
            disableAlldropdown: function(){
                $("select").attr('disabled','disabled');
            }

        };


        app.HTTPErrorHandler = HTTPErrorHandler;

        //Templates
        window.JST = {};
        //window.JST["app/templates/layouts/main.html"] = _.template(MainTemplate);
        //window.JST["app/templates/layouts/login.html"] = _.template(LoginTemplate);
		window.JST["app/templates/layouts/withoutmenu.html"] = _.template(WithoutMenuTemp);

        // Localize or create a new JavaScript Template object.
        var JST = window.JST = window.JST || {};

        // Configure LayoutManager with Backbone Boilerplate defaults.
        Layout.configure({
            // Allow LayoutManager to augment Backbone.View.prototype.
            manage: true,

            prefix: "app/templates/",

            fetchTemplate: function(path) {
                // Initialize done for use in async-mode
                var done;
                // Concatenate the file extension.
                path = path + ".html";

                // If cached, use the compiled template.
                if (JST[path]) {
                    return JST[path];
                } else {
                    // Put fetch into `async-mode`.
                    done = this.async();

                    // Seek out the template asynchronously.
                    return $.ajax({
                        url: app.root + path
                    }).then(function(contents) {
                        done(JST[path] = _.template(contents));
                    });
                }
            }
        });

        // Mix Backbone.Events, modules, and layout management into the app object.
        return _.extend(app, {
                // Create a custom object with a nested Views object.
                module: function(additionalProps) {
                    return _.extend({
                        Views: {}
                    }, additionalProps);
                },

                layout: new Layout({}),

                // Helper for using layouts.
                useLayout: function(name, options) {
                    // If already using this Layout, then don't re-inject into the DOM.
                    if (this.layout && this.layout.options.template === name) {
                        return this.layout;
                    }

                    // If a layout already exists, remove it from the DOM.
                    if (this.layout) {
                        this.layout.remove();
                    }

                    var _app = this;

                    // Create a new Layout with options.
                    var layout = new Layout(_.extend({
                        template: name,
                        //className: "layout " + name,
                        //id: "layout",

                        afterRender: function() {

                            console.log("doing layout after render");

                            //$("[data-toggle='dropdown']").dropdown();

                            //$("body").addClass("fixed");

                            //TODO : Fix any leaks
                            //Enable sidebar toggle
                            /*$("[data-toggle='offcanvas']").click(function(e) {
                                e.preventDefault();
                                //If window is small enough, enable sidebar push menu
                                if ($(window).width() <= 992) {
                                    if (!$('.row-offcanvas').hasClass("relative")) {
                                        $('.row-offcanvas').addClass("relative");
                                    }

                                    $('.row-offcanvas').toggleClass('active');
                                    $('.left-side').removeClass("collapse-left");
                                    $(".right-side").removeClass("strech");
                                } else {
                                    //Else, enable content streching
                                    $('.left-side').toggleClass("collapse-left");
                                    $(".right-side").toggleClass("strech");
                                }
                            });*/

                            /*
                             * ADD SLIMSCROLL TO THE TOP NAV DROPDOWNS
                             * ---------------------------------------
                             */
                            /*$(".navbar .menu").slimscroll({
                                height: "200px",
                                alwaysVisible: false,
                                size: "3px"
                            }).css("width", "100%");*/

                            //Set sidebar tree
                            //$(".sidebar .treeview").tree();

                            _app._fix();
                            //Fire when wrapper is resized
                            $(".wrapper").resize(function() {
                                console.log("resizing...");
                                _app._fix();
                            });

                            //Fix for charts under tabs
                            $('.box ul.nav a').on('shown.bs.tab', function(e) {
                                area.redraw();
                                donut.redraw();
                            });

                            //Add slimscroll
                            /*$(".sidebar").slimscroll({
                                height: ($(window).height() - $(".header").height()) + "px",
                                color: "#FFF",
                                railVisible: true,
                                railColor: '#000000'
                            });*/

                            /*$("#live-poll").click(function(ev){
                                ev.stopPropagation();
                                ev.preventDefault();

                                if($(this).data("poll")=="on"){
                                    $(this).data("poll", "off");
                                  //  $(this).find("span").html("off");
                                    $(this).find("i").css("color", "gray");
                                    _app.clearPolls();
                                    _app.manualStopPoll = true;
                                } else {
                                    $(this).data("poll", "on");
                                //    $(this).find("span").html("on");
                                    $(this).find("i").css("color", "deepskyblue");
                                    _app.setPolls();
                                    _app.manualStopPoll = false;
                                }
                            })*/

                            /*$("#refreshPage").click(function(ev){
                                ev.stopPropagation();
                                ev.preventDefault();
                                var refreshURL = location.hash;
                                console.log(refreshURL);
                                if(refreshURL.indexOf("#settings")>-1){
                                    location.reload();
                                }else{
                                    if(Backbone.history.fragment == "logout"){
                                        window.history.back();
                                    }
                                    if(Backbone.history.fragment == "remote-control"){
                                        alert(CommonStrings.disable_refresh);
                                    }
                                    else{
                                        Backbone.history.loadUrl(Backbone.history.fragment);
                                    }
                                }
                            })*/

                        }
                    }, options));

                    // Insert into the DOM.
                    $("#main").empty().append(layout.$el);

                    // Render the layout.
                    layout.render();

                    // Cache the refererence.
                    this.layout = layout;

                    // Return the reference, for chainability.
                    return layout;
                },

                /* 
                 * Make sure that the sidebar is streched full height
                 * ---------------------------------------------
                 * We are gonna assign a min-height value every time the
                 * wrapper gets resized and upon page load. We will use
                 * Ben Alman's method for detecting the resize event.
                 **/
                //alert($(window).height());
                _fix: function() {
                    //Get window height and the wrapper height
                    var height = $(window).height() - $("body .header").height();
                    $(".wrapper").css("min-height", height + "px");
                    var content = $(".wrapper").height();
                    //If the wrapper height is greater than the window
                    if (content > height)
                    //then set sidebar height to the wrapper
                        $(".left-side, html, body").css("min-height", content + "px");
                    else {
                        //Otherwise, set the sidebar to the height of the window
                        $(".left-side, html, body").css("min-height", height + "px");
                    }
                },
            },

            Backbone.Events);

    });
