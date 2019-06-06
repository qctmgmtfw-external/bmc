define(["views/DashboardView", "views/LoginView", "views/EventLogView", "views/SystemLogView", "views/AuditLogView","views/VideoLogView", "views/SensorView", "views/SensorDetailsView", "views/FRUView","views/PostCodeView", "views/IdentifyView","views/SettingsView", "views/RemoteControlView", "views/PowerControlView", "views/TaskEventsView", "views/TaskDetailsView", "views/ScriptsView", "views/StoreView", "views/UpdatesView", "views/HelpView",
"views/MessagesView", "views/NotificationsView", "views/UserTasksView", "views/maintenance/FirmwareUpgradeView","views/maintenance/dualFirmwareUpgradeView","views/maintenance/dual_image_config", "views/maintenance/HpmFirmwareUpdateView", "views/maintenance/BiosUpgradeView","views/settings/network/dns", "views/image_redirection/lmedia", "views/image_redirection/rmedia", "views/settings/pef/event_filters_edit_item", "views/settings/pef/event_filters","views/settings/pef/lan_destinations_edit_item", "views/settings/pef/lan_destinations", "views/settings/smtp", "views/settings/date_time.js", "views/settings/sasit_management/sasit_topology_info_edit_item", "views/settings/sasit_management/sasit_topology_info", "views/settings/raid_management/logical_device_info_edit_item", "views/settings/raid_management/logical_device_info","views/settings/raid_management/physical_device_info_edit_item", "views/settings/raid_management/physical_device_info", "views/settings/raid_management/create_logical_device", "views/settings/raid_management/raid_logical_physical_device_info_edit_item", "views/settings/raid_management/raid_logical_physical_device_info","views/system_inventory_info_edit_item", "views/system_inventory_info", "views/settings/users_edit_item", "views/settings/users", "views/settings/service_sessions", "views/settings/services_edit_item", "views/settings/services", "views/settings/network/bond", "views/settings/video/auto/pre_event", "views/settings/video/sol/sol_recorded_video", "views/settings/video/sol/sol_trigger_settings", "views/settings/video/auto/trigger_settings","views/settings/raid_management/raid_event_log", "views/settings/network/ip", "views/settings/media/general","views/remotePopUpView", "views/settings/video/sol/javasol", "app", "resize", "treeview", "slimscroll"],

function(DashboardView,
LoginView,
EventLogView,
SystemLogView,
AuditLogView,
VideoLogView,
SensorView,
SensorDetailsView,
FRUView,
PostCodeView,
IdentifyView,
SettingsView,
RemoteControlView,
PowerControlView,
TaskEventsView,
TaskDetailsView,
ScriptsView,
StoreView,
UpdatesView,
HelpView,
MessagesView,
NotificationsView,
UserTasksView,
FirmwareUpgradeView,
dualFirmwareUpgradeView,
DualImageconfigView,
HpmFirmwareUpdateView,
BiosUpgradeView,
DnsSettingsView,
LmediaView,
RmediaView,
EventFiltersEditItemView,
EventFiltersView,
LanDestinationEditItemView,
LanDestinationView,
SMTPSettingsView,
DateTimeSettingsView,
SasitTopologyInfoEditItemView,
SasitTopologyInfoView,
LogicalDeviceInfoEditItemView,
LogicalDeviceInfoView,
PhysicalDeviceInfoEditItemView,
PhysicalDeviceInfoView,
CreateLogicalDeviceView,
RaidLogicalPhysicalDeviceInfoEditItemView,
RaidLogicalPhysicalDeviceInfoView,
SystemInventoryInfoEditItemView,
SystemInventoryInfoView,
UsersEditItemView,
UsersView,
ServiceSessionsView,
ServicesEditItemView,
ServicesView,
BondView,
PreEventView,
SolRecordedVideoView,
SolTriggerSettingsView,
TriggerSettingsView,
RaidEventLogView,
IpView,
GeneralView,
RemotePopUpView,
JavasolView,
app) {
    "use strict";


    // Defining the application router.
    var Router = Backbone.Router.extend({

        routes: {
            "login": "login",
            "logout": "logout",
            "dashboard": "dashboard",
            "logs/event-log": "eventLog",
            "logs/event-log/:filter": "eventLog",
            "logs/system-log": "systemLog",
            "logs/audit-log": "auditLog",
            "logs/video-log": "videoLog",
            "sensors": "sensors",
            "sensors/:id": "sensorDetails",
            "fru": "fru",
            "postcode": "postcode",
            "identify": "identify",
            "settings": "settings",
            "remote-control": "remoteControl",
            "power-control": "powerControl",
            "tasks": "tasks",
            "tasks/:id": "taskDetails",
            "scripts": "scripts",
            "store": "store",
            "updates": "updates",
            "help": "help",
            "maintenance/firmware_update": "MaintenanceFirmwareUpgrade",
            "maintenance/firmware_upgrade": "MaintenanceFirmwareUpgrade",
	        "maintenance/bios_update": "MaintenanceBiosUpgrade",
            "maintenance/dual_firmware_update": "MaintenancedualFirmwareUpgrade",
            "maintenance/dual_firmware_upgrade": "MaintenancedualFirmwareUpgrade",
            "maintenance/dual_image_config": "MaintenancedualImageConfig",
    	    "maintenance/hpm_firmware_update": "HpmFirmwareUpdate",
            "settings/network/dns": "DnsSettings",
	    "image_redirection/lmedia": "ImageRedirectionLmedia",
            "image_redirection/rmedia": "ImageRedirectionRmedia",
            "settings/pef/event_filters/edit/:filter": "EventFiltersEditItem",
            "settings/pef/event_filters": "SettingsPefEventFilters",
            "settings/pef/lan_destinations/edit/:filter": "LanDestinationEditItem",
            "settings/pef/lan_destinations": "SettingsLanDestinations",
            "settings/smtp": "SMTPSettings",
            "settings/date_time": "DateTimeSettings",
	    "settings/sasit_management/sasit_topology_info/edit/:id": "SasitTopologyInfoEditItem",
            "settings/sasit_management/sasit_topology_info": "SettingsSasitManagementSasitTopologyInfo",
	    "settings/raid_management/logical_device_info/edit/:id": "LogicalDeviceInfoEditItem",
            "settings/raid_management/logical_device_info": "SettingsRaidManagementLogicalDeviceInfo",
            "settings/raid_management/physical_device_info/view/:id": "PhysicalDeviceInfoEditItem",
            "settings/raid_management/physical_device_info": "SettingsRaidManagementPhysicalDeviceInfo",
	    "settings/raid_management/create_logical_device": "SettingsRaidManagementCreateLogicalDevice",
            "settings/raid_management/raid_logical_physical_device_info/:id": "RaidLogicalPhysicalDeviceInfoEditItem",
            "settings/raid_management/raid_logical_physical_device_info": "SettingsRaidManagementRaidLogicalPhysicalDeviceInfo",
	    "/system_inventory_info/edit/:id": "SystemInventoryInfoEditItem",
            "system_inventory_info": "SystemInventoryInfo",
            "settings/users/edit/:id": "UsersEditItem",
            "settings/users": "SettingsUsers",
            "settings/service_sessions": "SettingsServiceSessions",
            "settings/service_sessions/:filter": "SettingsServiceSessions",
            "settings/services/edit/:id": "ServicesEditItem",
            "settings/services": "SettingsServices",
            "settings/network/bond": "SettingsNetworkBond",
	    "settings/video/auto/pre_event": "SettingsVideoAutoPreEvent",
	    "settings/video/sol/sol_recorded_video": "SettingsVideoSolSolRecordedVideo",
	    "settings/video/sol/sol_trigger_settings": "SettingsVideoSolSolTriggerSettings",
	    "settings/video/auto/trigger_settings": "SettingsVideoAutoTriggerSettings",
	    "settings/raid_management/raid_event_log": "SettingsRaidManagementRaidEventLog",
	    "settings/network/ip": "SettingsNetworkIp",
	    "settings/media/general": "SettingsMediaGeneral",
	    "remote":"RemoteControlPopUpView",
	    "settings/video/sol/javasol": "SettingsMediaJavasol",
            "": "default"
        },

        default: function() {
            this.navigate("dashboard", {
                trigger: true
            });
            console.log("Moving to dashboard");
        },

        login: function() {
            app.useLayout("layouts/login");
            app.layout.setView(".login-view", new LoginView()).render();
        },

        logout: function() {
            if (confirm(locale.logout_confirm_msg)) {
                app.logout(function() {
                    document.cookie = "refresh_disable=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                    document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                    app.router.navigate("login");
                    location.reload();
                });
            }else{
                if(Backbone.history.fragment == "logout"){
                    window.history.back();
                }else{
                    Backbone.history.loadUrl(Backbone.history.fragment);
                }
            }
        },

        SettingsMediaGeneral: function() {
            this.preSwitch();
            this.setView(GeneralView);
            this.postSwitch();
        },
		 RemoteControlPopUpView: function() {
            this.preSwitch();
            this.setView(RemotePopUpView);
            this.postSwitch();
        },

	SettingsMediaJavasol: function() {
            this.preSwitch();
            this.setView(JavasolView);
            this.postSwitch();
        },

        SettingsNetworkIp: function() {
            this.preSwitch();
            this.setView(IpView);
            this.postSwitch();
        },

        SettingsNetworkBond: function() {
            this.preSwitch();
            this.setView(BondView);
            this.postSwitch();
        },

		UsersEditItem: function(id) {
            this.preSwitch();
            this.setView(UsersEditItemView);
            this.postSwitch();
        },
        SettingsUsers: function() {
            this.preSwitch();
            this.setView(UsersView);
            this.postSwitch();
        },
        SettingsServiceSessions: function() {
            this.preSwitch();
            this.setView(ServiceSessionsView);
            this.postSwitch();
        },
        ServicesEditItem: function(id) {
            this.preSwitch();
            this.setView(ServicesEditItemView);
            this.postSwitch();
        },
        SettingsServices: function() {
            this.preSwitch();
            this.setView(ServicesView);
            this.postSwitch();
        },
        HpmFirmwareUpdate: function() {
            this.preSwitch();
            app.clearPolls();
            this.setView(HpmFirmwareUpdateView);
            this.postSwitch();
        },

        MaintenanceFirmwareUpgrade: function() {
            this.preSwitch();
            app.clearPolls();
            this.setView(FirmwareUpgradeView);
            this.postSwitch();
        },//Quanta
        MaintenanceBiosUpgrade: function() {
            this.preSwitch();
            app.clearPolls();
            this.setView(BiosUpgradeView);
            this.postSwitch();
        },
        MaintenancedualFirmwareUpgrade: function() {
            this.preSwitch();
            app.clearPolls();
            this.setView(dualFirmwareUpgradeView);
            this.postSwitch();
        },
        MaintenancedualImageConfig: function() {
            this.preSwitch();
            app.clearPolls();
            this.setView(DualImageconfigView);
            this.postSwitch();
        },
        DnsSettings: function() {
            this.preSwitch();
            this.setView(DnsSettingsView);
            this.postSwitch();
        },

		ImageRedirectionLmedia: function() {
            this.preSwitch();
            this.setView(LmediaView);
            this.postSwitch();
        },

        ImageRedirectionRmedia: function() {
            this.preSwitch();
            this.setView(RmediaView);
            this.postSwitch();
        },

        EventFiltersEditItem: function(filter) {
            this.preSwitch();
            this.setView(EventFiltersEditItemView);
            this.postSwitch();
        },
        SettingsPefEventFilters: function() {
            this.preSwitch();
            this.setView(EventFiltersView);
            this.postSwitch();
        },
        LanDestinationEditItem: function(filter) {
            this.preSwitch();
            this.setView(LanDestinationEditItemView);
            this.postSwitch();
        },
        SettingsLanDestinations: function() {
            this.preSwitch();
            this.setView(LanDestinationView);
            this.postSwitch();
        },
        SMTPSettings: function() {
            this.preSwitch();
            this.setView(SMTPSettingsView);
            this.postSwitch();
        },
        DateTimeSettings: function() {
            this.preSwitch();
            this.setView(DateTimeSettingsView);
            this.postSwitch();
        },
		SasitTopologyInfoEditItem: function(id) {
            this.preSwitch();
            this.setView(SasitTopologyInfoEditItemView);
            this.postSwitch();
        },
        SettingsSasitManagementSasitTopologyInfo: function() {
            this.preSwitch();
            this.setView(SasitTopologyInfoView);
            this.postSwitch();
        },
		LogicalDeviceInfoEditItem: function(id) {
            this.preSwitch();
            this.setView(LogicalDeviceInfoEditItemView);
            this.postSwitch();
        },
        SettingsRaidManagementLogicalDeviceInfo: function() {
            this.preSwitch();
            this.setView(LogicalDeviceInfoView);
            this.postSwitch();
        },
		PhysicalDeviceInfoEditItem: function(id) {
            this.preSwitch();
            this.setView(PhysicalDeviceInfoEditItemView);
            this.postSwitch();
        },
        SettingsRaidManagementPhysicalDeviceInfo: function() {
            this.preSwitch();
            this.setView(PhysicalDeviceInfoView);
            this.postSwitch();
        },
		SettingsRaidManagementCreateLogicalDevice: function() {
            this.preSwitch();
            this.setView(CreateLogicalDeviceView);
            this.postSwitch();
        },
		RaidLogicalPhysicalDeviceInfoEditItem: function(id) {
            this.preSwitch();
            this.setView(RaidLogicalPhysicalDeviceInfoEditItemView);
            this.postSwitch();
        },
        SettingsRaidManagementRaidLogicalPhysicalDeviceInfo: function() {
            this.preSwitch();
            this.setView(RaidLogicalPhysicalDeviceInfoView);
            this.postSwitch();
        },
        SettingsRaidManagementRaidEventLog: function() {
            this.preSwitch();
            this.setView(RaidEventLogView);
            this.postSwitch();
        },
        SystemInventoryInfoEditItem: function(id) {
            this.preSwitch();
            this.setView(SystemInventoryInfoEditItemView);
            this.postSwitch();
        },
        SystemInventoryInfo: function() {
            this.preSwitch();
            this.setView(SystemInventoryInfoView);
            this.postSwitch();
        },
        dashboard: function(event) {
            this.preSwitch();

            this.setView(DashboardView);

            /*switch (event) {
                    case "event-month":

                        break;
                    case "event-week":

                        break;
                }
*/
            this.postSwitch();
            console.log("dashboard router done");
            //this.navigate("user-login", {trigger: true});
        },

        eventLog: function(filter) {
            this.preSwitch();
            this.setView(EventLogView);
            this.postSwitch();
        },
        systemLog: function() {
            this.preSwitch();
            this.setView(SystemLogView);
            this.postSwitch();
        },
        auditLog: function() {
            this.preSwitch();
            this.setView(AuditLogView);
            this.postSwitch();
        },
        videoLog: function() {
            this.preSwitch();
            this.setView(VideoLogView);
            this.postSwitch();
        },

        sensors: function() {
            this.preSwitch();
            this.setView(SensorView);
            this.postSwitch();
        },

        sensorDetails: function(id) {
            this.preSwitch();
            this.setView(SensorDetailsView);
            this.postSwitch();
        },

        fru: function() {
            this.preSwitch();
            this.setView(FRUView);
            this.postSwitch();
        },

        postcode: function() {
            this.preSwitch();
            this.setView(PostCodeView);
            this.postSwitch();
        },

        identify: function() {
            this.preSwitch();
            this.setView(IdentifyView);
            this.postSwitch();
        },

        settings: function() {
            this.preSwitch();
            this.setView(SettingsView);
            this.postSwitch();
        },

        remoteControl: function() {
           // this.preSwitch();
            app.clearPolls();
            //this.setView(RemoteControlView);
            //this.postSwitch();
			 //opening the KVM in without header.menus etc..
            app.useLayout("layouts/withoutmenu");
            app.layout.setView(".center-side", new RemoteControlView()).render();
        },

        powerControl: function() {
            this.preSwitch();
            this.setView(PowerControlView);
            this.postSwitch();
        },

        tasks: function() {
            this.preSwitch();
            this.setView(TaskEventsView);
            this.postSwitch();
        },

        taskDetails: function(id) {
            this.preSwitch();
            this.setView(TaskDetailsView);
            this.postSwitch();
        },

        scripts: function() {
            this.preSwitch();
            this.setView(ScriptsView);
            this.postSwitch();
        },

        store: function() {
            this.preSwitch();
            this.setView(StoreView);
            this.postSwitch();
        },

        updates: function() {
            this.preSwitch();
            this.setView(UpdatesView);
            this.postSwitch();
        },

        help: function() {
            this.preSwitch();
            this.setView(HelpView);
            this.postSwitch();
        },
	SettingsVideoAutoPreEvent: function() {
            this.preSwitch();
            this.setView(PreEventView);
            this.postSwitch();
        },
	SettingsVideoSolSolRecordedVideo: function() {
            this.preSwitch();
            this.setView(SolRecordedVideoView);
            this.postSwitch();
        },
	SettingsVideoAutoTriggerSettings: function() {
            this.preSwitch();
            this.setView(TriggerSettingsView);
            this.postSwitch();
        },
	SettingsVideoSolSolTriggerSettings: function() {
            this.preSwitch();
            this.setView(SolTriggerSettingsView);
            this.postSwitch();
        },
        preSwitch: function() {
            if (app.lastActiveView) {
                if (app.lastActiveView.timer) clearInterval(app.lastActiveView.timer);
                app.lastActiveView.trigger("close");
            }

            if(!app.polling && !app.manualStopPoll) {
                app.setPolls();
            }

            if(window._video && window._video.running) {
                window._video.sock.close();
            }

            if (!document.cookie) {
                /*this.navigate("login", {
                    trigger: true
                });*/
                app.router.navigate("login");
                location.reload();
                throw new Error("Not Authorized");
            }/*else{
                if(!sessionStorage.getItem("garc") || sessionStorage.getItem("garc") == null){
                    app.logout();
                    document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                    app.router.navigate("login");
                    location.reload();
                }
            }*/

            if (!app.afterLoginInit) {
                app.afterLoginInit = true;
                //app.layout.setView(".notifications-menu", new NotificationsView()).render();
                //app.layout.setView(".messages-menu", new MessagesView()).render();
                //new UserTasksView().render().$el.prependTo(".navbar-nav");
                new NotificationsView().render().$el.prependTo(".navbar-nav");
                new MessagesView().render().$el.prependTo(".navbar-nav");

                app.initPollingEventHandlers();

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
