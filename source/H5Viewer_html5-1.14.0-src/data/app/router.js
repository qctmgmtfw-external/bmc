define(["views/RemoteControlView", "views/remotePopUpView", "app", "resize", "slimscroll"],

    function(RemoteControlView,
        RemotePopUpView,
        app) {
        "use strict";


        // Defining the application router.
        var Router = Backbone.Router.extend({

            routes: {
                "remote-control": "remoteControl",
                "remote": "RemoteControlPopUpView",
                "": "default"
            },

            default: function() {
                /*this.navigate("dashboard", {
                    trigger: true
                });
                console.log("Moving to dashboard");*/

                console.log("KVM default-screen");

                app.useLayout("layouts/withoutmenu");

                app.layout.setView(".center-side", new RemoteControlView()).render();

                setTimeout(function(){$("#toggle").trigger("click");}, 3000);//Quanta
            },

            login: function() {
                app.useLayout("layouts/login");
                app.layout.setView(".login-view", new LoginView()).render();
            },

            logout: function() {
                if (confirm(locale.logout_confirm_msg)) {
                    app.logout(function() {
                        document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                        app.router.navigate("login");
                        location.reload();
                    });
                } else {
                    if (Backbone.history.fragment == "logout") {
                        window.history.back();
                    } else {
                        Backbone.history.loadUrl(Backbone.history.fragment);
                    }
                }
            },
            RemoteControlPopUpView: function() {
                this.preSwitch();
                this.setView(RemotePopUpView);
                this.postSwitch();
            },

            remoteControl: function() {
                // this.preSwitch();
                //app.clearPolls();
                //this.setView(RemoteControlView);
                //this.postSwitch();
                //opening the KVM in without header.menus etc..

                
                app.useLayout("layouts/withoutmenu");
                app.layout.setView(".center-side", new RemoteControlView()).render();
            },
            preSwitch: function() {
                if (app.lastActiveView) {
                    if (app.lastActiveView.timer) clearInterval(app.lastActiveView.timer);
                    app.lastActiveView.trigger("close");
                }

                if (window._video && window._video.running) {
                    window._video.sock.close();
                }

                if (!document.cookie) {
                    /*this.navigate("login", {
                        trigger: true
                    });*/
                     
                    app.router.navigate("#remote-control");
                    location.reload();
                    throw new Error("Not Authorized");
                } else {
                    if (!sessionStorage.getItem("garc") || sessionStorage.getItem("garc") == null) {
                        app.logout();
                        document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                        app.router.navigate("login");
                        location.reload();
                    }
                }

                if (!app.afterLoginInit) {
                    app.afterLoginInit = true;
                    //app.layout.setView(".notifications-menu", new NotificationsView()).render();
                    //app.layout.setView(".messages-menu", new MessagesView()).render();
                    //new UserTasksView().render().$el.prependTo(".navbar-nav");
                    //new NotificationsView().render().$el.prependTo(".navbar-nav");
                    //new MessagesView().render().$el.prependTo(".navbar-nav");

                    //app.initPollingEventHandlers();

                }
            },

            postSwitch: function() {
                $('.sidebar-menu li').removeClass('active');
                $("a[href='" + location.hash + "']").parentsUntil('.sidebar-menu', 'li').addClass('active');

                app.featureFilter();

                app.runtime_configurations.fetch({
                    success: function() {
                        app.runtimeFeatureFilter();
                    }
                });

                //check and close log tree 
                if (location.hash.indexOf('#logs') == -1) {
                    if ($("a[href='#logs']").find(".fa-angle-down").length !== 0) {
                        $("a[href='#logs']").click();
                    }
                }
                if ($(window).width() <= 992) {
                    if ($(".row-offcanvas-left").hasClass("active")) {
                        $("[data-toggle='offcanvas']").click();
                    }
                }
            },

            setView: function(View) {
                var view = new View();

                app.layout.setView(".right-side", view).render();
                app.lastActiveView = view;
                /*if (this.loaded !== true) {
                        this.loaded = true;
                    } else {
                        app.layout.setView(".right-side", new View()).render();
                    }*/
            }

        });

        return Router;
    });
