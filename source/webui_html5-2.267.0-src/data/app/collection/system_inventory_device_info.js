define(["jquery", "underscore", "backbone", "models//system_inventory_device_info"],

function($, _, Backbone, SystemInventoryDeviceInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/system_inventory/device_count"
        },

        model: SystemInventoryDeviceInfoModel

    });

    return new collection();

});
