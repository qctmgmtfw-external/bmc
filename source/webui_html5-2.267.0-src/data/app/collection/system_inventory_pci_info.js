define(["jquery", "underscore", "backbone", "models//system_inventory_pci_info"],

function($, _, Backbone, SystemInventoryPciInfoModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/system_inventory/pci_info"
        },

        model: SystemInventoryPciInfoModel

    });

    return new collection();

});
