define(['jquery', 'underscore', 'backbone','app',
        'models/app_config',
        'models/user_config',
        'collection/audit-log',
        'models/system_info',
        'collection/ip',
      'text!templates/dashboard/stats.html','i18n!strings/nls/stats',
      'moment'],
    function($, _, Backbone, app ,AppConfigModel, UserConfigModel, AuditLogCollection, SystemInfoModel, IpCollection,StatsTemplate, CommonStrings,moment) {

        var view = Backbone.View.extend({

        	systemtime : 0,
        	browsertime: 0,
            offset:0,

            template: _.template(StatsTemplate),

            initialize: function() {

                this.acm = AppConfigModel;
                this.acm.bind("change", this.updateUptime, this);

                this.ucm = UserConfigModel;
                this.ucm.bind("change:tasks", this.updateTasks, this);

                this.alc = AuditLogCollection;
                this.alc.bind("add change reset remove", this.updateAuditLog, this);

                this.sim = SystemInfoModel;
                this.sim.bind("add change reset remove", this.getFwVersion, this);

                this.im = IpCollection;

                this.getTime();
                this.$("#cmcversion").hide();
            },
            getNetworkInfo: function(collection){
                collection.each(function(model){
                    var ipv4mode = model.get('ipv4_dhcp_enable');
                    var ipv6mode = model.get('ipv6_dhcp_enable');
                    var macaddress = model.get('mac_address');
                    var ipv4address = model.get('ipv4_address');
                    var ipv6address = model.get('ipv6_address').split(" ");
                    var ipv6address_output = "<ul>";
                    var index = 0;
                    if (ipv6mode == 1)
                    	index = 3;
                    else
                    	index = 16;
                    console.log("index "+ index +" ipv6mode " + ipv6mode);
                    for(var i = 1; i<=index;i++)
                    {
                        if(ipv6address[i]!= "::")
                        {
				ipv6address_output+="<li>"+(i-1)+" : "+ipv6address[i]+"</li>";
                        }
                    }
                    ipv6address_output+="</ul>";
                    if(ipv4mode == 0)
                        $("#ipv4mode").html(" Static");
                    else if(ipv4mode == 1)
                        $("#ipv4mode").html(" DHCP");
                    else
                        $("#ipv4mode").html(" Unknown");

                    if(ipv6mode == 0)
                        $("#ipv6mode").html(" Static");
                    else if(ipv6mode == 1)
                        $("#ipv6mode").html(" DHCP");
                    else
                        $("#ipv6mode").html(" Disable");

                    $("#macaddress").html(" "+macaddress);
                    $("#ipv4address").html(" "+ipv4address);
                    $("#ipv6linnkaddress").html(" "+ipv6address[0]);
                    $("#ipv6address").html(" "+ipv6address_output);
                });
            },
            getSysmac: function(){
                $.ajax({
                    url: "/api/system-mac",
                    type: "GET",
                    success: function(response)
                    {

                        var output="<ul>";
                        if(response)
                        {
                            for (var i = 0; i< response.length;i++)
                            {
                                var j = i+1;
                                output+="<li>"+j+" :"+response[i].lan_card_type + " : " + response[i].mac + "</li>";
                            }
                        }
                        output+="</ul>";

                        $('#sysmac').html(output);
                    }, dataType: "json"});
            },
            getTime: function(){
                view.time_range = 0;
                var timerid = -1;
                function setPoll(time){
                    if(timerid == -1)
                    {
                        $.ajax({
                            url: "/api/system-time",
                            type: "GET",
                            success: function(data){
                                systemtime = data.currenttime;
                                browsertime = Math.ceil(Date.now()/1000);
                                offset = data.utc_offset;
                                syncTime();
                        }, dataType: "json"});
                    }
                }
                function timeConverter(UNIX_timestamp){
                  var a = new Date(UNIX_timestamp * 1000);
                  var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
                  var year = a.getUTCFullYear();
                  var month = months[a.getMonth()];
                  var date = a.getUTCDate();
                  var hour = a.getUTCHours();
                  var min = a.getUTCMinutes();
                  var sec = a.getUTCSeconds();
                  if (sec < 10) {
                	  sec = "0" + sec;
                  }
                  if (min < 10) {
                	  min = "0" + min;
                  }
                  if (hour < 10) {
                	  hour = "0" + hour;
                  }
                  var time = date + ' ' + month + ' ' + year + ' ' + hour + ':' + min + ':' + sec ;
                  return time;
                }
                function syncTime(){
                    var currenttime = Math.ceil(Date.now()/1000);
                    if(timerid != -1)
                        clearTimeout(timerid);
                    timerid = setTimeout(function () {syncTime()}, 1000);
                    $("#time").html("BMC Date&Time : "+timeConverter(systemtime+currenttime-browsertime+offset));
                    //$("#time").html(""+timeConverter(currenttime));
                }
                setPoll();
            },
            close: function() {
                    clearTimeout(this.timerid);
            },
            reload: function(model, value) {
                clearTimeout(this.timerid);
            },
            events: {
            },
            beforeRender: function() {
                var that = this;
               /* $.getJSON("https://www.megarac.com/store/spx/ae/update.php?callback=?", "last_script_id=0", function(res) {
                    that.$("#new-ae-scripts-count").html(res.count);
                });*/
              if(app.configurations.isFeatureAvailable("AUTOMATION")) {
                    $.ajax({
                      url: "https://www.megarac.com/store/spx/ae/update.php?callback=?",
                      dataType: 'json',
                      global:false,
                      data: "last_script_id=0",
                      success: function(res) {
                          that.$("#new-ae-scripts-count").html(res.count);
                      },
          		      error : function(){
          		      }
                    });
                }
            },

            afterRender: function() {
                var that = this;
                AppConfigModel.fetch({
                    success: function(){
                        that.updateUptime.call(that, AppConfigModel);
                    }
                });
                //UserConfigModel.fetch();
                AuditLogCollection.fetch({
                    success: function(){
                        that.updateAuditLog.call(that);
                    }
                });
                SystemInfoModel.fetch(
                  {
                    success: function(){
                        that.getFwVersion.call(that,SystemInfoModel);
                    }
                  });

                  this.im.fetch(
                    {
                      success: function(){
                          that.getNetworkInfo.call(that,IpCollection);
                      }
                    });

                this.acm.trigger("change", this.acm, this.acm.get('minutes_per_count'));
                //this.ucm.trigger("change:tasks", this.ucm, this.ucm.get('tasks'));
                this.alc.trigger("change", this.alc, this.alc.get('id'));
                this.sim.trigger("change", this.sim, this.sim.get('fw_ver'));
                this.im.trigger("change", this.im, this.im.get('ipv4_dhcp_enable'));
                this.getSysmac();
            },

            updateUptime: function(model, value) {

                var hours = ((model.get('minutes_per_count')*model.get('poh_counter_reading'))/60)%24;
                var days = Math.floor((model.get('minutes_per_count')*model.get('poh_counter_reading')/60)/24);
                if(isNaN(hours) && isNaN(days)) {
                    hours = 0; days = 0
                }
                this.$("#uptime").html(days+ " d "+hours).next().html(hours==1?"hr":"hrs");
            },
            getFwVersion:function(model,value){
              //Test
              var firmwareVersion = model.get('fw_ver');
              var builddate = model.get('date');
              var buildtime = model.get('time');
              var bios_version = model.get('bios_version');
              var lanport = model.get('lanport');
              var chipset = model.get('chipset');
              var hostname = model.get('host_name');
              var fantableversion = model.get('fan_table');
              var cmc_support = model.get('cmc_support');
              var cmc_fw_version = model.get('cmc_version');
              this.$("#fwversion").html(" "+firmwareVersion);
              this.$("#buildtime").html(" "+builddate+" "+buildtime);
              this.$("#biosversion").html(" "+bios_version);
	          this.$("#hostname").html(" "+hostname);
              this.$("#fantableversion").html(" "+fantableversion);
              if(cmc_support)
              {
                  this.$("#cmc_support").show();
                  this.$("#cmcversion").html("PDB Version : <b>"+cmc_fw_version+"</b>");
              }

              switch(lanport)
              {
                  case 0x00:
				    this.$("#lanport").html(" Dedicated-NIC");
				    break;
    			  case 0x01:
                    this.$("#lanport").html(" Shared-NIC");
    				break;
    			case 0x02:
                    this.$("#lanport").html(" Shared-NIC");
    				break;
    			case 0x03:
                    this.$("#lanport").html(" Shared-NIC");
    				break;
    			default:
                    this.$("#lanport").html(" N/A");
    				break;
              }
              switch(chipset)
              {
                  case 24:
                    this.$("#chipset").html(" AST2500(A0)");
                  break;
                  case 25:
                    this.$("#chipset").html(" AST2500(A1)");
                  break;
                  case 26:
                    this.$("#chipset").html(" AST2510(A0)");
                  break;
                  case 27:
                    this.$("#chipset").html(" AST2510(A1)");
                  break;
                  case 28:
                    this.$("#chipset").html(" AST2520(A0)");
                  break;
                  case 29:
                    this.$("#chipset").html(" AST2520(A1)");
                  break;
                  case 32:
                    this.$("#chipset").html(" AST2500(A2)");
                  break;
                  case 33:
                    this.$("#chipset").html(" AST2510(A2)");
                  break;
                  case 34:
                    this.$("#chipset").html(" AST2520(A2)");
                  break;
                  case 35:
                    this.$("#chipset").html(" AST2530(A2)");
                  break;

                  default:
                    this.$("#chipset").html(" Unknown");
                  break;
              }
            },
            script_more_info: function(ev) {
	    	ev.preventDefault();
                if(app.configurations.isFeatureAvailable("AUTOMATION")) {
        		var url = '#scripts';
        		window.location.assign(url);
                }else{
                    alert("Automation Engine is Not Enabled");
                }
            },

            updateTasks: function(model, value) {
                if (model.get('tasks')) {
                    this.$("#user-activity-count").html(model.get('tasks').length);
                } else {
                    this.$("#user-activity-count").html("0");
                }
            },

            updateAuditLog: function() {
                this.$("#new-audit-issues-count").html(this.alc.models.length);
            },

            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });
