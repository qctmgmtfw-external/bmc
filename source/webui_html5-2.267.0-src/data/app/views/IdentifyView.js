define(['jquery', 'underscore', 'backbone','app',
        //data

        //localize
        'i18n!strings/nls/identify',
        //template
        'text!templates/identify.html'
    ],

    function($, _, Backbone, app, CommonStrings , IdentifyTemplate) {

        var view = Backbone.View.extend({

            template: _.template(IdentifyTemplate),
            active:1,

            initialize: function() {
                console.log("initialize");
                this.ledstatus;
                this.getLEDStatus();
            },
            afterRender: function() {
                if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                    app.disableAllbutton();
                    app.disableAllinput();
                 }
                 this.$el.find('input[name="identify_status"]').iCheck({
                     checkboxClass: 'icheckbox_square-blue',
                     radioClass: 'iradio_square-blue'//,
                     //increaseArea: '20%'
                 });
                 if(sessionStorage.privilege_id < CONSTANTS["ADMIN"]){
                    $("#save").addClass("disable_a_href");
                 }
             },
            events: {
                "click .help-link": "toggleHelp",
                "click #save": "setLED",
            },

            toggleHelp: function(e) {
                console.log("toggleHelp");
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
            getLEDStatus:function()
            {
                function updateLEDStatus(value){
                    if(value == 0)
                    {
                        $("#led_status").html(" "+CommonStrings.led_off).css('color', 'red');//To.Do
                        $('input[name="identify_status"][value="0"]').iCheck('enable');
                        $('input[name="identify_status"][value="0"]').iCheck('check');
                        //$('input[name="blink_timeout"]').attr('readonly','readonly');
                        this.ledstatus = 0;
                    }
                    else if(value == 1)
                    {
                        $("#led_status").html(" "+CommonStrings.led_on).css('color', 'green');
                        $('input[name="identify_status"][value="1"]').iCheck('enable');
                        $('input[name="identify_status"][value="1"]').iCheck('check');
                        //$('input[name="blink_timeout"]').attr('readonly','readonly');
                        this.ledstatus = 1;
                    }


                }
                $.ajax({
                    url: "/api/chassis-status",
                    type: "GET",
                    success: function(response)
                    {
                        var status = response.led_status;
                        console.log("status = "+status);
                        updateLEDStatus(status);
                    }, dataType: "json"});
            },

            serialize: function() {

                return {
                    locale: CommonStrings
                };
            },
            setLED: function(){
                function updateLEDStatus(value){
                    if(value == 0)
                    {
                        $("#led_status").html(" "+CommonStrings.led_off).css('color', 'red');
                        this.ledstatus = 0;
                    }
                    else if(value == 1)
                    {
                        $("#led_status").html(" "+CommonStrings.led_on).css('color', 'green');
                        this.ledstatus = 1;
                    }
                    else if(value == 2)
                    {
                        $("#led_status").html(" "+CommonStrings.led_blink).css('color', 'green');
                        this.ledstatus = 2;
                    }
                }
                var blink_timeout = 15;
                var force_on = 0;
                this.ledstatus = parseInt($('input[name="identify_status"]').filter(':checked').val());
                console.log("this.ledstatus = "+this.ledstatus);
                var input_value = document.getElementById("blink_timeout").value;
                console.log("input_value = "+input_value);
                if((isNaN(input_value))||(input_value<1)||(input_value>255))
                {
                    console.log("isNaN(input_value) = "+isNaN(input_value));
                    alert("Please enter the number between 1 to 255");
                    return;
                }
                if(this.ledstatus == 2)//Blink
                {
                    
                    if((input_value>=1) && (input_value<=255))//if timeout is sending
                    {
                        blink_timeout = input_value;
                    }
                    console.log("blink_timeout = "+blink_timeout);
                }
                else if(this.ledstatus == 1)//On
                {
                    force_on=1;
                }
                else if(this.ledstatus == 0)//Off
                {
                    blink_timeout = 0;
                }
                $.ajax({
                    url: "/api/actions/chassis-led",
                    type: "PUT",
                    contentType: 'application/json',
        		    data: JSON.stringify({"blink_timeout": blink_timeout, "force_on": force_on}),
                    dataType: "json",
                    success: function(model) {
                        if(model.force_on)
                        {
                            updateLEDStatus(1);
                        }
                        else if((model.force_on==0)&&(model.blink_timeout>0)) {
                            updateLEDStatus(2);
                        }
                        else {
                            updateLEDStatus(0);
                        }
                        //updateLEDStatus(this.ledstatus);
                    },
                    error: app.HTTPErrorHandler
                });
            }

        });

        return view;

    });
