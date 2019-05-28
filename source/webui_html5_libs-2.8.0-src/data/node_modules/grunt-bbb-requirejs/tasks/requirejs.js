/*
 * grunt-bbb-requirejs
 * http://gruntjs.com/
 *
 * Copyright (c) 2013 Tim Branyen, Tyler Kellen, contributors
 * Licensed under the MIT license.
 */
module.exports = function(grunt) {
  const ENV = process.env;
  const CWD = process.cwd();

  const fs = require("fs");
  const path = require("path");

  // External libs.
  const requirejs = require("requirejs");
  const Compiler = require("es6-module-transpiler").Compiler;

  // Shorthand Lo-Dash and the Grunt logger.
  var _ = grunt.util._;
  var log = grunt.log;

  // Convert the contents passed to CommonJS and then return.
  function toCommonJS(moduleName, contents, commonJs) {
    return commonJs.convert(moduleName, String(contents));
  }

  // TODO: extend this to send build log to grunt.log.ok / grunt.log.error by
  // overriding the r.js logger (or submit issue to r.js to expand logging
  // support)
  requirejs.define("node/print", [], function() {
    return function print(msg) {
      if (msg.substring(0, 5) === "Error") {
        grunt.log.errorlns(msg);
        grunt.fail.warn("RequireJS failed.");
      } else {
        grunt.log.oklns(msg);
      }
    };
  });

  grunt.registerTask("list", "Show module dependencies.", function(prop) {
    var options = grunt.config("requirejs") || {};
    var baseUrl = options.baseUrl || "app";
    var done = this.async();
    var exports = {};
    
    exports.init = function(grunt) {
      var exports = {};
      var _ = grunt.util._;

      exports.list = function(appDir, done) {
        var jsRegExp = /\.js$/;

        requirejs.tools.useLib(function(require) {
          require(["parse", "commonJs"], function(parse, commonJs) {
            var deps = {};
            var files = [];

            // Recursively find all files inside the application directory
            function recurse(dir) {
              fs.readdirSync(dir).forEach(function(name) {
                var subdir = path.resolve(dir, name);
                var stats = fs.statSync(subdir);
                
                if (stats.isDirectory()) {
                  recurse(subdir);
                } else if (jsRegExp.test(name)) {
                  files.push(subdir);
                }
              });
            }

            // Start with the app directory e.g. app/
            recurse(appDir);

            files.forEach(function(name) {
              var contents = fs.readFileSync(name, "utf8");
              var shortname = name.slice(name.indexOf(appDir));
              var dep, all;

              if (options.es6) {
                // Convert from ES6.
                contents = new Compiler(contents, moduleName.slice(-2, 0)).toAMD();
              } else {
                // Convert from CommonJS.
                contents = commonJs.convert(name, contents);
              }

              try {
                dep = parse.findDependencies(name, contents)
              } catch (ex) {
                console.log(ex);
              }

              if (dep && dep.length) {
                all = deps[shortname] = parse.findDependencies(name, contents);

                // Remove `require`, `module`, `exports`.
                all.splice(all.indexOf("require"), 1);
                all.splice(all.indexOf("module"), 1);
                all.splice(all.indexOf("exports"), 1);
              }
            });
            
            console.log(exports.tree(deps));
            done();
          });
        });
      };

      exports.tree = function(obj) {
        var tree = [""];

        function spaces(len, end, start) {
          start = start || " ";
          end = end || " ";

          if (!start) {
            return Array(len+1).join(Array(3).join(end));
          } else {
            return Array(len+1).join(start + Array(2).join(end));
          }
        }

        function traverse(obj, depth) {
          _.each(obj, function(val, key) {
            var len;

            if (_.isArray(val)) {
              tree.push("\u251c" + spaces(depth) + " " + key);

              len = val.length;

              _.each(val, function(val, i) {
                if (_.isString(val)) {
                  if (i == len-1) {
                    tree.push("\u2502 " + spaces(depth+1, "\u2500", "\u2514") + " "
                      + val);
                  } else {
                    tree.push("\u2502 " + spaces(depth+1, "\u2500", "\u251c") + " "
                      + val);
                  }
                } else if (_.isObject(val)) {
                  traverse(obj, depth+1);
                }
              });

              tree.push("\u2502");

            } else if (_.isObject(val)) {
              tree.push(spaces(depth) + key);
              traverse(val, depth+1);
            } else {
              tree.push(spaces(depth) + key);
            }

          });
        }

        traverse(obj, 0);

        tree.pop();

        return tree.join("\n");
      };

      return exports;
    };

    exports.init(grunt).list(path.normalize(baseUrl + "/"), done);
  });

  grunt.registerMultiTask("requirejs", "Build with r.js.", function() {
    // Make the current task reusable within the async callbacks below.
    var task = this;

    // Put this task into `async-mode`, cause we're gonna need it.
    var done = task.async();

    // Cache this function so that we can reuse it within RequireJS's
    // utilities.
    var nodeRequire = require;

    // Access RequireJS's internal utilities.
    requirejs.tools.useLib(function(require) {
      // Get access to the CommonJS convert utility.
      require(["commonJs"], function(commonJs) {
        var options = task.options({
          // Include the main ration file.
          mainConfigFile: "app/config.js",

          // Setting the base url to the distribution directory allows the
          // Uglify minification process to correctly map paths for Source
          // Maps.
          baseUrl: "dist/app",

          // Default location for where scripts are modules.
          moduleDirs: ["dist/app", "dist/test"],

          // Include Almond to slim down the built filesize.
          name: "almond",

          // Set the Bootstrap as the main entry point.
          include: ["main"],
          insertRequire: ["main"],

          // Since we bootstrap with nested `require` calls this option allows
          // R.js to find them.
          findNestedDependencies: true,

          // Wrap everything in an IIFE.
          wrap: true,

          // Output file.
          out: "dist/source.min.js",

          // Enable Source Map generation.
          generateSourceMaps: true,

          // Do not preserve any license comments when working with source maps.
          // These options are incompatible.
          preserveLicenseComments: false,

          // Minify using UglifyJS.
          optimize: "uglify2",

          // Show warnings
          logLevel: 2,

          // Ensure modules are inserted
          skipModuleInsertion: false,

          // If the contents do not contain a define call, then wrap with 
          onBuildRead: function (moduleName, filePath, contents) {
            var moduleDir = _.find(options.moduleDirs, function(dir) {
              return filePath.indexOf(path.resolve(CWD, dir)) === 0;
            });

            // Do not execute the conversion on files that do not exist in the
            // main application path.
            if (!moduleDir) {
              return contents;
            }

            if (options.es6) {
              return new Compiler(contents, moduleName.slice(-2, 0)).toAMD();
            }

            // Convert CommonJS to AMD.
            var wrapped = commonJs.convert(moduleName, contents);
            var noDefine = wrapped.indexOf("define(") === -1;
            var noConfig = wrapped.indexOf("require.config") === -1;

            if (noDefine && noConfig) {
              wrapped = [
                "define(function(require, exports, module) {",
                  wrapped,
                "});"
              ].join("\n");
            }

            return wrapped;
          }
        });

        // Only log the options during verbose mode.
        grunt.verbose.writeflags(options, "Options");

        // Full optimiziation.
        requirejs.optimize(options, done);
      });
    });
  });
};
