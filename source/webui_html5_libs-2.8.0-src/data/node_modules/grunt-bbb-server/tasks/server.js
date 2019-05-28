/*
 * grunt-bbb-server
 * https://github.com/backbone-boilerplate/grunt-bbb-server
 *
 * Copyright 2013 Tim Branyen
 * Licensed under the MIT license.
 */
"use strict";

module.exports = function(grunt) {
  var ENV = process.env;
  var CWD = process.cwd();

  var path = require("path");
  var fs = require("fs");
  var https = require("https");

  // External libs.
  var express = require("express");
  var requirejs = require("requirejs");
  var gzip = require("gzip-js");
  var httpProxy = require("http-proxy");
  var Compiler = require("es6-module-transpiler").Compiler;

  // Shorthand Lo-Dash.
  var _ = grunt.util._;

  grunt.registerMultiTask("server", "Run development server.", function() {

    var options = this.options({
      // Fundamentals.
      favicon: "favicon.ico",
      index: "index.html",

      // Should this router automatically handle pushState requests.
      pushState: true,

      // Url root paths.  These are useful to determine where application vs
      // vendor code exists in the path.
      root: "/",
      moduleDirs: ["app", "test"],

      // Where on the filesystem files are, can be absolute or relative.
      prefix: ".",

      // Should this server exist forever or die immediately after all tasks
      // are done.
      forever: true,

      // Controls how the server is run.
      ssl: ENV.SSL || false,
      host: ENV.HOST || "127.0.0.1",
      port: ENV.PORT || 8000,

      proxy: {},

      // Any express-compatible server will work here.
      server: null,

      // Register default compiler mappings.
      middleware: {
        // Script pre-processors.
        //"\.coffee$": require("grunt-lib-coffee").compile,
        //"\.ts$": require("grunt-lib-typescript").compile,
        "\.js$": function(buffer, req, res, next) {
          var moduleDir = _.find(options.moduleDirs, function(dir) {
            return req.url.indexOf(options.root + dir) === 0;
          });

          // Only process JavaScript that are required modules, this means
          // bailing out early if not in the module path.
          if (!moduleDir || req.url.indexOf(options.root + "vendor") > 0) {
            return next();
          }

          // The module name is just the JavaScript file stripped of the
          // host and location.
          var moduleName = req.url.split(moduleDir)[1];
          moduleName = moduleName.slice(1);

          // I'd rather work with strings here, since buffers aren't widely
          // supported.
          buffer = String(buffer);

          // If ES6 modules are enabled in the configuration, convert by
          // default.
          if (options.es6) {
            buffer = new Compiler(buffer, moduleName.slice(-2, 0)).toAMD();
          }

          // This method allows hooking into the RequireJS toolchain.
          requirejs.tools.useLib(function(require) {
            // Convert to AMD if using CommonJS, by default the conversion
            // will ignore modules that already contain a define.
            require(["commonJs"], function(commonJs) {
              var wrapped = commonJs.convert(moduleName, buffer);
              var noDefine = wrapped.indexOf("define(") === -1;
              var noConfig = wrapped.indexOf("require.config") === -1;

              if (noDefine && noConfig) {
                wrapped = [
                  "define(function(require, exports, module) {",
                    wrapped,
                  "});"
                ].join("\n");
              }

              res.header("Content-type", "application/javascript");
              next(wrapped);
            });
          });
        },

        // Stylus.
        "\.styl$": function(buffer, req, res, next) {
          var stylus = require("grunt-lib-stylus").init(grunt);
          var contentType = "text/css";
          var opts = {
            paths: ["." + req.url.split("/").slice(0, -1).join("/") + "/"]
          };

          // Compile the source.
          stylus.compile(String(buffer), opts, function(contents) {
            res.header("Content-type", contentType);
            next(contents);
          });
        },
        
        // LESS.
        "\.less": function(buffer, req, res, next) {
          var less = require("grunt-lib-less").init(grunt);
          var contentType = "text/css";
          var opts = {
            paths: ["." + req.url.split("/").slice(0, -1).join("/") + "/"]
          };

          // Compile the source.
          less.compile(String(buffer), opts, function(contents) {
            res.header("Content-type", contentType);
            next(contents);
          });
        },

        // SCSS.
        "\.scss": function(buffer, req, res, next) {
          var less = require("grunt-lib-less").init(grunt);
          var contentType = "text/css";
          var opts = {
            paths: ["." + req.url.split("/").slice(0, -1).join("/") + "/"]
          };

          // Compile the source.
          less.compile(String(buffer), opts, function(contents) {
            res.header("Content-type", contentType);
            next(contents);
          });
        },
      },
    });

    // Ensure we have prefix relative.
    options.prefix = path.normalize(options.prefix);

    // Merge maps together.
    options.map = _.extend({}, options.map, fs.readdirSync(options.prefix).filter(function(file) {
      return fs.statSync(path.join(options.prefix, file)).isDirectory();
    }).reduce(function(memo, current) {
      memo[current] = current;
      return memo;
    }, {}));

    // Run forever and disable crashing.
    if (options.forever === true) {
      // Put this task into async mode, and never complete it.
      this.async();

      // Setting force to true, keeps Grunt from crashing while running the
      // server.
      grunt.option("force", true);
    }

    // Make this value more meaningful otherwise you can provide your own keys.
    if (_.isBoolean(options.ssl) && options.ssl) {
      // Load the SSL certificates, in case they are needed.
      options.ssl = {
        key: fs.readFileSync(__dirname + "/ssl/server.key", "utf8"),
        cert: fs.readFileSync(__dirname + "/ssl/server.crt", "utf8")
      };
    }

    // Run the server.
    run(options);

  });

  // Actually run the server...
  function run(options) {
    // If the server is already available use it.
    var site = options.server ? options.server() : express();
    var protocol = options.ssl ? "https" : "http";

    // Allow compression to be disabled.
    if (options.gzip !== false) {
      site.use(express.compress());
    }

    // Go through each compiler and provide an identical serving experience.
    _.each(options.middleware, function(callback, extension) {
      // Investigate if there is a better way of writing this.
      site.get(new RegExp(extension), function(req, res, next) {
        var url = req.url;

        // If there are query parameters, remove them.
        url = url.split("?")[0];

        // Determine the correct asset path.
        var path = options.map[url.slice(1)] || "." + url;

        // Read in the file contents.
        fs.readFile(path, function(err, buffer) {
          // File wasn't found.
          if (err) {
            return next();
          }

          callback(buffer, req, res, next);
        });
      });
    });

    // Map static folders to take precedence over redirection.
    Object.keys(options.map).sort().reverse().forEach(function(name) {
      var dirMatch = grunt.file.isDir(options.map[name]) ? "/*" : "";
      site.get(options.root + name + dirMatch, function(req, res, next) {
        // Find filename.
        var filename = req.url.slice((options.root + name).length);
        // If there are query parameters, remove them.
        filename = filename.split("?")[0];

        res.sendfile(path.join(options.prefix, options.map[name] + filename));
      });
    });

    // Very similar to map, except that the mapped path is another server.
    Object.keys(options.proxy).sort().reverse().forEach(function(name) {
      var target = options.proxy[name];
      var protocol = target.https ? "https" : "http";

      var proxyOptions = {
        // This can be a string or an object.
        target: target,

        // Do not change the origin, this can affect how servers respond.
        changeOrigin: false,

        // Remove the forwarded headers, make it feel seamless.
        enable : {
          xforward: false
        }
      };

      // Ensure the https proxy settings are configured here as well.
      if (_.isBoolean(target.https) && target.https) {
        proxyOptions.https = {
          key: fs.readFileSync(__dirname + "/ssl/server.key", "utf8"),
          cert: fs.readFileSync(__dirname + "/ssl/server.crt", "utf8")
        };
      }

      // Initialize the actual proxy object.
      var proxy = new httpProxy.HttpProxy(proxyOptions);

      // Same thing for these, if you have https boolean set to true, default
      // to internal keys/certs.
      if (_.isBoolean(target.https) && target.https) {
        // Load the SSL certificates, in case they are needed.
        target.https = {
          key: fs.readFileSync(__dirname + "/ssl/server.key", "utf8"),
          cert: fs.readFileSync(__dirname + "/ssl/server.crt", "utf8")
        };
      }

      // Listen on all HTTP verbs for seamless proxying.
      site.all(options.root + name, function(req, res, next) {
        var referer = protocol + "://" + options.host;
        var origin = protocol + "://" + proxyOptions.target.host;

        if (options.port !== "80") {
          referer += ":" + options.port;
        }

        // Transparently set the referer, origin, and host headers.
        _.extend(req.headers, {
          referer: origin,
          origin: origin,
          host: proxyOptions.target.host,
          accept: req.headers.accept || "*/*"
        }, target.headers);

        if (String(req.headers.accept) === "undefined") {
          req.headers.accept = "*/*";
        }

        // Make the proxy request.
        proxy.proxyRequest(req, res);
      });
    });

    // Compression middleware.
    site.all("*", function(content, req, res, next) {
      if (content) {
        return res.send(content);
      }

      next();
    });

    // Attempt to laod from the prefixed directory first.
    //site.use(express.static(path.join(CWD, options.prefix)));
    site.all("*", function(req, res, next) {
      // If there are query parameters, remove them.
      var filename = path.join(options.prefix, req.url.split("?")[0]);

      // Detect if the file requested exists in the current prefixed path.
      fs.exists(filename, function(exists) {
        // If the file doesn't exist or if it's a folder, let another handler
        // take care of serving.
        if (!exists || filename.slice(-1) === path.sep) {
          next();
        } else {
          res.sendfile(filename);
        }
      });
    });

    // Ensure all routes go home, client side app..
    if (options.pushState) {
      site.all("*", function(req, res) {
        fs.createReadStream(path.join(options.prefix, options.index)).pipe(res);
      });
    }

    // Echo out a message alerting the user that the server is running.
    console.log("Listening on", protocol + "://" + options.host + ":" +
      options.port);

    // Start listening.
    if (!options.ssl) {
      return site.listen(options.port, options.host);
    }

    // Create the SSL server instead...
    https.createServer(options.ssl, site).listen(options.port, options.host);
  }
};
