define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/rmedia',
//template
'text!templates/image_redirection/rmedia_item.html'],

function($, _, Backbone, app, locale, RmediaTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(RmediaTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
            this.model.bind("change:image_name", this.updateImageName, this);
            this.model.bind("change:redirection_status", this.updateRedirectionStatus, this);
            this.collection.bind("add reset remove", this.changeInImages, this);
        },

        events: {
            "click .help-link": "toggleHelp",
            "click .action-1": "startRedirection",
            "click .action-2": "stopRedirection"
        },

        beforeRender: function() {},

        afterRender: function() {
            this.changeInImages.call(this);
            if(!app.isVMedia()){
               $(".content .btn").addClass("disable_a_href"); 
            }
        },

        updateImageName: function(model, value) {
            if(value) {
                this.$(".image_name").html(value);
            } else {
                this.$(".image_name").html("");
                this.changeInImages.call(this);
            }
        },

        updateRedirectionStatus: function(model, value) {
            if(value == 1) {
                this.$(".session_index").html(model.get('session_index'));
                this.$(".image_name").html(model.get('image_name'));
                this.$(".redirection_status").html(locale.started);
                this.$(".action-2").removeAttr("disabled");
                this.$(".action-2")[0].disabled = false;
                this.$(".action-2").removeClass("disabled");
                this.$(".action-1").attr("disabled", "disabled");
                this.$(".action-1")[0].disabled = true;
                this.$(".action-1").addClass("disabled");
            } else {
                this.$(".session_index").html("N/A");
                var v = locale['status_'+value];
                this.$(".redirection_status").html(locale.halted+(v?v:""));
                if(value == 0) {
                    this.$(".action-2").attr("disabled", "disabled");
                    this.$(".action-2")[0].disabled = true;
                    this.$(".action-2").addClass("disabled");
                }
            }

        },

        changeInImages: function() {
            var context = this;
            if(this.collection.length == 0 && this.model.get('image_name') == "") {
                this.$(".image_name").html("");
                this.$(".action-1").attr("disabled", "disabled");
                this.$(".action-1")[0].disabled = true;
                this.$(".action-1").addClass("disabled");
                this.$(".action-2").attr("disabled", "disabled");
                this.$(".action-2")[0].disabled = true;
                this.$(".action-2").addClass("disabled");
                return;
            }
            if(this.model.get('redirection_status') != 0) {
                this.$(".action-2").removeAttr("disabled");
                this.$(".action-2")[0].disabled = false;
                this.$(".action-2").removeClass("disabled");

                // console.log(this.collection.pluck('image_name'), this.model.get('image_name'));

                // var selectedImageModel = this.collection.find(function(model){ 
                //     return (model.get('image_name') == context.model.get('image_name'));
                // });

                // console.log(JSON.stringify(selectedImageModel.toJSON()));

                this.$(".image_name").data("image_type", this.model.get('media_type'));
                this.$(".image_name").data("image_index", this.model.get('media_index'));
                return; //Session active, so do not allow change here. Or empty collection of images so no point changing
            }
            if(this.collection.pluck('image_type').indexOf(this.model.get('media_type')) == -1) {
                this.$(".image_name").html("");
                return;

            } //No images for this media type
            this.$(".image_name").html("<select class='image_list'></select>");
            this.collection.each(function(image){
                if(image.get('image_type') == context.model.get('media_type')) {
                        context.$(".image_list").append("<option value='"+image.get('image_index')+"'>"+image.get('image_name')+"</option>");
                }
            })
            context.$(".action-1").removeAttr("disabled");
            context.$(".action-1").removeClass("disabled");
            context.$(".action-1")[0].disabled = false;
        },

        startRedirection: function() {
            var selectedImage = this.$(".image_list").val();
            var selectedImageName = this.$(".image_list option:selected").text();

            if(selectedImage === null || selectedImage === undefined) {
                alert("Not a valid image");
                return;
            }
            var context = this;
            var selectedImageModel = this.collection.find(function(model){ 
                return (model.get('image_name') == selectedImageName);
            });

            context.$(".action-1").attr("disabled", "disabled");
            context.$(".action-1")[0].disabled = true;
            context.$(".action-1").addClass("disabled");

            $.ajax({
                url: "/api/settings/media/remote/start-media",
                type: "POST",
                contentType: "application/json",
                data: JSON.stringify({
                    //media_type: context.model.get('media_type'), 
                    image_name: selectedImageName,
                    image_type: context.model.get('media_type') || selectedImageModel.get('image_type'),
                    image_index: selectedImageModel.get('image_index')
                    //start_bit: 
                }),
                processData: false,
                success: function() {
                    context.$(".image_name").html(selectedImageModel.get('image_name'));
                    context.$(".image_name").data("image_index",selectedImageModel.get('image_index'));
                    context.$(".image_name").data("image_type",selectedImageModel.get('image_type'));
                    context.$(".action-2").removeAttr("disabled");
                    context.$(".action-2")[0].disabled = false;
                    context.$(".action-2").removeClass("disabled");
                    context.model.collection.fetch();
                    context.collection.fetch();
                    //context.changeInImages.call(context);
                    Backbone.history.loadUrl(Backbone.history.fragment);
                },
                error: function(e,x,s){
                    context.$(".action-1").removeAttr("disabled");
                    context.$(".action-1")[0].disabled = false;
                    context.$(".action-1").removeClass("disabled");
                    context.model.collection.fetch();
                    context.collection.fetch();
                    app.HTTPErrorHandler(e,x,s);
                }
            });
        },

        stopRedirection: function() {
            var context = this;
            context.$(".action-2").attr("disabled", "disabled");
            context.$(".action-2")[0].disabled = true;
            context.$(".action-2").addClass("disabled");

            $.ajax({
                url: "/api/settings/media/remote/stop-media",
                type: "POST",
                contentType: "application/json",
                data: JSON.stringify({
                    //media_type: context.model.get('media_type'), 
                    image_name: context.$(".image_name").text(),
                    image_type: context.$(".image_name").data("image_type"),
                    image_index: context.$(".image_name").data("image_index")
                    //start_bit: 
                }),
                processData: false,
                success: function() {
                    context.$(".action-1").removeAttr("disabled");
                    context.$(".action-1")[0].disabled = false;
                    context.$(".action-1").removeClass("disabled");
                    //context.changeInImages.call(context);
                    context.model.collection.fetch();
                    context.collection.fetch();
                    //context.changeInImages.call(context);
                    Backbone.history.loadUrl(Backbone.history.fragment);
                },
                error: function(e,x,s){
                    context.$(".action-2").removeAttr("disabled");
                    context.$(".action-2")[0].disabled = false;
                    context.$(".action-2").removeClass("disabled");
                    context.model.collection.fetch();
                    context.collection.fetch();
                    app.HTTPErrorHandler(e,x,s);
                }
            });
        },

        removeItem: function() {
            this.$el.remove();
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
            return {
                locale: locale,
                model: this.model
            };
        }

    });

    return view;

});
