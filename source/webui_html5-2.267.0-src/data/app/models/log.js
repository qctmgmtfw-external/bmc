define([
        "jquery", "underscore", "backbone",
        'i18n!strings/nls/common'
    ],

    function($, _, Backbone, CommonStrings) {

        var model = Backbone.Model.extend({

            initialize: function() {
                this.localeDate = new Date();
                this.localeTimeZoneMinutes = this.localeDate.getTimezoneOffset();
            },

            description: function() {
                return this.get('event_description').split('_').join(' ');
            },

            monthStrings: [CommonStrings.jan, CommonStrings.feb, CommonStrings.mar, CommonStrings.apr, CommonStrings.may, CommonStrings.jun,
                CommonStrings.jul, CommonStrings.aug, CommonStrings.sep, CommonStrings.oct, CommonStrings.nov, CommonStrings.dec
            ],

            day: [CommonStrings.sun, CommonStrings.mon, CommonStrings.tue, CommonStrings.wed, CommonStrings.thu, CommonStrings.fri, CommonStrings.sat],

            getMonth: function(utc) {
                if (utc) {
                    return this.getUtcMonth(utc);
                } else {
                    //				log("month :", new Date(this.get('timestamp') * 1000).getMonth(),  new Date(this.get('timestamp') * 1000).getUTCMonth());
                    return new Date(this.get('timestamp') * 1000).getMonth();
                }
            },

            getYear: function(utc) {
                if (utc) {
                    return this.getUtcYear(utc);
                } else {
                    //				log("year : ", new Date(this.get('timestamp') * 1000).getFullYear());
                    return new Date(this.get('timestamp') * 1000).getFullYear();
                }
            },

            getUtcYear: function(utc) {
                return new Date((utc - (this.localeTimeZoneMinutes * 60)) * 1000).getUTCFullYear();
            },

            getUtcMonth: function(utc) {
                //			log("getUtcMonth: ", utc);
                return new Date((utc - (this.localeTimeZoneMinutes * 60)) * 1000).getUTCMonth();
            },

            getUtcDate: function(utc) {
                return new Date((utc - (this.localeTimeZoneMinutes * 60)) * 1000).getUTCDate();
            },

            getUtcDay: function(utc) {
                var view = this;
                //			log("getUtcDay: ",utc, (this.localeTimeZoneMinutes*60), new Date((utc-(view.localeTimeZoneMinutes*60)) * 1000).getUTCDay(), this.day[new Date((utc-(view.localeTimeZoneMinutes*60)) * 1000).getUTCDay()]);
                return this.day[new Date((utc - (this.localeTimeZoneMinutes * 60)) * 1000).getUTCDay()];
            },

            getUtcHour: function(utc) {
                return new Date((utc - (this.localeTimeZoneMinutes * 60)) * 1000).getUTCHours();
            },

            getUtcMinutes: function(utc) {
                return new Date((utc - (this.localeTimeZoneMinutes * 60)) * 1000).getUTCMinutes();
            }

        });

        return model;

    });
