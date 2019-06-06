define(['jquery', 'underscore', 'backbone', 
        'models/runtime-configuration'
    ],
    function($, _, Backbone, RuntimeConfigurationModel) {

        var collection = Backbone.Collection.extend({

            url: "/api/configuration/runtime",

            model: RuntimeConfigurationModel,

            isFeatureEnabled: function(feature) {
                var item = this.findWhere({
                    "feature": feature
                });
                console.log('fe', item);
                return (item ? item.get('enabled') : 0);
            },

            setFeatureState: function(feature, state) {
                var item = this.find(function(model) {
                    return model.get("feature") == feature;
                });
                item.set('enabled', state);
                this.trigger('runtime_feature_update');
            }

        });

        return new collection();

    });
