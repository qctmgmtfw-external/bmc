define(['jquery', 'underscore',
	//plugins
	'tooltip',
	'collapse',
	'iCheck'],
	function ($, _){

		var utils = {

			hoverSupport: function(){
				//Add hover support for touch devices
				$('.btn').bind('touchstart', function() {
					$(this).addClass('hover');
				}).bind('touchend', function() {
					$(this).removeClass('hover');
				});
			},

			activateTooltips : function(){
				$("[data-toggle='tooltip']").tooltip();
			},

			addCollapseEvents : function() {
				$("[data-widget='collapse']").click(function() {
					//Find the box parent        
					var box = $(this).parents(".box").first();
					//Find the body and the footer
					var bf = box.find(".box-body, .box-footer");
					if (!box.hasClass("collapsed-box")) {
						box.addClass("collapsed-box");
						bf.slideUp();
					} else {
						box.removeClass("collapsed-box");
						bf.slideDown();
					}
				});
			},

			listenRemoveEvents : function() {
				$("[data-widget='remove']").click(function() {
					//Find the box parent        
					var box = $(this).parents(".box").first();
					box.slideUp();
				});
			},

			initICheck : function() {
				$("input[type='checkbox'], input[type='radio']").iCheck({
					checkboxClass: 'icheckbox_minimal',
					radioClass: 'iradio_minimal'
				});
			}

		};

		return utils;

	});