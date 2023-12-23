from flask import Flask, request, make_response
import flask
import json
import os
from flask import send_from_directory
from moviepy.editor import VideoFileClip
import subprocess
import multiprocessing

app = Flask(__name__)

name = "map.json"


@app.route("/videos")
def get():
    json_file = open(name, 'r')
    videos_json = json.load(json_file)

    response = flask.make_response(videos_json)
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

def run_cpp(input_file, output_file, start_pos, end_pos, kach):
    subprocess.run(["./main", input_file, output_file, start_pos, end_pos, kach])


@app.route('/upload', methods=['POST'])
def upload_file():
    file = request.files['file']
    description = request.form['description']
    filename = file.filename
    title = request.form['title']
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
    full_video = "storage/"+filename
    full_duration = VideoFileClip(full_video).duration
    parts = 2
    part_duration = full_duration/parts
    start_pos = 0
    index = 1
    query_full_video = []
    query_part_name = []
    query_start_pos = []
    query_end_pos = []
    while (True) :
        end_pos = start_pos+part_duration
        if end_pos > full_duration:
            end_pos = full_duration
        part_name = "part"+str(index)+".ts"
        query_full_video.append(full_video)
        query_part_name.append(part_name)
        st = str(start_pos)
        en = str(end_pos)
        query_start_pos.append(st)
        query_end_pos.append(en)
        start_pos = end_pos
        index += 1
        if start_pos >= full_duration:
            break
    event = []
    for ind in range(parts):
        process = multiprocessing.Process(target=run_cpp, args=(query_full_video[ind], query_part_name[ind], query_start_pos[ind], query_end_pos[ind], "720p"))
        process.start()
        # run_cpp(query_full_video[ind], query_part_name[ind], query_start_pos[ind], query_end_pos[ind], "720p")

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
