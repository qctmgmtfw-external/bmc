define(["jquery", "underscore", "backbone", "models//raid_logical_physical_device_info"],

function($, _, Backbone, RaidLogicalPhysicalDeviceInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/raid_management/physical_device_info"
        },

        model: RaidLogicalPhysicalDeviceInfoModel

    });

    return new collection();

});
