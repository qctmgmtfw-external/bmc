define(["jquery", "underscore", "backbone"],

    function($, _, Backbone) {

        var model = Backbone.Model.extend({

            defaults: {},

            validation: {

            },

            timestamp: function() {
                if(isNaN(this.get('timestamp'))) {
                    var logdate = this.get('timestamp').replace('  ',' ');
                    var lda = logdate.split(' ');
                    var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
                    var n = new Date();
                    var lm = months.indexOf(lda[0]);
                    var yr = n.getFullYear();
                    if(lm > n.getMonth()){
                        yr -= 1;
                    }
                    var dt = lda[1];
                    var time = lda[2].split(':');

                    return Math.floor(new Date(yr, lm, dt, time[0], time[1], time[2]).getTime()/1000);

                } else {
                    return this.get('timestamp');
                }
            },

            cause: function() {
                return this.get('message').split(':')[0];
            },

            event: function() {
                var msg = this.get('message');

                var msgary = msg.split(':');
                msgary.shift();
                msg = msgary.join(':');

                if (msg.length > 70) {
                    msg = msg.substr(0, 70) + '...';
                }
                return msg;
            }
        });

        return model;

    });