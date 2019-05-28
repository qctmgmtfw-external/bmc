# Grunt configuration updated to latest Grunt.  That means your minimum
# version necessary to run these tasks is Grunt 0.4.
#
# Please install this locally and install `grunt-cli` globally to run.
module.exports = ->

  # Initialize the configuration.
  @initConfig

    jshint:
      all: ["tasks/*.js", "<%= nodeunit.tests %>"]

      options:
        jshintrc: ".jshintrc"

    # Configuration to be run (and then tested).
    server:
      default:
        options:
          forever: false

    # Unit tests.
    nodeunit:
      tests: ["test/*.js"]
  
  # Actually load this plugin's task(s).
  @loadTasks "tasks"
  
  # These plugins provide necessary tasks.
  @loadNpmTasks "grunt-contrib-jshint"
  @loadNpmTasks "grunt-contrib-nodeunit"
  
  # Whenever the "test" task is run, first clean the "tmp" dir, then run this
  # plugin's task(s), then test the result.
  @registerTask "test", ["server", "nodeunit"]
  
  # By default, lint and run all tests.
  @registerTask "default", ["jshint", "test"]
