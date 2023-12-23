from flask import Flask, request, make_response
import flask
import json
import os
from flask import send_from_directory
#from moviepy.editor import VideoFileClip
import datetime
import subprocess
import concurrent.futures

app = Flask(__name__)

name = "map.json"


@app.route("/videos")
def get():
    json_file = open(name, 'r')
    videos_json = json.load(json_file)

    response = flask.make_response(videos_json)
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response

def run_cpp(input_file, output_file, start_pos, end_pos):
    subprocess.run("main", input_file, output_file, start_pos, end_pos)


def manifest_generate(filename, segments_mas):
    print(segments_mas)
    with open("storage/processed/" + filename + ".m3u8", 'w') as file_ts:
        file_ts.write("#EXTM3U\n")
        file_ts.write("#EXT-X-VERSION:3\n")
        file_ts.write("#EXT-X-TARGETDURATION:" + str(segments_mas[0][1]) + '\n')
        file_ts.write("#EXT-X-MEDIA-SEQUENCE:0\n")
        file_ts.write("#EXT-X-PLAYLIST-TYPE:VOD\n")
        for i in range(len(segments_mas)):
            file_ts.write("#EXTINF:" + str(segments_mas[i][1]) + ',\n')
            file_ts.write(segments_mas[i][0] + '\n')


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
                              'manifest_url': 'http://127.0.0.1:5000/play/' + filename[:4] + ".m3u8"})
    with open(name, 'w') as fp:
        json.dump(my_json, fp)

    manifest_generate(filename[:4], [('out1.ts', 9)])
    response = make_response("uploaded")
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response



@app.route('/play/<path:path>')

def play(path):
    print(path)
    if os.path.isfile('storage/processed/' + path) == True:
        response = send_from_directory('storage/processed/', path)
        response.headers['Access-Control-Allow-Origin'] = '*'
        return response
    else:
        print("No such file")
        response = flask.make_response("404 not found", 404)
        response.headers['Access-Control-Allow-Origin'] = '*'
        return response

        

if __name__ == "__main__":
    app.run()
