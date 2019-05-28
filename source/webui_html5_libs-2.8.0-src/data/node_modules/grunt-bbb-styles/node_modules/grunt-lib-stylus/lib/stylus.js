/*
 * grunt-lib-stylus
 * http://gruntjs.com/
 *
 * Copyright (c) 2013 Eric Woroshow, contributors
 * Licensed under the MIT license.
 */

'use strict';

exports.init = function(grunt) {

  // Nodejs libs.
  var path = require('path');

  // External libs.
  var stylus = require('stylus');
  var nib = require('nib');

  // A wrapper around `compile` that accepts a file and reads its contents.
  exports.compileFile = function(srcFile, options, callback) {
    options = grunt.util._.extend({filename: srcFile}, options);

    exports.compile(grunt.file.read(srcFile), options, callback);
  };

  // Accepts a String of source Stylus-flavored CSS and an options object.  The
  // last argument is a callback function, since Stylus has an asynchronous
  // API.
  exports.compile = function(srcCode, options, callback) {
    // Never compress output in debug mode
    if (grunt.option('debug')) {
      options.compress = false;
    }

    var s = stylus(srcCode);

    // Load nib always.
    options.paths = options.paths || [];
    options.paths.push(nib.path);

    grunt.util._.each(options, function(value, key) {
      if (key === 'urlfunc') {
        // Custom name of function for embedding images as Data URI
        s.define(value, stylus.url());
      } else if (key === 'use') {
        value.forEach(function(func) {
          if (typeof func === 'function') {
            s.use(func());
          }
        });
      } else {
        s.set(key, value);
      }
    });

    s.render(function(err, css) {
      if (err) {
        grunt.log.error(err);
        grunt.fail.warn('Stylus failed to compile.');

        callback(css, true);
      } else {
        callback(css, null);
      }
    });
  };

  return exports;
};
