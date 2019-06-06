define(["jquery", "underscore", "backbone", "models//system_inventory_dimm_info"],

function($, _, Backbone, SystemInventoryDimmInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/system_inventory/dimm_info"
        },

        model: SystemInventoryDimmInfoModel

    });

    return new collection();

});
