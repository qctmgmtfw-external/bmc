define([
    "jquery",
    "underscore",
    "backbone",
    "app",
    "collection/event-log",
    "collection/sensors",
    "views/TimelineDateView",
    "views/TimelineItemView",
    "models/datetime/date_time",

    "i18n!strings/nls/common",
    //template
    'text!templates/eventlog.html',
    //plugins
    "moment",
    "chartjs",
    "datepicker"
], function($, _, Backbone, app, EventLogCollection, SensorsCollection, TimelineDateView, TimelineItemView,DateTimeModel, CommonStrings, LogTemplate, moment) {

    //var Chartjs = Chart.noConflict();

    var view = Backbone.View.extend({

        template: _.template(LogTemplate),

        initialize: function() {
            this.collection = EventLogCollection;

            SensorsCollection.bind("add", this.addSensor, this);
            this.datetimemodel = DateTimeModel;

            //this.collection.bind("add", this.addEvent, this);
            //this.collection.bind("reset", this.loadEvent, this);
        },

        events: {
            "click #iddate_range_begin": "showDatePickerDateRangeBegin",
            "click #iddate_range_end": "showDatePickerDateRangeEnd",
            "ifChecked input[name='time_zone']": "checkedTimeZone",
            "ifUnchecked input[name='time_zone']": "uncheckedTimeZone",
            "change #idsensor": "filter",
            "change #idevent_type": "filter",
            "dp.change #iddate_range_begin": "load",
            "dp.change #iddate_range_end": "load",
            "click #idcl_log": "sendClearSEL",
            "click #idsave_log": "saveSEL",
			"click .help-link": "toggleHelp",
            "click #idsave_ierr_log": "saveierr"
        },

        beforeRender: function() {
        },

        afterRender: function() {
        	this.$el.find('input[name="time_zone"]').iCheck({
                checkboxClass: 'icheckbox_square-blue',
                radioClass: 'iradio_square-blue',
                increaseArea: '20%'
            });
        	$("#idbmctimezone").iCheck("check");
            var timezoneOffset = 0;
            var abstimezoneOffset = 0;
            var gmtString = "";
            $.ajax({
                url: "/api/system-time",
                type: "GET",
                success: function(data){
                    timezoneOffset = Math.floor(data.utc_offset/60);
                    //gmtString = timezoneOffset < 0 ? " GMT + ": " GMT  ";
                    if(timezoneOffset >= 0)
                        gmtString = "GMT + ";
                    else
                        gmtString = "GMT - ";

                    abstimezoneOffset = Math.abs(timezoneOffset);
                    gmtString = gmtString+parseInt(abstimezoneOffset/60)+":"+parseInt(abstimezoneOffset%60);
                    $('#idUTCOffset').html(gmtString);
            }, dataType: "json"});
            
        	if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                app.disableAllbutton();
             }
            var context = this,startDate = null, endDate = null; //, SensorName = null;;

            $('#iddate_range_begin').datetimepicker({
                                pickTime: false
            });
            $("#iddate_range_end").datetimepicker({
                                pickTime: false
            });

            this.filterKey = location.hash.split('/').pop();
            if(this.filterKey == "today") {

                startDate = moment().startOf('day');
                endDate = moment();

            } else if(this.filterKey == "month") {

                startDate = moment().subtract(30,'days');
                endDate = moment();
            } else if(this.filterKey != "event-log") {
                /*
		        SensorsCollection.each(function(model) {
			        if(model.get('sensor_number') == filter)
	                    SensorName =  model.get('sensor_name');
                });
                */
                if(!isNaN(this.filterKey)) {
        		    $("#idsensor").val(this.filterKey);
                }
	        }

            $("#iddate_range_begin").data('DateTimePicker').setDate(startDate);
            $("#iddate_range_end").data('DateTimePicker').setDate(endDate);

            this.collection.fetch({
                success: function() {
                    console.log("call load events");
                    context.load.call(context);
                },
                failure: function() {

                }
            });

            SensorsCollection.fetch({success: function(){
                SensorsCollection.each(function(model){context.addSensor.call(context, model, SensorsCollection)});

                if(!isNaN(context.filterKey)) {
                    $("#idsensor").val(context.filterKey);
                    //this.$("#idsensor").trigger("change");
                    context.filter();
                }
            }});

            /*SensorsCollection.fetch({success: function(){
                SensorsCollection.each(function(model){context.addSensor.call(context, model, SensorsCollection)});
                console.log("sensor fetch"+this.filterKey);

                if(!isNaN(this.filterKey)) {
                    this.$("#idsensor").val(this.filterKey);
                    //this.$("#idsensor").trigger("change");
                    this.filter();
                }
            }});*/

            if(sessionStorage.privilege_id < CONSTANTS["OPERATOR"]){
                app.disableAllbutton();
            }
            var context = this;
            this.datetimemodel.fetch({
                success: function(){
                	window.sessionStorage.setItem("BMC_Timezone", context.datetimemodel.get("timezone"));
                    }
             });


        },

        showDatePickerDateRangeBegin: function() {
            $('#iddate_range_begin').data('DateTimePicker').show();
        },

        showDatePickerDateRangeEnd: function() {
            $('#iddate_range_end').data('DateTimePicker').show();
        },
        checkedTimeZone: function(ev) {
            //this.datetimemodel.set('time_zone', $(ev.target).filter(':checked').val());

            var timezoneOffset = 0;
            var abstimezoneOffset = 0;
            var gmtString = "";
            if ($(ev.target).filter(':checked').val() == '1') {
            	timezoneOffset = this.datetimemodel.get("utc_minutes");
            	gmtString = timezoneOffset < 0 ? " GMT - ": " GMT + ";
                abstimezoneOffset = Math.abs(timezoneOffset);
            	gmtString = gmtString+parseInt(abstimezoneOffset/60)+":"+parseInt(abstimezoneOffset%60);
            	$('#idUTCOffset').html(gmtString);
                $(".client_time").hide();
                $(".bmc_time").show();
            }
            if ($(ev.target).filter(':checked').val() == '2') {
            	timezoneOffset = new Date().getTimezoneOffset();
            	gmtString = timezoneOffset < 0 ? " GMT + ": " GMT - ";
                abstimezoneOffset = Math.abs(timezoneOffset);
            	gmtString = gmtString+parseInt(abstimezoneOffset/60)+":"+parseInt(abstimezoneOffset%60);
            	$('#idUTCOffset').html(gmtString);
                $(".bmc_time").hide();
                $(".client_time").show();
            }

        },
        uncheckedTimeZone: function() {},



        sendClearSEL: function() {
            var context = this;

            if(window.confirm("You are about to clear the IPMI Event Log. Do you really want to clear the IPMI Event Log?")) {
                $.ajax({
                    url: "/api/logs/event",
                    type: "DELETE",
                    success: function() {
                        context.collection.reset();
                        //context.collection.fetch();
                        context.collection.fetch({
                            success: function() {
                                console.log("call load events");
                                context.load.call(context);
                            },
                            failure: function() {

                            }
                        });

                    },
                    error : app.HTTPErrorHandler
                });
            }
        },
//SEL save log
        saveSEL: function() {
            function getbits(orig,startbit,endbit)
            {
                var temp = orig;
                var mask =0x00;

                for(var i=startbit;i>=endbit;i--)
                {
                    mask = mask | (1 << i);
                }

                return (temp & mask);
            }
            function getbitsval(orig,startbit,endbit)
            {
                var temp;
                temp = getbits(orig,startbit,endbit);
                temp = temp >> endbit;
                return temp;
            }
            function destroyClickedElement(event) {
                document.body.removeChild(event.target);
            }
            function download(strData, strFileName, strMimeType) {
                var D = document,
                    A = arguments,
                    a = D.createElement("a"),
                    d = A[0],
                    n = A[1],
                    t = A[2] || "text/plain";
                    var ieEDGE = navigator.userAgent.match(/Edge/g);
                    var ie = navigator.userAgent.match(/MSIE\s([\d.]+)/),
                    ie11 = navigator.userAgent.match(/Trident\/7.0/) && navigator.userAgent.match(/rv:11/),
                    ieVer=(ie ? ie[1] : (ie11 ? 11 : -1));

                    a.href = "data:" + strMimeType + "charset=utf-8," + escape(strData);
                    if ('download' in a)
            	    {
            	        a.setAttribute("download", n);
            	        a.innerHTML = "downloading...";
            	        D.body.appendChild(a);
            	        setTimeout(function() {
            	            var e = D.createEvent("MouseEvents");
            	            e.initMouseEvent("click", true, false, window, 0, 0, 0, 0, 0, false, false, false, false, 0, null);
            	            a.dispatchEvent(e);
            	            D.body.removeChild(a);
            	    		}, 66);
                	}
                	else if (window.Blob)
                	{
            	    	var textFileAsBlob = new Blob([strData], {type: 'text/plain'});
                	    var downloadLink = document.createElement("a");
            	        downloadLink.download = strFileName;
            	        downloadLink.innerHTML = "Download File";

            	        if (window.webkitURL != null) {
            	            downloadLink.href = window.URL.createObjectURL(textFileAsBlob);
            	     	   } else {
            	            downloadLink.href = window.URL.createObjectURL(textFileAsBlob);
            	            downloadLink.onclick = destroyClickedElement;
            	            downloadLink.style.display = "none";
            	            document.body.appendChild(downloadLink);
            	        }
            	        if (navigator.msSaveBlob)
            	        {
                     		window.navigator.msSaveBlob(textFileAsBlob, strFileName);
                    	}
                    	else
                    	{
                        	downloadLink.click();
                    	}
            	    }
                	else
                	{
                		console.log("Browser is not supported");
                	}
            }
            var context = this;
            var MAX_EVENT_TYPE = 9;     //0 to 8, Filter of Event type
            var ALL_SENSORS = 0;        //Filter for all sensors
            var ALL_EVENTS = 0;     //Filter for all events.
            var SYSTEM_EVT = 1;     //System Event Record tye.
            var OEM_EVT = 2;        //OEM Event record type.
            var BIOS = 3;       //BIOS generated events.
            var SMI = 4;        //SMI Handler Events
            var SMS = 5;        //System Management Software events
            var SYSTEM_OEM = 6; //System Software - OEM Events
            var RCS = 7;        //Remote Console Software Events
            var TMRCS = 8;      //Terminal Mode RCS Events
            if($("#_rdoSELHex").is(":checked"))
            {$.ajax({
                type: "GET",
                url: "/api/logs/event-file",
                success: function(response, status, xhr) {
                    var filename = "SEL_HEX.txt";
                    download(response, filename,'text/plain');
                }
            });}
        else if($("#_rdoSELText").is(":checked"))
        {
                $.ajax({
                type: "GET",
                url: "/api/logs/event",
                success: function(response, status, xhr) {
                    //var filename = "SELLog";
                    var SELINFO_DATA = response;
                    console.log("SELINFO_DATA = "+SELINFO_DATA);
                    var output="Event ID\t\tTime Stamp\t\t\t\t\tSeverity\tSensor Name\t\t\tSensor Type\t\t\t\tDescription\r\n";
                    console.log("SELINFO_DATA.length = "+SELINFO_DATA.length);
                    for (j = 0; j <= SELINFO_DATA.length; j++) {
                        if (j >= SELINFO_DATA.length) {
                            break;
                        }
                    var json = JSON.parse(JSON.stringify(SELINFO_DATA[j]));
                    console.log("content = "+JSON.stringify(SELINFO_DATA[j]));
                    console.log("severity = "+json.severity+"   event description = "+json.event);
                    //Quanta Change
                    //Change event parser to use common library libQuantaEventParser for sync all event messages are same
                    if (json.record_type >= 0x0 &&
                        json.record_type <= 0xBF) {
                        //Range reserved for standard SEL Record Types

                        //7-bit Event/Reading Type Code Range
                        type = getbits(json.event_direction, 6, 0);
                        if (type == 0x6F) {
                            // Quanta ---
                            if(json.sensor_type >= 0x1E && json.sensor_type <= 0x20)
                            {
                                // This event is generated by OS, sensor num = 0x00, so can't grab sensor name
                                json.sensor_name = "OS";
                            }
                            // --- Quanta
                        }

                        /* Show sensor number if can't get sensor name(because no sdr) */
                        console.log("sensor_number=" +json.sensor_number);
                        if(json.sensor_name == "Unknown")
                            json.sensor_name = "#0x"+json.sensor_number;

                        // --- Quanta
                        sensortype = json.sensor_type;

                    } else if (json.record_type >= 0xC0 &&json.record_type < 0xDF) {

                        // Quanta++
                        eventdesc = "";
                        sensortype = "";

                            /* SEL OEM Record - RecordType from 0xC0 to 0xDF */
                            json.sensor_name = json.record_type.toString(16);
                            for(var i = 3; i>=1; i--)
                            {
                                /* OEMDefined[1]~OEMDefined[3] is ManufacturerID, display on SensorType field */
                                var tmpOEM = (i.toString(16));
                                while (tmpOEM.length < 2) {
                                    tmpOEM = "0" + tmpOEM;
                                }

                                sensortype = sensortype + tmpOEM;
                            }

                        // Quanta--
                    } else if (json.record_type == 0xDF) {
                        //0xDF- Extended SEL records.
                        sensortype = "Extended SEL";
                    } else if (json.record_type >= 0xE0 &&
                        json.record_type <= 0xFF) {
                        // Quanta++
                        sensortype = "";
                    }
                    var offset = json.utc_offset;
                    var client_offset = new Date().getTimezoneOffset();
                    var eventTimeStamp = json.timestamp;
                    eventTimeStamp = eventTimeStamp + (client_offset*60);
                    var a = new Date(eventTimeStamp * 1000);
                    var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
                    var year = a.getFullYear();
                    var month = months[a.getMonth()];
                    var date = a.getDate();
                    var hour = a.getHours();
                    var min = a.getMinutes();
                    var sec = a.getSeconds();
                    if (sec < 10) {
                        sec = "0" + sec;
                    }
                    if (min < 10) {
                        min = "0" + min;
                    }
                    if (hour < 10) {
                        hour = "0" + hour;
                    }
                    var formattedTime =month+"/"+date+"/"+year+"    "+hour + ':' + min + ':' + sec;

                    if(!json.severity) json.severity = "N/A";
                    if(!json.sensor_name) json.sensor_name = "N/A";
                    if(!json.sensor_type) json.sensor_type = "N/A";

                    output += "\t"+json.id;
                    output += "\t\t" + formattedTime;
                    output += "\t\t[" + json.severity+"]"; if (json.severity.length <=8) output += "\t";
                    output += "\t[" + json.sensor_name+"]"; if (json.sensor_name.length < 8) output += "\t";
                    output += "\t[" + json.sensor_type+"]"; if (json.sensor_type.length < 8) output += "\t";
                    output += "\t" + json.event+"\r\n";
                }
                var filename = "SEL_TEXT.txt";
                download(output, filename,'text/plain');
            }
            });
         }
        },
        // Quanta ---
        updateEvent: function(){
        	// Set a timer at here.
        	// 1. If user stay in event-log page, timer will called by self.
        	// 2. If browser find new event coming, just let view load again.
        	setTimeout(function(){
        		if(window.location.hash == "#logs/event-log") {
        			if(EventLogCollection.length > window.eventCount)
        				window.thisEventView.load();
        			else
        				window.thisEventView.updateEvent();
        		}
            }, 5000);
        },
        // ---Quanta
        load: function() {
            console.log("loading events..");
            var eventLogs = this.collection;
            window.eventCount = eventLogs.length;//Quanta
            window.thisEventView = this;//Quanta
            this.chartData = {
                labels: [],
                data: []
            };


            //this.chart.removeData();
            
            if(this.filterKey == "deasserted")
            {
                eventLogs = this.collection.bydeassertion();
            }

            if ($("#iddate_range_begin input").val()) {
                var start = $("#iddate_range_begin").data("DateTimePicker").getDate(),
                    end;

                start = parseInt(moment(start).format('X'), 10);

                if ($("#iddate_range_end input").val()) {
                    end = $("#iddate_range_end").data("DateTimePicker").getDate();
                    end = parseInt(moment(end).format('X'), 10);
                }

                eventLogs = this.collection.byDateRange(start, end);
                //console.log("loading events...");
            }

            this.$el.find('#event-content').html('<li><i class="fa fa-clock-o animated fadeInDown"></i></li>');

            var groupedLog = eventLogs.groupBy(function(model) {
                //return moment(model.get('timestamp'), 'X').format('MMM Do YYYY');
                return moment(model.get('timestamp'), 'X').format('MMMM YYYY');
            });
            _.forEachRight(groupedLog, this.addEvents, this);

            this.drawChart();
            this.updateEvent();//Quanta
        },

        addSensor: function(model, collection) {
            var o = model.get("name"),
                v = model.get("sensor_number"),
                option = $("<option>").attr("value", v).html(o);

            this.$el.find("#idsensor").append(option);

        },

        addEvents: function(modelList, title, collection) {
            //console.log("add", arguments);
            //now render the date
            if(title=="January 1970") title=CommonStrings.pre_init_timestamp;  //Non timestamp default date.
            var context = this;
            var tdv = new TimelineDateView({
                date: title
            }).render().$el;

            this.chartData.labels.push(title);
            this.chartData.data.push(modelList.length);

            //console.log(modelList.length, title);

            //this.chart.addData([modelList.length], title);

            this.$el.find('#event-content').prepend(tdv);

            _.eachRight(modelList, function(model) {
                if(model.get('sensor_type')=='' && model.get('event_reading_class')!="oem") { //checked for OEM event
                     return;
                }

                var tiv = new TimelineItemView({
                    model: model
                }).render().$el;

                tiv.insertAfter(tdv);
                //$(".wrapper").resize();

            }, this);


        },

        filter: function() {
            var elSensor = this.$el.find("#idsensor"),
                elEventType = this.$el.find("#idevent_type");

            this.$el.find(".timeline-item-holder").hide();

            var sval = elSensor.val(),
                etval = elEventType.val();

            var filterClass = ".timeline-item-holder";

            if (sval) {
                filterClass += ".sensor_" + sval;
            }
            if (etval) {
                filterClass += "." + etval;
            }
            console.log(filterClass);

            this.$el.find(filterClass).show();
        },

        drawChart: function() {
            //Chartjs
        	//Quanta---
        	var parent = this.$el.find("#log-count-graph-canvas").parent();
        	this.$el.find("#log-count-graph-canvas").remove();
        	parent.append('<canvas id="log-count-graph-canvas"><canvas>');
        	//---Quanta
            var ctx = this.$el.find("#log-count-graph-canvas").get(0).getContext("2d");

            var data = {
                labels: this.chartData.labels,
                datasets: [{
                    label: "data1",
                    fillColor: "rgba(255,255,255,.9)",
                    strokeColor: "#fff",
                    highlightFill: "#fff",
                    highlightStroke: "rgba(255,255,255,.9)",
                    data: this.chartData.data
                }]
            };

            var options = {
                //scaleShowGridLines: false,
                //scaleShowLabels: false,
                barValueSpacing: 5,
                scaleGridLineColor: "rgba(255,255,255,.2)",
                scaleFontColor: "#fff",
                scaleIntegersOnly: true,
                scaleBeginAtZero: true,
                scaleLineColor: "rgba(255, 255, 255, 1)",
                responsive: true,
                //maintainAspectRatio: true,
                showTooltips: true
            };
            //Quanta ---
            if(!isNaN(this.chartData.data)) {
            	if(this.chartData.data > 896) {
            		options.scaleOverride = true;
            		options.scaleSteps = 9;
            		options.scaleStepWidth = 101;
            		//scaleStartValue = 0;
            	}
            }
            // --- Quanta
            this.chart = new Chart(ctx).Bar(data, options);

            /* //Morris
            this.barChart = new Morris.Bar({
                element: "log-count-graph",
                data: [{
                    y: '2006',
                    a: 100,
                    b: 90
                }, {
                    y: '2007',
                    a: 75,
                    b: 65
                }, {
                    y: '2008',
                    a: 50,
                    b: 40
                }, {
                    y: '2009',
                    a: 75,
                    b: 65
                }, {
                    y: '2010',
                    a: 50,
                    b: 40
                }, {
                    y: '2011',
                    a: 75,
                    b: 65
                }, {
                    y: '2012',
                    a: 100,
                    b: 90
                }],
                xkey: 'y',
                ykeys: ['a', 'b'],
                labels: ['Series A', 'Series B'],
                barColors: ['rgba(255, 255, 255, 1)', 'rgba(255,255,255,.75)'],
                gridTextColor: 'white'
            });*/
        },

        /*loadEvent : function(collection, xhr) {
			console.log("load", arguments);
		},*/
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
            saveierr: function(){
              $.ajax({
                  type: "GET",
                  url: "/api/logs/ierr-file",
                  success: function(response, status, xhr) {
                      function destroyClickedElement(event) {
                          document.body.removeChild(event.target);
                      }
                      var filename = "IERR.txt";
                      //console.log("response = "+response);
                      if(response!="")
                      {
                        var textFileAsBlob = new Blob([response], {type: 'text/plain'});
                  	    var downloadLink = document.createElement("a");
              	        downloadLink.download = filename;
              	        downloadLink.innerHTML = "Download File";

              	        if (window.webkitURL != null) {
              	            downloadLink.href = window.URL.createObjectURL(textFileAsBlob);
              	     	   } else {
              	            downloadLink.href = window.URL.createObjectURL(textFileAsBlob);
              	            downloadLink.onclick = destroyClickedElement;
              	            downloadLink.style.display = "none";
              	            document.body.appendChild(downloadLink);
              	        }
              	        if (navigator.msSaveBlob)
              	        {
                       		window.navigator.msSaveBlob(textFileAsBlob, filename);
                      	}
                      	else
                      	{
                          	downloadLink.click();
                      	}
                    }
                    else {
                      alert("The log contains no data!");
                    }
                  },
                  error: function() {
                    alert("There is no IERR logs.");
                  }
                });
            },
        serialize: function() {
            return {
                locale: CommonStrings
            };
        }

    });

    return view;

});
