from flask import Flask
import json
import os
app = Flask(__name__)

name = "map.json"

@app.route("/videos")
def get():
    json_file = open(name, 'r')
    videos_json = json.load(json_file)
    return videos_json


@app.route('/upload')
def upload_file(request):
    file = request.files['file']
    description = request.form['description']
    filename = file.filename
    description_name = description.filename
    path_to_save = name
    file.save(os.path.join(path_to_save, {filename : description_name}))
    

@app.route('/play')
def play(filename):
    if path.isfile(filename) == True:
        return send_from_directory('storage/'+filename+'.mp4', filename)


if __name__ == "__main__":
    app.run()
