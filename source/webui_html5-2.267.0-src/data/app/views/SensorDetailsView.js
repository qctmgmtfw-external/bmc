define(['jquery', 'underscore', 'backbone', 'app',
        //data
	    "collection/event-log",
        'collection/sensors',
        "views/TimelineDateView",
        "views/TimelineItemView",
        //localize
        'i18n!strings/nls/sensor-details',
        //template
        'text!templates/sensor-details.html',
        //plugins
        'moment',
        'd3',
        'c3',
        //'knob',
        'chartjs'
    ],

    function($, _, Backbone, app, EventLogCollection, SensorCollection, TimelineDateView, TimelineItemView, CommonStrings, SensorTemplate, moment, d3, c3) {
		//Quanta ---
		//An easy way to identify the number is float or interage
		function isFloat(n) {
			return Number(n) === n && n % 1 !== 0;
		}
		// --- Quanta
        var view = Backbone.View.extend({

            template: _.template(SensorTemplate),

            initialize: function() {

                this.collection = SensorCollection;
		        EventLogCollection.bind("add remove change reset", this.renderSensorEvents, this);

            },

            close: function() {
                //app.poll.off("2sec");
            },

            events: {
		        "click #sensor-events": "showDetails",
                	"click .help-link": "toggleHelp"
	        },

            beforeRender: function() {

            },

            afterRender: function() {
                var that = this;// , NewCollection = null;;

                this.sensor_id = location.hash.split('/').pop();

                var hrefURL = $("#change-threshold").attr("href")+"/"+this.sensor_id;

                $("#change-threshold").attr("href", hrefURL);

                if(sessionStorage.privilege_id < CONSTANTS["OPERATOR"]){
                   $("#change-threshold").addClass("disable_a_href");
                }

                EventLogCollection.fetch({
                        success: function(collection, resp, options) {
				            that.renderSensorEvents.call(that, EventLogCollection.bySensor(that.sensor_id, 20))
                        },
                });


                //console.log("sensors afterRender");
                this.collection.fetch({
                    success: function(collection, resp, options) {
                        that.model = collection.getSensor(that.sensor_id);

                        that.loaded.call(that, that.model);

                        //that.drawChart.call(that);
                        that.drawChartC3.call(that);
                        that.updateReading.call(that);
                        that.collection.bind("add loaded reset", that.updateChartC3, that);
                        that.collection.bind("add loaded reset", that.updateReading, that);

                        that.setThresholds.call(that);
                    },
                    error: function() {}
                });
            },

            updateReading: function() {
            	//Quanta---
            	var val = this.model.reading();
                //this.$('#live-reading-text').html(this.model.reading()+" "+this.model.unit());//AMI
            	this.$('#live-reading-text').html((isFloat(val)? val.toFixed(3) : val)+" "+this.model.unit());
            	//---Quanta
            },

            setThresholds: function() {
				var sensor_srt_mask = this.model.get('sensor_srt_mask');
				var val = 0;//Quanta
				if(sensor_srt_mask & 0x20) {
					//Quanta---
                	//this.$('#unr').html(this.model.hnr()+' '+this.model.unit());//AMI
					val = this.model.hnr();
					this.$('#unr').html((isFloat(val)? val.toFixed(3) : val)+' '+this.model.unit());
					//---Quanta
				}
				else
					this.$('#unr').html(" N/A");

				if(sensor_srt_mask & 0x10){
					//Quanta---
                	//this.$('#uc').html(this.model.hc()+' '+this.model.unit());//AMI
					val = this.model.hc();
					this.$('#uc').html((isFloat(val)? val.toFixed(3) : val)+' '+this.model.unit());
					//---Quanta
				}
				else
					this.$('#uc').html(" N/A");

				if(sensor_srt_mask & 0x08){
					//Quanta---
                	//this.$('#unc').html(this.model.hnc()+' '+this.model.unit());//AMI
					val = this.model.hnc();
                	this.$('#unc').html((isFloat(val)? val.toFixed(3) : val)+' '+this.model.unit());
                	//---Quanta
				}
				else
					this.$('#unc').html(" N/A");

				if(sensor_srt_mask & 0x01){
					//Quanta---
                	//this.$('#lnc').html(this.model.lnc()+' '+this.model.unit());//AMI
					val = this.model.lnc();
                	this.$('#lnc').html((isFloat(val)? val.toFixed(3) : val)+' '+this.model.unit());
                	//---Quanta
				}
				else
					this.$('#lnc').html(" N/A");

				if(sensor_srt_mask & 0x02){
					//Quanta---
                	//this.$('#lc').html(this.model.lc()+' '+this.model.unit());//AMI
					val = this.model.lc();/*
					if(isFloat(val)) {
						console.log("It is float " + val);
						val.toFixed(3);}
					console.log("lc: " + val);*/
                	this.$('#lc').html((isFloat(val)? val.toFixed(3) : val)+' '+this.model.unit());
                	//---Quanta
				}
				else
					this.$('#lc').html(" N/A");

				if(sensor_srt_mask & 0x04){
					//Quanta---
                	//this.$('#lnr').html(this.model.lnr()+' '+this.model.unit());//AMI
					val = this.model.lnr();
                	this.$('#lnr').html((isFloat(val)? val.toFixed(3) : val)+' '+this.model.unit());
                	//---Quanta
				}
				else
					this.$('#lnr').html(" N/A");
            },

            drawChartC3: function() {
				//Quanta+[
            	function changeTwoDecimal_f(x){
                    var f_x = parseFloat(x);
                    if (isNaN(f_x)) {
                        alert('function:changeTwoDecimal->parameter error');
                        return false;
                    }
                    var f_x = Math.round(x * 100) / 100;
                    var s_x = f_x.toString();
                    var pos_decimal = s_x.indexOf('.');
                    if (pos_decimal < 0)
                    {
                        pos_decimal = s_x.length;
                        s_x += '.';
                    }
                    while (s_x.length <= pos_decimal + 2)
                    {
                        s_x += '0';
                    }
                    return s_x;
                }
                //Quanta+]
                if(this.chart) return;

                var labels = _.map(_.pluck(app.readings['sensor_' + this.sensor_id], 'time'), function(t){
                    return moment(new Date(t)).format('YYYY-MM-DD HH:mm:ss');
                });

                var data = _.pluck(app.readings['sensor_' + this.sensor_id], 'reading');

                this.chartLength = labels.slice(-15).length;

                //console.log(labels.slice(-10));
                var xdata = ['x'].push(labels.slice(-15));
                var ydata = [this.model.sensorType()].push(data.slice(-15));

                //Quanta*
                var lnr = this.model.lc();//this.model.lnr();
                var hnr = this.model.hc();//this.model.lnr();//this.model.hnr();
                var tmp = 0;
                var origHnr = hnr;

                if(lnr>hnr) {
                    tmp = lnr;
                    lnr = hnr;
                    hnr = tmp;
                }

                var inc = (hnr-lnr)/6;
				var unit = this.model.unit();
				if(unit.indexOf("&deg;")> -1 )
                	var unit = "°C";
				if(this.model.get('discrete_state') !== 0 && this.model.get('accessible') !== 0xD5) {
                    unit = "N/A";
                }
                var thresholds = [lnr, changeTwoDecimal_f(lnr + inc), changeTwoDecimal_f(lnr + (inc*2)), changeTwoDecimal_f(lnr + (inc*3)), changeTwoDecimal_f(lnr + (inc *4)), changeTwoDecimal_f(lnr + (inc * 5)),  hnr];//Quanta+

                if(origHnr != 0) { //Quanta++
                	this.chart = c3.generate({
                		bindto: '#live-reading',
                		data: {
                			x: 'x',
                			xFormat: '%Y-%m-%d %H:%M:%S',
                			columns: [
                			          // ['x'],
                			          // [this.model.sensorType()]
                			          xdata,
                			          ydata
                			          ]
                		},
                		axis : {
                			x: {
                				type: 'timeseries',
                				tick: {
                					culling: true,
                					format: '%H:%M:%S'
                				}
                			},
                			y: {
                				label: {
                					text: this.model.unit(),
                					position: 'outer-middle'
                				},
                				tick: {
                					values: thresholds,
	 								format: function(d){
										return (Math.floor(d*100)/100);
									}
               				},
                				//max: hnr,
                				min: lnr,
                            	default: thresholds
                			}
                		}
                	});
                } else {
                	this.chart = c3.generate({
                		bindto: '#live-reading',
                		data: {
                			x: 'x',
                			xFormat: '%Y-%m-%d %H:%M:%S',
                			columns: [ xdata, ydata ]
                		},
                		axis : {
                			x: {
                				type: 'timeseries',
                				tick: {
                					culling: true,
                					format: '%H:%M:%S'
                				}
                			},
                			y: {
                				label: {
                					text: this.model.unit(),
                					position: 'outer-middle'
                				}
                			}
                		}
                	});
                }
                //app.poll.on("10sec", this.updateChartC3, this);
            },

            updateChartC3: function() {
                if(!this.chart) return;
                var l = 0;
                var now = moment(new Date());
                //Quanta---
                var type = this.model.sensorType();
                //This closure is used for storing the sensor data. And return the data for c3 chart use.
                function getC3DataFun(){
                	var dataTimeArray = ['x'];
                	var dataValArray = [type];
                	var dataUnixTimeArray = [];
                	function updateChartData(dataVal, dataTime, curUnixTime){
                		dataValArray.push(dataVal);
                		dataTimeArray.push(dataTime);
                		dataUnixTimeArray.push(curUnixTime);
                		//We only keep last 5 min data.
                		if((curUnixTime - dataUnixTimeArray[0]) > 300){
                			dataTimeArray.splice(1,1);
                			dataValArray.splice(1,1);
                			dataUnixTimeArray.shift();
                		}

                		return {
                			time : dataTimeArray,
                			readData : dataValArray
                		};
                	}
                	return updateChartData;
                }

                if(!this.updateChartDataFun){
                	this.updateChartDataFun = getC3DataFun();
                }

                var C3Data = this.updateChartDataFun(this.model.reading(), now.format('YYYY-MM-DD HH:mm:ss'), now.format('X'));

                this.chart.load({
                    columns: [
                        C3Data.time,
                        C3Data.readData
                    ],
                    to: C3Data.time[1]
                });
                //Here to fix the time stamp may be covered due to width limited.
                //Force extend 105% width length.  
                var svgWidth = $("svg").attr("width");
                svgWidth = svgWidth * 1.05;
                $("svg").attr("width", svgWidth);
                //---Quanta

                //Below are AMI original.
                //this.chartLength++;

                //console.log(this.chartLength);

                //if(this.chartLength > 15) {
                //    l = 1;
                //    this.chartLength = 16;
                //}

                //var now = moment(new Date());//Quanta--

                //this.chart.flow({
                //    columns: [
                //        ['x', now.format('YYYY-MM-DD HH:mm:ss')],
                //        [this.model.sensorType(), this.model.reading()]
                //    ],
                //    duration: 1000,
                //    length: l,
                //    to: now.subtract(5, 'minutes').format('YYYY-MM-DD HH:mm:ss')
                //});
            },

            drawChart: function(){

                var labels = _.map(_.pluck(app.readings['sensor_' + this.sensor_id], 'time'), function(t){
                    return moment(new Date(t)).format('h:mm:ss');
                });

                this.chartData = {
                    labels: labels,
                    data: _.pluck(app.readings['sensor_' + this.sensor_id], 'reading')
                };

                var ctx = this.$el.find("#live-reading").get(0).getContext("2d");

                var data = {
                    labels: this.chartData.labels.slice(-10),
                    datasets: [{
                        label: "data1",
                        fillColor: "rgba(151,187,205,0.2)",
                        strokeColor: "rgba(151,187,205,1)",
                        highlightFill: "#fff",
                        highlightStroke: "rgba(255,255,255,.9)",
                        data: this.chartData.data.slice(-10)
                    }]
                };

                this.chartLength = this.chartData.labels.slice(-10).length;

                var options = {
                    //scaleShowGridLines: false,
                    //scaleShowLabels: false,
                    // scaleOverride: true,
                    // scaleSteps: Math.floor((this.model.hnr() - this.model.lnr())/6),
                    // scaleStartValue: this.model.lnr(),
                    scaleGridLineColor: "rgba(200,200,200,.4)",
                    scaleFontColor: "#888",
                    scaleIntegersOnly: false,
                    scaleBeginAtZero: false,
                    scaleLineColor: "rgba(150, 150, 150, .1)",
                    responsive: true,
                    animation: false,
                    //maintainAspectRatio: true,
                    showTooltips: true
                };

                this.chart = new Chart(ctx).Line(data, options);
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

            updateChart: function(model, collection) {
                if(!this.chart) return;

                this.chart.addData([this.model.reading()], moment(new Date()).format('h:mm:ss'));

                this.chartLength++;

                if(this.chartLength > 10) {
                    this.chart.removeData();
                }
            },

            load: function(model, collection, xhr) {},

            loaded: function(model) {
                this.renderSensorDetail.call(this, model);
//                this.renderSensorEvents.call(this, model);

               /* this.collection.each(function(model) {
                    if (!app.readings) app.readings = {};

                    if (!app.readings['sensor_' + model.get('sensor_number')]) {
                        app.readings['sensor_' + model.get('sensor_number')] = [];
                    }
                    app.readings['sensor_' + model.get('sensor_number')].push(model.reading());
                });*/
            },

            renderSensorDetail: function(model) {
                this.$('#sensor_name').text(model.get('name'));
            },

            renderSensorEvents: function(SensorEvents) {

		this.$el.find('#event-content').html('<li><i class="fa fa-clock-o animated fadeInDown"></i></li>');

		if (SensorEvents.length >= 20){
                        this.$el.find('#event-content').html('<div><i class="pull-right"><a class="btn btn-primary" id="sensor-events">More</a><i></div>');
                }

                var groupedLog = SensorEvents.groupBy(function(model) {
                    return moment(model.get('timestamp'), 'X').format('MMMM YYYY');
                });


                _.forEachRight(groupedLog, this.addEvents, this);


            },

        addEvents: function(modelList, title, collection) {
            //now render the date
            var tdv = new TimelineDateView({
                date: title
            }).render().$el;
//            this.chartData.labels.push(title);
//            this.chartData.data.push(modelList.length);

            this.$el.find('#event-content').prepend(tdv);

            _.eachRight(modelList, function(model) {
                var tiv = new TimelineItemView({
                    model: model
                }).render().$el;
                tiv.insertAfter(tdv);
                $(".wrapper").resize();

            }, this);
        },

        showDetails: function(){
                app.router.navigate("logs/event-log/"+ this.sensor_id, {trigger: true});
        },


            serialize: function() {
                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });
