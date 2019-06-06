define(["jquery", "underscore", "backbone", "models//system_inventory_cpu_info"],

function($, _, Backbone, SystemInventoryCpuInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/system_inventory/cpu_info"
        },

        model: SystemInventoryCpuInfoModel

    });

    return new collection();

});
