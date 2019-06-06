define(['jquery', 'underscore', 'backbone',
        //data

        //localize
        'i18n!strings/nls/postcode',
        //template
        'text!templates/postcode.html'
    ],

    function($, _, Backbone, CommonStrings, PostCodeTemplate) {

        var view = Backbone.View.extend({

            template: _.template(PostCodeTemplate),
            active:1,

            initialize: function() {
                console.log("initialize");
                this.getPostCode();
            },

            events: {
                "click .help-link": "toggleHelp"
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
            getPostCode:function()
            {
                $('#currCode').html("N/A");
                $('#prevCode').html("N/A");
                $.ajax({
                    url: "/api/post-code",
                    type: "GET",
                    success: function(response)
                    {
                        for (var i = 0; i< response.length;i++)
                        {
                            if(i==0)
                                $('#currCode').html(response[i].postcode);
                            else if( i==1)
                                $('#prevCode').html(response[i].postcode);
                        }
                    }, dataType: "json"});
            },

            serialize: function() {
                console.log("serialize");

                return {
                    locale: CommonStrings
                };
            }

        });

        return view;

    });
