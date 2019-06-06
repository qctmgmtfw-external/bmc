define(["jquery", "underscore", "backbone", "i18n!strings/nls/common"],

function($, _, Backbone, locale) {

    var model = Backbone.Model.extend({

        defaults: {},

        validation: {

        },

        url: "/api/chassis-status",

        power: function(command, opts) {
        	var model = this;
        	var options = {
        		url: '/api/actions/power',
        		type: 'POST',
                	contentType: 'application/json',
        		data: JSON.stringify({"power_command": command})
        	};

        	_.extend(options, opts);

        	return (this.sync || Backbone.sync).call(this, null, this, options);
        },

        led: function(blink_time, force_on, opts) {
        	var model = this;
        	var options = {
        		url: '/api/actions/chassis-led',
        		type: 'POST',
                	contentType: 'application/json',
        		data: JSON.stringify({"blink_time": blink_time, "force_on": force_on})
        	};

        	_.extend(options, opts);

        	return (this.sync || Backbone.sync).call(this, null, this, options);
        }
    });

    return new model;

});
