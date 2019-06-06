module.exports = function(grunt) {
    "use strict";

    grunt.initConfig({
        // Wipe out previous builds and test reporting.
        clean: ["dist/", "test/reports"],

        // Run your source code through JSHint's defaults.
        jshint: {
            options: {
                laxcomma: true,
                '-W086': true,
                '-W030': true,
                '-W069': true,
                '-W004': true,
                '-W051': true,
                '-W041': true,
                '-W099': true
            },
            src: ["app/**/*.js"]
        },

        // This task uses James Burke's excellent r.js AMD builder to take all
        // modules and concatenate them into a single file.
        requirejs: {
            release: {
                options: {
                    mainConfigFile: "app/config.js",
                    generateSourceMaps: false,
                    include: ["main"],
                    out: "dist/source.min.js",
                    optimize: "uglify2",

                    // Since we bootstrap with nested `require` calls this option allows
                    // R.js to find them.
                    findNestedDependencies: true,

                    // Include a minimal AMD implementation shim.
                    name: "almond",

                    // Setting the base url to the distribution directory allows the
                    // Uglify minification process to correctly map paths for Source
                    // Maps.
                    baseUrl: "app",

                    // Wrap everything in an IIFE.
                    wrap: true,

                    // Do not preserve any license comments when working with source
                    // maps.  These options are incompatible.
                    preserveLicenseComments: false
                }
            },
            debug: {
                options: {
                    mainConfigFile: "app/config.js",
                    generateSourceMaps: true,
                    include: ["main"],
                    insertRequire: ["main"],
                    out: "dist/source.min.js",
                    optimize: "uglify2",

                    // Since we bootstrap with nested `require` calls this option allows
                    // R.js to find them.
                    findNestedDependencies: true,

                    // Include a minimal AMD implementation shim.
                    name: "almond",

                    // Setting the base url to the distribution directory allows the
                    // Uglify minification process to correctly map paths for Source
                    // Maps.
                    baseUrl: "app",

                    // Wrap everything in an IIFE.
                    wrap: true,

                    // Do not preserve any license comments when working with source
                    // maps.  These options are incompatible.
                    preserveLicenseComments: false
                }
            }
        },

        // This task simplifies working with CSS inside Backbone Boilerplate
        // projects.  Instead of manually specifying your stylesheets inside the
        // HTML, you can use `@imports` and this task will concatenate only those
        // paths.
        styles: {
            // Out the concatenated contents of the following styles into the below
            // development file path.
            "dist/styles.css": {
                // Point this to where your `index.css` file is location.
                src: "app/styles/index.css",

                // The relative path to use for the @imports.
                paths: ["app/styles"],

                // Rewrite image paths during release to be relative to the `img`
                // directory.
                forceRelative: "/app/img/"
            }
        },

        // Minify the distribution CSS.
        cssmin: {
            release: {
                files: {
                    "dist/styles.min.css": ["dist/styles.css"]
                }
            }
        },

        less: {
            development: {
                options: {
                    paths: ["vendor/bower/bootstrap/less", "vendor/bower/font-awesome/less", "vendor/bower/less-prefixer", "app/css"],
                    yuicompress: false
                },
                files: {
                    "dist/styles.min.css": "app/less/styles.less"
                }
            },
            production: {
                options: {
                    paths: ["vendor/bower/bootstrap/less", "vendor/bower/font-awesome/less", "vendor/bower/less-prefixer", "app/css"],
                    yuicompress: true
                },
                files: {
                    "dist/styles.min.css": "app/less/styles.less"
                }
            }
        },

        server: {
            options: {
                host: "0.0.0.0",
                port: 8000,
                middleware: {
                    //Middleware to update fixtures
                    "sensors$": function(buffer, req, res, next) {
                        var sensors = JSON.parse(String(buffer)),
                            i = 0;
                        for (i = 0; i < sensors.length; i += 6) {
                            //more frequent normal state
                            var s = sensors[i];
                            s.id = i + 1;

                            var rand = Math.ceil(Math.random() * 20) % 20;
                            if (rand == 5) {
                                var rand_state = Math.ceil(Math.random() * 10) % 7; //7 states



                                if (rand_state == 2 || rand_state == 4) {
                                    s.sensor_state = 2;
                                } else if (rand_state != 3) {
                                    s.sensor_state = 3;
                                } else {
                                    s.sensor_state = 1;
                                }

                                switch (rand_state) {
                                    case 0:
                                        s.reading = s.lower_non_recoverable_threshold;
                                        break;
                                    case 1:
                                        s.reading = s.lower_critical_threshold;
                                        break;
                                    case 2:
                                        s.reading = s.lower_non_critical_threshold;
                                        break;
                                    case 3:
                                        s.reading = (s.lower_non_critical_threshold + s.higher_non_critical_threshold) / 2;
                                        break;
                                    case 4:
                                        s.reading = s.higher_non_critical_threshold;
                                        break;
                                    case 5:
                                        s.reading = s.higher_critical_threshold;
                                        break;
                                    case 6:
                                        s.reading = s.higher_non_recoverable_threshold;
                                        break;

                                }
                            }
                            sensors[i] = s;
                        }
                        buffer = new Buffer(JSON.stringify(sensors));
                        next(buffer);
                    },
                    "^/api": function(buffer, req, res, next) {

                        var contentType = "application/json";
                        //console.log(String(buffer));
                        // Compile the source.
                        res.header("Content-type", contentType);
                        next(buffer);

                    },
                    //Middleware to fix images
                    "png$": function(buffer, req, res, next) {
                        var contentType = "image/png";
                        res.header("Content-type", contentType);
                        next(buffer);
                    }
                }
            },

            development: {},

            release: {
                options: {
                    prefix: "dist"
                }
            },

            test: {
                options: {
                    forever: false,
                    port: 8001
                }
            }
        },

        processhtml: {
            debug: {
                files: {
                    "dist/index.html": ["index.html"]
                }
            },
            dist: {
                files: {
                    "dist/index.html": ["index.html"]
                }
            }
        },

        // Move vendor and app logic during a build.
        copy: {
            release: {
                files: [/*
                    {
                        src: ["api/**"],
                        dest: "dist/"
                    }, {
                        src: ["app/templates/**"],
                        dest: "dist/"
                    }, {
                        expand: true,
                        src: "vendor/bower/html5shiv/dist/**",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/libs/js/"
                    }, {
                        expand: true,
                        src: "vendor/bower/respond/dest/*.min.js",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/libs/js/"
                    }, {
                        expand: true,
                        src: "vendor/bower/fullcalendar/fullcalendar.print.css",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/libs/css/"
                    }, */
                    {
                        expand: true,
                        src: "app/fonts/*.woff",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/fonts/"
                    },
                    //{ src: "vendor/**", dest: "dist/" },
                    { 
                    	expand: true,
                    	cwd: "app/libs/", 
                    	src: "**", 
                    	dest: "dist/libs/" 
                    },
                    {
                        expand: true,
                        src: "app/images/**",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/images/"
                    }, {
                        expand: true,
                        src: "vendor/bower/iCheck/skins/square/blue.png",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/"
                    }, {
                        expand: true,
                        src: "vendor/bower/iCheck/skins/square/blue@2x.png",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/"
                    }, {
                        expand: true,
                        src: "vendor/bower/font-awesome/fonts/*.woff",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/fonts/"
                    }, {
                        expand: true,
                        src: "vendor/bower/bootstrap/fonts/*.woff",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/fonts/"
                    }, {
                        expand: true,
                        src: "vendor/bower/ionicons/fonts/*.woff",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/fonts/"
                    }, {
                        expand: true,
                        src: "vendor/footable/css/fonts/*.woff",
                        flatten: true,
                        filter: 'isFile',
                        dest: "dist/fonts/"
                    }
                ]
            },
            debug: {
                files: [
                /*
                {
                    src: ["api/**"],
                    dest: "dist/"
                }, {
                    src: ["app/**"],
                    dest: "dist/"
                }, {
                    expand: true,
                    src: "vendor/bower/html5shiv/dist/**",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/libs"
                }, {
                    expand: true,
                    src: "vendor/bower/respond/dest/*.min.js",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/libs"
                }, */
                {
                    expand: true,
                    src: "app/fonts/**",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/fonts/"
                }, 
                /*
                {
                    src: "vendor/**",
                    dest: "dist/"
                },*/
                { 
                    expand: true,
                    cwd: "app/libs/", 
                    src: "**", 
                    dest: "dist/libs/" 
                },
                {
                    expand: true,
                    src: "app/images/**",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/images/"
                }, {
                    expand: true,
                    src: "vendor/bower/iCheck/skins/square/blue.png",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/"
                }, {
                    expand: true,
                    src: "vendor/bower/iCheck/skins/square/blue@2x.png",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/"
                }, {
                    expand: true,
                    src: "vendor/bower/font-awesome/fonts/*.woff",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/fonts/"
                }, {
                    expand: true,
                    src: "vendor/bower/bootstrap/fonts/*.woff",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/fonts/"
                }, {
                    expand: true,
                    src: "vendor/bower/ionicons/fonts/*.woff",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/fonts/"
                }, {
                    expand: true,
                    src: "vendor/footable/css/fonts/*.woff",
                    flatten: true,
                    filter: 'isFile',
                    dest: "dist/fonts/"
                }]
            }
        },

        compress: {
            release: {
                options: {
                    archive: "dist/source.min.js.gz"
                },

                files: ["dist/source.min.js"]
            }
        },

        // Unit testing is provided by Karma.  Change the two commented locations
        // below to either: mocha, jasmine, or qunit.
        karma: {
            options: {
                basePath: process.cwd(),
                singleRun: true,
                captureTimeout: 7000,
                autoWatch: true,
                logLevel: "ERROR",

                reporters: ["dots", "coverage"],
                browsers: ["PhantomJS"],

                // Change this to the framework you want to use.
                frameworks: ["jasmine"],

                plugins: [
                    "karma-jasmine",
                    "karma-mocha",
                    "karma-qunit",
                    "karma-phantomjs-launcher",
                    "karma-coverage"
                ],

                preprocessors: {
                    "app/**/*.js": "coverage"
                },

                coverageReporter: {
                    type: "lcov",
                    dir: "test/coverage"
                },

                files: [
                    // You can optionally remove this or swap out for a different expect.
                    //"vendor/bower/chai/chai.js",
                    "vendor/bower/jquery/jquery.js",
                    "vendor/bower/jasmine-jquery/lib/jasmine-jquery.js",
                    "vendor/bower/requirejs/require.js",
                    "test/runner.js",

                    {
                        pattern: "app/**/*.*",
                        included: false
                    },
                    // Derives test framework from Karma configuration.
                    {
                        pattern: "test/<%= karma.options.frameworks[0] %>/**/*.spec.js",
                        included: false
                    }, {
                        pattern: "vendor/**/*.js",
                        included: false
                    }
                ]
            },

            // This creates a server that will automatically run your tests when you
            // save a file and display results in the terminal.
            daemon: {
                options: {
                    singleRun: false
                }
            },

            // This is useful for running the tests just once.
            run: {
                options: {
                    singleRun: true
                }
            }
        },

        watch: {
            scripts: {
                files: '**/*.js',
                tasks: ['jshint', 'copy:debug', 'requirejs:debug'],
                options: {
                    spawn: false,
                },
            },
        },

        coveralls: {
            options: {
                coverage_dir: "test/coverage/"
            }
        },

        notify: {
            production: {
                options: {
                    message: "BMCUI2 - Production build complete"
                }
            },
            debug: {
                options: {
                    message: "BMCUI2 - Debug build complete"
                }
            }
        }
    });

    // Grunt contribution tasks.
    grunt.loadNpmTasks("grunt-contrib-clean");
    grunt.loadNpmTasks("grunt-contrib-jshint");
    grunt.loadNpmTasks("grunt-contrib-cssmin");
    grunt.loadNpmTasks("grunt-contrib-copy");
    grunt.loadNpmTasks("grunt-contrib-compress");
    grunt.loadNpmTasks("grunt-contrib-less");
    grunt.loadNpmTasks("grunt-contrib-watch");

    grunt.loadNpmTasks("grunt-notify");
    grunt.loadNpmTasks("grunt-beep");

    // Third-party tasks.
    grunt.loadNpmTasks("grunt-karma");
    grunt.loadNpmTasks("grunt-karma-coveralls");
    grunt.loadNpmTasks("grunt-processhtml");

    // Grunt BBB tasks.
    grunt.loadNpmTasks("grunt-bbb-server");
    grunt.loadNpmTasks("grunt-bbb-requirejs");
    grunt.loadNpmTasks("grunt-bbb-styles");

    // When running the default Grunt command, just lint the code.
    grunt.registerTask("default", [
        "clean",
        //"jshint",
        "processhtml:dist",
        "copy:release",
        "requirejs:release",
        //    "styles",
        //    "cssmin",
        "less:production",
        "notify:production",
        "beep:2"
    ]);

    grunt.registerTask("debug", [
        "clean",
        //"jshint",
        "processhtml:debug",
        "copy:debug",
        "requirejs:debug",
        //    "styles",
        //    "cssmin",
        "less:production",
        "notify:debug",
        "beep:2"
    ]);
};