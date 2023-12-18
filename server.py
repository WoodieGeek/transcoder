from flask import Flask
import json
import os
app = Flask(__name__)


json_file = open('map.json', 'r')
videos_json = json.load(json_file)


@app.route("/videos")
def get():
    return videos_json


@app.route('/upload')
def upload_file(request):
    file = request.files['file']
    description = request.form['description']
    filename = file.filename
    description_name = description.filename
    path_to_save = "map.json"
    file.save(os.path.join(path_to_save, {filename : description_name}))


if __name__ == "__main__":
    app.run()

