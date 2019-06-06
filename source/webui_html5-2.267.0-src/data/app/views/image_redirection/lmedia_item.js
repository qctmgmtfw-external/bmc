define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/lmedia',
//template
'text!templates/image_redirection/lmedia_item.html'],

function($, _, Backbone, app, locale, LmediaTemplate) {

    var view = Backbone.View.extend({

        tagName: "tr",

        template: _.template(LmediaTemplate),

        initialize: function() {
            this.model.bind("destroy", this.removeItem, this);
            this.model.bind("change:image_name", this.updateImageName, this);
            this.model.bind("change:redirection_status", this.updateRedirectionStatus, this);
            this.collection.bind("add reset remove", this.changeInImages, this);
            this.customObj;
        },

        events: {
            "click .help-link": "toggleHelp",
            "click .action-1": "startRedirection",
            "click .action-2": "stopRedirection",
            "click .action-3": "uploadImage",
            "click .action-4": "deleteImage",
            "change .media_upload": "uploadImage"
        },

        beforeRender: function() {},

        afterRender: function() {
            this.changeInImages.call(this);
            if(!app.isVMedia()){
               $(".content .btn, .content .media_upload").addClass("disable_a_href"); 
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
            if(this.model.get('redirection_status') == 1) {
                this.$(".action-2").removeAttr("disabled");
                this.$(".action-2")[0].disabled = false;
                this.$(".action-2").removeClass("disabled");

                this.$(".image_name").data("image_type", this.model.get('media_type'));
                this.$(".image_name").data("image_index", this.model.get('media_index'));
                return; //Session active, so do not allow change here. Or empty collection of images so no point changing
            }
            if(this.model.get('media_type') == 1) { //CD
                if(!this.collection.hasCDImage()) {
                    this.$(".image_name").html("");
                    return;
                } //No images for this media type . 

            } else {
                if(!this.collection.hasFDHDDImage()) {
                    this.$(".image_name").html(""); 
                    return; //No images for this media type . 
                }
                
            }
            this.$(".image_name").html("<select class='image_list'></select>");
            this.collection.each(function(image){
                var ext = image.get('image_name').toLowerCase().split('.').pop();
                if(context.model.get('media_type') == 1 && (ext == "iso" || ext == "nrg")) {
                        context.$(".image_list").append("<option value='"+image.get('image_index')+"'>"+image.get('image_name')+"</option>");
                } else if(context.model.get('media_type') != 1 && (ext == "ima" || ext == "img")) {
                        context.$(".image_list").append("<option value='"+image.get('image_index')+"'>"+image.get('image_name')+"</option>");
                }
            })
            context.$(".action-1").removeAttr("disabled");
            context.$(".action-1")[0].disabled = false;
            context.$(".action-1").removeClass("disabled");
        },

        startRedirection: function() {
            var selectedImage = this.$(".image_list option:selected").text();
            if(selectedImage === null || selectedImage === undefined) {
                alert("Not a valid image");
                return;
            }
            var context = this;
            var selectedImageModel = this.collection.find(function(model){ 
                return (model.get('image_name') == selectedImage);
            });


            context.$(".action-1").attr("disabled", "disabled");
            context.$(".action-1")[0].disabled = true;
            context.$(".action-1").addClass("disabled");

            $.ajax({
                url: "/api/settings/media/local/start-media",
                type: "POST",
                contentType: "application/json",
                data: JSON.stringify({
                    //media_type: context.model.get('media_type'), 
                    image_type: context.model.get('media_type') || selectedImageModel.get('image_type'),
                    image_index: selectedImageModel.get('image_index'),
                    image_name: selectedImage
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
                    app.HTTPErrorHandler(e,x,s);
                    context.model.collection.fetch();
                    context.collection.fetch();
                }
            });
        },

        stopRedirection: function() {
            var context = this;
            context.$(".action-2").attr("disabled", "disabled");
            context.$(".action-2")[0].disabled = true;
            context.$(".action-2").addClass("disabled");

            $.ajax({
                url: "/api/settings/media/local/stop-media",
                type: "POST",
                contentType: "application/json",
                data: JSON.stringify({
                    //media_type: context.model.get('media_type'), 
                    image_type: context.model.get('media_type') || context.$(".image_name").data("image_type"),
                    image_index: context.$(".image_name").data("image_index")//,
                    //image_name: selectedImage
                    //start_bit: 
                }),
                processData: false,
                success: function() {
                    context.$(".action-1").removeAttr("disabled");
                    context.$(".action-1")[0].disabled = false;
                    context.$(".action-1").removeClass("disabled");
                    context.model.collection.fetch();
                    //context.changeInImages.call(context);
                    context.collection.fetch();
                    //context.changeInImages.call(context);
                    Backbone.history.loadUrl(Backbone.history.fragment);
                },
                error: function(e,x,s){
                    context.$(".action-2").removeAttr("disabled");
                    context.$(".action-2")[0].disabled = false;
                    context.$(".action-2").removeClass("disabled");
                    app.HTTPErrorHandler(e,x,s);
                    context.model.collection.fetch();
                    context.collection.fetch();
                }
            });
        },

        uploadImage: function(ev) {
            var context = this;
            var el = $(ev.currentTarget);
            var name = $(el)[0].files[0].name;
            var ext = name.split('.').pop().toLowerCase();
            if(this.model.get("media_type") == 1 && ext != "iso" && ext != "nrg") {
                alert("Invalid CD/DVD image. Only ISO or NRG is supported");
                return;
            }
            if(this.model.get("media_type") == 2 && ext != "ima" && ext != "img") {
                alert("Invalid Floppy image. Only IMA or IMG is supported");
                return;
            }
            if(this.model.get("media_type") == 4 && ext != "ima" && ext != "img") {
                alert("Invalid Harddisk image. Only IMA or IMG is supported");
                return;
            }

            if(this.collection.pluck("image_name").indexOf(name) != -1) {
                alert("Image with same name already exists!");
                return;
            }

            $.ajax({
                url: "/api/settings/media/local/lmediasizecheck",
                type: "POST",
                contentType: "application/json",
                data: JSON.stringify({
                    image_size: $(el)[0].files[0].size
                }),
                processData: false,
                success: function() {
            var data = new FormData();
            data.append('lmedia_image', $(el)[0].files[0]);
            lmediaViewObj = context.customObj;
            lmediaViewObj.stopPoll();

            $.ajax({
                url: "/api/settings/media/local/file",
                type: "POST",
                contentType: false,
                processData: false,
                dataType: "json",
                cache: false,
                data: data,
                xhr: function() {
                    var cxhr = $.ajaxSettings.xhr();
                    if(cxhr.upload) {
                        cxhr.upload.addEventListener('progress', function(e){context.uploadProgress.call(context, e);}, false);
                    }
                    return cxhr;
                },
                success: function() {
                    var el = context.$(".media_upload").parent();
                    el.css("background-image","none");
                    context.collection.fetch();
                    //context.changeInImages.call(context);
                    lmediaViewObj.poll();
                    Backbone.history.loadUrl(Backbone.history.fragment);
                },
                error: function(e,x,s) {
                    var el = context.$(".media_upload").parent();
                    el.css("background-image","linear-gradient(to right, rgba(255, 0, 0, .5) 100%, #transparent 0%)");
                    //el.css("background-image","rgba(255, 0, 0, .5)");
                    app.HTTPErrorHandler(e,x,s);
                    lmediaViewObj.poll();
                }
            });
                },
                error: function(e,x,s){
                    context.$(".action-2").removeAttr("disabled");
                    context.$(".action-2")[0].disabled = false;
                    context.$(".action-2").removeClass("disabled");
                    app.HTTPErrorHandler(e,x,s);
                }
            });
        },

        uploadProgress: function(e) {
            if(e.lengthComputable) {
                //context.$("#upload_progress").attr({"aria-valuenow": e.loaded, "aria-valuemax": e.total});
                //context.$("#upload_progress").css("width", (e.loaded/e.total)*50+"%");
                var el = this.$(".media_upload").parent();
                el.css("background-image","linear-gradient(to right, darkseagreen "+(e.loaded/e.total)*100+"%, transparent 0%)");
                console.log(e.loaded, e.total);
                //$(".progress-info").html("Uploading "+Math.ceil((e.loaded/e.total)*100)+"%");
            }
        },

        deleteImage: function() {
            var context = this;
            if(this.$(".image_list").length) {
                if(!confirm("Are you sure to delete the selected image?")) return;

                $.ajax({
                    url: "/api/settings/media/local/image?"+
                            "&image_index="+context.$(".image_list").val()+
                            "&image_name="+(context.$(".image_list option:selected").text() || context.$(".image_name").text()),
                    type: "DELETE",
                    success: function() {
                        context.collection.fetch();
                        //context.changeInImages.call(context);
                        Backbone.history.loadUrl(Backbone.history.fragment);
                    },
                    error: function(e,x,s) {
                        app.HTTPErrorHandler(e,x,s);
                    }
                });

            } else {
                alert("No image to delete!");
                //TODO: check for active redirection image
            }


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
