import falcon
import gettext
import os
from jinja2 import Environment, FileSystemLoader
import ipc

# Setup Jinja2
path = os.path.join (os.path.dirname (__file__), 'templates')
jinja_env = Environment (loader=FileSystemLoader (path))

class FactoryHomeResource:
    def on_get (self, req, resp, **kwargs):
        # Determine language (from header 'Accept-Language' or query param)
        lang = kwargs['lang'] or 'en'

        # Install the _() function globally or pass it to the template
        # Passing it to the template is cleaner for MVC
        _ = req.context.translate

        # Render the View
        template = jinja_env.get_template ('uninitialized-index.html')
        resp.status = falcon.HTTP_200
        resp.content_type = falcon.MEDIA_HTML
        
        # We pass the _ function to Jinja2 so it can translate strings in HTML
        resp.text = template.render (_=_, current_lang=lang)

class RootResource:
    def on_get (self, req, resp):
        accept_lang = req.get_header ('Accept-Language')

        lang = None

        if accept_lang:
            # Extract 'en' from 'en-US,en;q=0.9'
            lang = accept_lang.split (',')[0].split (';')[0].split ('-')[0].strip ()

        # Final Fallback
        lang = lang or 'en'

        raise falcon.HTTPMovedPermanently ("/" + lang + "/")

class LanguageMiddleware:
    def process_resource (self, req, resp, resource, params):
        # 1. Look specifically for the 'lang' variable from the URL path
        # This comes from your route: app.add_route('/{lang}/home', resource)
        lang = params.get('lang')

        # 2. Fallback: If URL doesn't have {lang}, check the Browser Header
        if not lang:
            accept_lang = req.get_header ('Accept-Language')
            if accept_lang:
                # Extract 'en' from 'en-US,en;q=0.9'
                lang = accept_lang.split (',')[0].split (';')[0].split ('-')[0].strip ()
        
        # 3. Final Fallback
        lang = lang or 'en'

        # --- Translation Logic ---
        try:
            translation = gettext.translation ('base', 'locales', 
                                               languages=[lang], 
                                               fallback=True)
        except Exception:
            # If the folder for 'it' or 'en' is missing, use default
            translation = gettext.translation('base', 'locales', languages=['en'], fallback=True)
        
        # Attach to context so your Resource can use it
        req.context.translate = translation.gettext

# Initialize app with middleware
app = falcon.App (middleware = [LanguageMiddleware ()])

# Define the local path (e.g., /usr/bin/webserver/static)
# __file__ helps locate the folder relative to your python script
working_dir = os.path.dirname (os.path.abspath (__file__))
static_path = os.path.join (working_dir, 'static')

if ipc.is_factory_default_state ():
    app.add_route ('/{lang}/', FactoryHomeResource ())
else:
    pass

app.add_static_route ('/static', static_path)
app.add_route ('/', RootResource ())

if __name__ == '__main__':
    from waitress import serve
    print ("Server starting on http://0.0.0.0:8080")
    serve (app, host='0.0.0.0', port=8080)
