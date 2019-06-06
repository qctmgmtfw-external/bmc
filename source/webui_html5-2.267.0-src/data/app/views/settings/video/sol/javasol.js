define(['jquery', 'underscore', 'backbone', 'app',
//localize
'i18n!strings/nls/javasol',
//template
'text!templates/settings/video/sol/javasol.html'
//plugins
],

function($, _, Backbone, app, locale, JavasolTemplate) {

    var view = Backbone.View.extend({

        template: _.template(JavasolTemplate),

        initialize: function() {
        },

        events: {
            "click #download": "download",
            "click .help-link": "toggleHelp"
        },

        beforeRender: function() {

        },

        afterRender: function() {
            if (sessionStorage.privilege_id < CONSTANTS["ADMIN"] && sessionStorage.kvm_access != 1) {
                app.disableAllbutton();
            }
        },
        download: function() {
			windowURL = window.location.href.split("/");
			protocol =  windowURL[0];
			protocol =  protocol.replace(/\:/g,"");
			windowURL = windowURL[2];
			if (windowURL.indexOf("]") != -1) {
				externalIP = windowURL.split("]");
				externalIP = externalIP[0] + "]";
			} else {
				externalarr = windowURL.split(":");
				externalIP = externalarr[0];
				externalport = (windowURL.indexOf(":")== -1 ) ? 80 : externalarr[1] ; 
			}
			$.ajax({
				type: "GET",
				url: "/api/settings/javasol?protocol="+protocol+"&externalIP="+externalIP+"&externalport="+externalport,
				success: function(response, status, xhr) {
					var filename = "jsol";
					var blob = new Blob([response], {
						type: 'application/x-java-jnlp-file'
					});
					if (typeof window.navigator.msSaveBlob !== 'undefined') {
						window.navigator.msSaveBlob(blob, filename);
					} else {
						var URL = window.URL || window.webkitURL;
						var downloadURL = URL.createObjectURL(blob);
						var a = document.createElement("a");
						if (typeof a.download == "undefined") {
							window.location = downloadURL;
						} else {
							$(a).attr("href", downloadURL);
							$(a).attr("download", filename);
							$(a).attr("target", "_blank");
							$(a).css({
								"position": "absolute",
								"opacity": "0"
							});
							document.body.appendChild(a);
							a.click();
						}
					}
				},
				error: function(xhr, textStatus, errorThrown) {         
				}
			});
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
            $('.tooltip').hide();
            if (help_items.hasClass('hide')) {
                help_items.removeClass('hide');
            } else {
                help_items.addClass('hide');
            }
        },

		serialize: function() {
			return {
				locale: locale
			};
		}

    });

    return view;

});
