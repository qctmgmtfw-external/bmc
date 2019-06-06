define(['jquery', 'underscore', 'backbone',
        //data
        "collection/fru",
        //localize
        'i18n!strings/nls/fru',
        //template
        'text!templates/fru.html'
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
    ],

    function($, _, Backbone, FRUCollection, CommonStrings, FRUTemplate) {

        var view = Backbone.View.extend({

            template: _.template(FRUTemplate),

            initialize: function() {
                this.collection = FRUCollection;
                this.collection.bind("add reset", this.load, this);
                this.collection.fetch();
            },

            active: 0,

            events: {
                "change #fru_device_id": "rerender",
                "click .help-link": "toggleHelp"
            },

            beforeRender: function() {

            },

            afterRender: function() {},

            load: function(model, collection, xhr) {
                this.render();
            },

            rerender: function(ev) {
                if (ev) {
                    this.active = $(ev.target).val();
                } else {
                    this.active = this.$("#fru_device_id").val();
                }

                this.render();
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

            serialize: function() {

                var ret = {
                    locale: CommonStrings,
                    devices: [],
                    device: {},
                    board: {},
                    chassis: {},
                    product: {}
                };

                if (this.collection.length) {

                    if(this.active===0) {
                        this.active = this.collection.at(0).get('device').id;
                    }

                    console.log("active", this.active);

                    var context = this;

                    ret = {
                        locale: CommonStrings,
                        devices: this.collection.toJSON(),
                        device: this.collection.find(function(m){return m.get('device').id == context.active;}).get('device'),
                        board: this.collection.find(function(m) {return m.get('device').id == context.active;}).get('board'),
                        chassis: this.collection.find(function(m) {return m.get('device').id == context.active;}).get('chassis'),
                        product: this.collection.find(function(m) {return m.get('device').id == context.active;}).get('product'),
                        active: this.active
                    };
                }

                return ret;
            }

        });

        return view;

    });