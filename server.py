from flask import Flask, request, make_response
import json
import os
app = Flask(__name__)

name = "map.json"

@app.route("/videos")
def get():
    json_file = open(name, 'r')
    videos_json = json.load(json_file)
    return videos_json


@app.route('/upload', methods=['POST'])
def upload_file():
    file = request.files['file']
    description = request.form['description']
    filename = file.filename
    title = request.form['title']
    path_to_save = name
    file.save(os.path.join('storage', filename))
    json_file = open(name, 'r')
    my_json = json.load(json_file)
    my_json['videos'].append({'name': title,
                              'description': description,
                              'manifest_url': 'http://127.0.0.1:5000/play/'+filename})
    with open(name, 'w') as fp:
        json.dump(my_json, fp)

    response = make_response("uploaded")
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

if __name__ == "__main__":
    app.run()