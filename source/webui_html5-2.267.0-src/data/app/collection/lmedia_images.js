define(["jquery", "underscore", "backbone", "models//lmedia_img"],

function($, _, Backbone, LmediaImgModel) {

    var collection = Backbone.Collection.extend({

        url: function() {
            return "/api/settings/media/local/images"
        },

        model: LmediaImgModel,

        hasCDImage: function() {
        	var im = this.pluck('image_name');
        	var list = _.filter(im, function(i){
        		var ext = i.toLowerCase().split('.').pop();
        		return ext == "iso" || ext == "nrg";
        	});
        	return list.length;
        },

        hasFDHDDImage: function() {
        	var im = this.pluck('image_name');
        	var list = _.filter(im, function(i){
        		var ext = i.toLowerCase().split('.').pop();
        		return ext == "ima" || ext == "img";
        	});
        	return list.length;
        }

    });

    return new collection();

});
