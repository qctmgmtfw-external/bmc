// This is the runtime configuration file.  It complements the Gruntfile.js by
// supplementing shared properties.
require.config({
    //##LANG_TAGS
    paths: {
        // Make vendor easier to access.
        "vendor": "../vendor",

        // Almond is used to lighten the output filesize.
        "almond": "../vendor/bower/almond/almond",

        // Opt for Lo-Dash Underscore compatibility build over Underscore.
        "underscore": "../vendor/bower/lodash/dist/lodash",

        "i18n": "../vendor/bower/requirejs-i18n/i18n",
        "text": "../vendor/bower/text/text",

        // Map remaining vendor dependencies.
        "jquery": "../vendor/bower/jquery/dist/jquery",
        "backbone": "../vendor/bower/backbone/backbone",
        "layoutmanager": "../vendor/bower/layoutmanager/backbone.layoutmanager",
        "bower": "../vendor/bower",
        "tooltip": "../vendor/bower/bootstrap/js/tooltip",
        "dropdown": "../vendor/bower/bootstrap/js/dropdown",
        "alert": "../vendor/bower/bootstrap/js/alert",
        "modal": "../vendor/bower/bootstrap/js/modal",
        "backbone-validation": "../vendor/bower/backbone-validation/dist/backbone-validation-amd",
        "moment": "../vendor/bower/moment/moment",
        "moment-timezone": "../vendor/bower/moment-timezone/builds/moment-timezone-with-data.min",
        "timezone-meta-data": "./libs/timezone-meta",
        "datepicker": "../vendor/bower/eonasdan-bootstrap-datetimepicker/build/js/bootstrap-datetimepicker.min",
        "collapse": "../vendor/bower/bootstrap/js/collapse",
        "raphael": "../vendor/bower/raphael/raphael",
        "morris": "../vendor/bower/morris.js/morris",
        //"jvectormap": "../vendor/bower/jvectormap/jquery-jvectormap",
        "fullcalendar": "../vendor/bower/fullcalendar/fullcalendar",
        //"knob": "../vendor/bower/jquery-knob/js/jquery.knob",
        //"wysihtml5": "../vendor/bower/bootstrap3-wysihtml5-bower/dist/bootstrap3-wysihtml5.all.min", //TODO: locale for wysi plugin
        "iCheck": "../vendor/bower/iCheck/icheck",
        //"inputmask": "../vendor/bower/jquery.inputmask/dist/jquery.inputmask.bundle",
        //"rangeslider": "../vendor/bower/ion.rangeSlider/js/ion.rangeSlider",
        "slimscroll": "../vendor/bower/jquery.slimscroll/jquery.slimscroll",
        //"pace": "../vendor/bower/pace/pace",
        //"boxrefresh": "./libs/boxrefresh",
        "center": "./libs/center",
        "resize": "./libs/resize",
        "todo": "./libs/todo",
        "treeview": "./libs/treeview",
        //"jvmlib": "../vendor/bower/jvectormap/lib",
        "sparkline": "../vendor/bower/kapusta-jquery.sparkline/dist/jquery.sparkline",
        "sortable": "../vendor/bower/Sortable/Sortable",
        "easypiechart": "../vendor/bower/jquery.easy-pie-chart/dist/jquery.easypiechart",
        "chartjs": "../vendor/bower/Chart.js/Chart",
        //"db": "../vendor/bower/db.js/src/db",
        //"prettify": "../vendor/bower/google-code-prettify/bin/prettify.min",
        //"cm": "../vendor/bower/codemirror/lib/codemirror",
        //"codemirror-python": "../vendor/bower/codemirror/mode/python/python",
        "notify": "../vendor/bower/notifyjs/dist/notify-combined",
        "footable": "../vendor/bower/footable/dist/footable.all.min",
        "c3": "../vendor/bower/c3/c3",
        "d3": "../vendor/bower/d3/d3",
        "videoplayback": "./libs/videoplayback",
		"tree-view":"../vendor/bower/bootstrap-treeview/dist/bootstrap-treeview.min"
        //"ydn.db": "../vendor/bower/ydn.db/jsc/ydn.db-dev"
        //##KVM_TAGS
        //##MEDIA_TAGS
	    //##OEM_TAGS
    },

    shim: {
        // This is required to ensure Backbone works as expected within the AMD
        // environment.
        "backbone": {
            // These are the two hard dependencies that will be loaded first.
            deps: ["jquery", "underscore"],

            // This maps the global `Backbone` object to `require("backbone")`.
            exports: "Backbone"
        },

        //"datepicker": ["jquery"],

        "bower/bootstrap/js/tab": ["jquery"],
        "bower/bootstrap/js/dropdown": ["jquery"],
        "bower/bootstrap/js/tooltip": ["jquery"],
        "bower/bootstrap/js/modal": ["jquery"],
        "collapse": ["jquery"],
        "bower/iCheck/icheck": ["jquery"],

        "c3": ["d3"],

        "notify": ["jquery"],
        "morris": ["jquery"],
        "jvectormap": ["jquery", "bower/jvectormap/jquery-mousewheel"
            /*, "jvmlib/jvectormap"
                    , "jvmlib/abstract-canvas-element"
                    , "jvmlib/abstract-element"
                    , "jvmlib/abstract-shape-element"
                    , "jvmlib/color-scale"
                    , "jvmlib/data-series"
                    , "jvmlib/numeric-scale"
                    , "jvmlib/ordinal-scale"
                    , "jvmlib/proj"
                    , "jvmlib/simple-scale"
                    , "jvmlib/svg-canvas-element"
                    , "jvmlib/svg-circle-element"
                    , "jvmlib/svg-element"
                    , "jvmlib/svg-group-element"
                    , "jvmlib/svg-path-element"
                    , "jvmlib/svg-shape-element"
                    , "jvmlib/vector-canvas"
                    , "jvmlib/vml-canvas-element"
                    , "jvmlib/vml-circle-element"
                    , "jvmlib/vml-element"
                    , "jvmlib/vml-group-element"
                    , "jvmlib/vml-path-element"
                    , "jvmlib/vml-shape-element"
                    , "jvmlib/world-map"*/
        ],
        "fullcalendar": ["jquery"],
        "knob": ["jquery"],
        "inputmask": ["jquery"],
        "rangeslider": ["jquery"],
        "slimscroll": ["jquery"],
        "boxrefresh": ["jquery"],
        "center": ["jquery"],
        "resize": ["jquery"],
        "todo": ["jquery"],
        "treeview": ["jquery"],
        "sparkline": ["jquery"],
        "easypiechart": ["jquery"],
        "footable": ["jquery"],
		"tree-view":["jquery"],
        "wysihtml5": ["backbone"],
        "bower/layoutManager/backbone.layoutmanager": ["backbone"],
        "bower/backbone-validation/dist/backbone-validation-amd": ["backbone"],
        "moment-timezone" : ["moment"],
        "timezone-meta-data" : ["moment-timezone"],
        "datepicker": ["collapse", "moment"]

        //##KVM_SHIMS
        //##MEDIA_SHIMS
        //##OEM_SHIMS
    }
});
