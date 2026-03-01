import falcon
import gettext
import os
from jinja2 import Environment, FileSystemLoader

# Setup Jinja2
path = os.path.join(os.path.dirname(__file__), 'templates')
jinja_env = Environment(loader=FileSystemLoader(path))

class HomeResource:
    def on_get(self, req, resp):
        # 1. Determine language (from header 'Accept-Language' or query param)
        lang = req.get_param('lang') or 'en'

        # 2. Setup gettext for this request
        # 'base' is the filename of your .po/.mo files
        # localedir is the path to your 'locales' folder
        translation = gettext.translation(
            'base', 
            localedir=os.path.join(os.path.dirname(__file__), 'locales'), 
            languages=[lang],
            fallback=True # Fallback to English if language not found
        )
        
        # Install the _() function globally or pass it to the template
        # Passing it to the template is cleaner for MVC
        _ = translation.gettext

        # 3. Render the View
        template = jinja_env.get_template('uninitialized-index.html')
        resp.status = falcon.HTTP_200
        resp.content_type = falcon.MEDIA_HTML
        
        # We pass the _ function to Jinja2 so it can translate strings in HTML
        resp.text = template.render(_=_, current_lang=lang)

class LanguageMiddleware:
    def process_resource(self, req, resp, resource, params):
        # Look for ?lang= or fallback to 'en'
        lang = req.get_param('lang') or 'en'
        
        # Create the translation object
        translation = gettext.translation(
            'base', 'locales', languages=[lang], fallback=True
        )
        # Attach the translation function to the request object
        req.context.translate = translation.gettext

# Initialize app with middleware
app = falcon.App(middleware=[LanguageMiddleware()])

# Define the local path (e.g., /usr/bin/webserver/static)
# __file__ helps locate the folder relative to your python script
working_dir = os.path.dirname(os.path.abspath(__file__))
static_path = os.path.join(working_dir, 'static')

# Map URL '/static' to the disk folder
app.add_static_route('/static', static_path)

app.add_route('/', HomeResource())

if __name__ == '__main__':
    from waitress import serve
    print("Server starting on http://0.0.0.0:8080")
    serve(app, host='0.0.0.0', port=8080)
