define(["jquery", "underscore", "backbone"],

    function($, _, Backbone) {

        var model = Backbone.Model.extend({

            defaults: {},

            validation: {

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