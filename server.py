from flask import Flask, request, make_response
import flask
import json
import os
from flask import send_from_directory

app = Flask(__name__)

name = "map.json"


@app.route("/videos")
def get():
    json_file = open(name, 'r')
    videos_json = json.load(json_file)

    response = flask.make_response(videos_json)
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response


@app.route('/upload', methods=['POST'])
def upload_file():
    file = request.files['file']
    description = request.form['description']
    filename = file.filename
    title = request.form['title']
    path_to_save = name
    if not os.path.exists('storage'):
        os.makedirs('storage')
    file.save(os.path.join('storage', filename))
    json_file = open(name, 'r')
    my_json = json.load(json_file)
    my_json['videos'].append({'name': title,
                              'description': description,
                              'manifest_url': 'http://127.0.0.1:5000/play/' + filename})
    with open(name, 'w') as fp:
        json.dump(my_json, fp)

    response = make_response("uploaded")
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response


@app.route('/play/<filename>')
def play(filename):
    if os.path.isfile('storage/' + filename) == True:
        return send_from_directory('storage', filename)
    else:
        print("No such file")
        return flask.make_response("404 not found", 404)


if __name__ == "__main__":
    app.run()