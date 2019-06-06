define(['jquery', 'underscore', 'backbone', 'app',
//data
'collection//logical_device_info',
//localize
'i18n!strings/nls/logical_device_info',
//template
'text!templates/settings/raid_management/logical_device_info_edit_item.html',
//plugins

],

function($, _, Backbone, app, LogicalDeviceInfoCollection, locale, LogicalDeviceInfoEditItemTemplate) {

    var view = Backbone.View.extend({

        template: _.template(LogicalDeviceInfoEditItemTemplate),

        initialize: function() {
            LogicalDeviceInfoCollection.fetch({
                async: false
            });
            var id = location.hash.split('/').pop();
            if (!isNaN(id)) {
                this.model = LogicalDeviceInfoCollection.get(id);
            } else {
                this.model = new LogicalDeviceInfoCollection.model;
                this.model.url = LogicalDeviceInfoCollection.url;
            };
            this.model.bind('change:controller_id', this.updateControllerId, this);
            this.model.bind('change:controller_name', this.updateControllerName, this);
            this.model.bind('change:ld_name', this.updateLdName, this);
            this.model.bind('change:type', this.updateType, this);
            this.model.bind('change:state', this.updateState, this);
            this.model.bind('change:Stripe_size', this.updateStripeSize, this);
            this.model.bind('change:read_policy', this.updateReadPolicy, this);
            this.model.bind('change:write_policy', this.updateWritePolicy, this);
            this.model.bind('change:cache_policy', this.updateCachePolicy, this);
            this.model.bind('change:bgi', this.updateBgi, this);
            this.model.bind('change:ssd_caching', this.updateSsdCaching, this);
            this.model.bind('change:progress', this.updateProgress, this);
            this.model.bind('change:bad_blocks', this.updateBadBlocks, this);
            this.model.bind('change:size', this.updateSize, this);


        },

        events: {
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {


            //this.model.fetch();
            var _parent = this;
            _.each(this.model.attributes, function(value, attr, list) {
                _parent.model.trigger('change:' + attr, this.model, value);
            });
        },

        load: function(model, collection, xhr) {

        },

        reload: function(model, value) {

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
        updateControllerId: function(model, value) {},
        updateControllerName: function(model, value) {},
        updateLdName: function(model, value) {},
        updateType: function(model, value) {},
        updateState: function(model, value) {},
        updateStripeSize: function(model, value) {
            var res=this.GetStripeSize(value);
            $('#idStripe_size').html(res);
        },
        updateReadPolicy: function(model, value) {},
        updateWritePolicy: function(model, value) {},
        updateCachePolicy: function(model, value) {},
        updateBgi: function(model, value) {
            if(value==0){
                $('#idbgi').html(locale.bgi_enabled);   
            } else if(value==1) {
                $('#idbgi').html(locale.bgi_disabled);
            } else{
                $('#idbgi').html(value);
            }
        },
        updateSsdCaching: function(model, value) {
            if(value==0){
                $('#idssd_caching').html(locale.ssd_disabled);   
            } else if(value==1) {
                $('#idssd_caching').html(locale.ssd_enabled);
            } else{
                $('#idssd_caching').html(value);
            }
        },
        updateProgress: function(model, value) {
            if(value==255){
                $('#idprogress').html(locale.not_available);
            } else{
                $('#idprogress').html(value);    
            }
        },
        updateBadBlocks: function(model, value) {
            if(value==0){
                $('#idbad_blocks').html(locale.blocks_empty);   
            } else if(value==1) {
                $('#idbad_blocks').html(locale.blocks_not_empty);
            } else{
                $('#idbad_blocks').html(value);
            }  
        },
        updateSize: function(model, value) {
            $('#idsize').html(value);
        },

        GetStripeSize:function(value){
            var byteValue = this.GetStripeSizeByte(value);
            var originalValue;
            
            if(byteValue < 1024){
                originalValue = byteValue+ " Bytes"
            }else if ((byteValue >= 1024) && (byteValue < (1024*1024))){
                originalValue = (byteValue/1024) + " KB"
            }else if ((byteValue >= (1024 * 1024)) && (byteValue < (1024 * 1024 * 1024))){
                originalValue = (byteValue/(1024 * 1024)) + " MB"
            }else if (byteValue >= (1024 * 1024 * 1024)){
                originalValue = (byteValue/(1024 * 1024 * 1024)) + " GB"
            }
            return originalValue;
        },
        GetStripeSizeByte:function(value) {
            var byteValue= (512 * (Math.pow(2,value)));
            return byteValue;
        },
        validData: function(model) {
            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }
        },

        errorFields: [],

        invalidData: function(model, errors) {
            console.log('invalid', errors);
            var elMap = [],
                jel;

            var field = null;
            while ((field = this.errorFields.pop()) !== undefined) {
                field.tooltip('hide');
            }

            for (var e in errors) {
                jel = this.$el.find("#" + elMap[e]);
                jel.tooltip({
                    animation: false,
                    title: errors[e],
                    trigger: 'manual',
                    placement: 'top'
                });
                jel.tooltip('show');
                this.errorFields.push(jel);


            }
            $("#save-icon").removeClass().addClass("fa fa-save");
        },

        save: function() {


            $("#save-icon").removeClass().addClass("ion-loading-c");
            this.$(".alert-success,.alert-danger").addClass("hide");


            var context = this;
            try {
                context = that || this;
            } catch (e) {
                context = this;
            }
            context.model.save({}, {
                success: function() {

                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_success', context);
                    var back_url = location.hash.split('/').slice(0, - 2);
                    //back_url.pop();
                    app.router.navigate(back_url.join('/'), {
                        trigger: true
                    });
                },

                error: function() {
                    context.$("#save-icon").removeClass().addClass("fa fa-save");
                    app.events.trigger('save_error', context);
                }
            });


        },

        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
