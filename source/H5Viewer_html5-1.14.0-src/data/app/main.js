// Break out the application running from the configuration definition to
// assist with testing.
require(["config"], function() {
    // Kick off the application.
    var storedLocale  = window.navigator.languages ? window.navigator.languages[0] : null;
    storedLocale = (storedLocale || window.navigator.language || window.navigator.browserLanguage).toLowerCase(); //For Browser support
//##LANG_TAGS
    storedLocale = "root";
    if (storedLocale) {
        requirejs.config({
            config: {
                i18n: {
                    locale: storedLocale
                },
            }
        });
    }
    require(["app", "router"], function(app, Router) {
        // Define your master router on the application namespace and trigger all
        // navigation from this instance.
        app.router = new Router();
        //app.auto_router = new AutoRouter();

        app.initialize();

        // Trigger the initial route and enable HTML5 History API support, set the
        // root folder to '/' by default.  Change in app.js.
        Backbone.history.start({
            pushState: false,
            root: app.root
        });

        // Map from CRUD to HTTP for our default `Backbone.sync` implementation.
        var methodMap = {
            'create': 'POST',
            'update': 'PUT',
            'patch': 'PATCH',
            'delete': 'DELETE',
            'read': 'GET'
        };

        Backbone.sync = function(method, model, options) {
            var type = methodMap[method];

            // Default options, unless specified.
            _.defaults(options || (options = {}), {
                emulateHTTP: Backbone.emulateHTTP,
                emulateJSON: Backbone.emulateJSON
            });

            // Default JSON-request options.
            var params = {
                type: type,
                dataType: 'json'
            };

            // Ensure that we have a URL.
            if (!options.url) {
                params.url = _.result(model, 'url') || urlError();
            }

            if (!options.error) {
                params.error = app.HTTPErrorHandler;
            } else if(options.error instanceof Array){
                options.error.push(app.HTTPErrorHandler);
            } else {
                options.error = [options.error, app.HTTPErrorHandler];
            }

            // Ensure that we have the appropriate request data.
            if (options.data == null && model && (method === 'create' || method === 'update' || method === 'patch')) {
                params.contentType = 'application/json';
                params.data = JSON.stringify(options.attrs || model.toJSON(options));
            }

            // For older servers, emulate JSON by encoding the request into an HTML-form.
            if (options.emulateJSON) {
                params.contentType = 'application/x-www-form-urlencoded';
                params.data = params.data ? {
                    model: params.data
                } : {};
            }

            // For older servers, emulate HTTP by mimicking the HTTP method with `_method`
            // And an `X-HTTP-Method-Override` header.
            if (options.emulateHTTP && (type === 'PUT' || type === 'DELETE' || type === 'PATCH')) {
                params.type = 'POST';
                if (options.emulateJSON) params.data._method = type;
                var beforeSend = options.beforeSend;
                options.beforeSend = function(xhr) {
                    xhr.setRequestHeader('X-HTTP-Method-Override', type);
                    if (beforeSend) return beforeSend.apply(this, arguments);
                };
            }

            // Don't process data on a non-GET request.
            if (params.type !== 'GET' && !options.emulateJSON) {
                params.processData = false;
            }

            // If we're sending a `PATCH` request, and we're in an old Internet Explorer
            // that still has ActiveX enabled by default, override jQuery to use that
            // for XHR instead. Remove this line when jQuery supports `PATCH` on IE8.
            if (params.type === 'PATCH' && noXhrPatch) {
                params.xhr = function() {
                    return new ActiveXObject("Microsoft.XMLHTTP");
                };
            }

            // Make the request, allowing the user to override any Ajax options.
            var xhr = options.xhr = Backbone.ajax(_.extend(params, options));
            model.trigger('request', model, xhr, options);
            return xhr;
        };

    });
});
