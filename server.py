from flask import Flask, request, make_response
import flask
import json
import os
from flask import send_file
from moviepy.editor import VideoFileClip
import datetime
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
    if not os.path.exists('storage/processed'):
        os.makedirs('storage/processed')
    file.save(os.path.join('storage', filename))
    json_file = open(name, 'r')
    my_json = json.load(json_file)
    my_json['videos'].append({'name': title,
                              'description': description,
                              'manifest_url': 'http://127.0.0.1:5000/play/storage/processed/' + filename.split('.')[0] + ".m3u8"})
    with open(name, 'w') as fp:
        json.dump(my_json, fp)
    full_video = "storage/"+filename
    full_duration = VideoFileClip(full_video).duration
    parts = 1
    part_duration = full_duration/parts
    start_pos = 0
    index = 1
    query_full_video = []
    query_part_name = []
    query_start_pos = []
    query_end_pos = []
    query_time = []
    while (True) :
        end_pos = start_pos+part_duration
        if end_pos > full_duration:
            end_pos = full_duration
        # part_name = "part"+str(index)+".ts"
        query_full_video.append(full_video)
        query_part_name.append("storage/processed/")
        query_time.append(end_pos-start_pos)
        st = str(start_pos)
        en = str(end_pos)
        query_start_pos.append(st)
        query_end_pos.append(en)
        start_pos = end_pos
        index += 1
        if start_pos >= full_duration:
            break
    manifest360 = []
    for ind in range(parts):
        manifest360.append((query_part_name[ind]+"360.ts", query_time[ind]))
        process = multiprocessing.Process(target=run_cpp, args=(query_full_video[ind], query_part_name[ind]+"360.ts", query_start_pos[ind], query_end_pos[ind], "360p"))
        process.start()
    manifest_generate(("360").split('.')[0], manifest360)
    manifest480 = []
    for ind in range(parts):
        manifest480.append((query_part_name[ind]+"480.ts", query_time[ind]))
        process = multiprocessing.Process(target=run_cpp, args=(query_full_video[ind], query_part_name[ind]+"480.ts", query_start_pos[ind], query_end_pos[ind], "480p"))
        process.start()
    manifest_generate(("480").split('.')[0], manifest480)
    manifest720 = []
    for ind in range(parts):
        manifest720.append((query_part_name[ind]+"720.ts", query_time[ind]))
        process = multiprocessing.Process(target=run_cpp, args=(query_full_video[ind], query_part_name[ind]+"720.ts", query_start_pos[ind], query_end_pos[ind], "720p"))
        process.start()
    manifest_generate(("720").split('.')[0], manifest720)
    manifest1080 = []
    for ind in range(parts):
        manifest1080.append((query_part_name[ind]+"1080.ts", query_time[ind]))
        process = multiprocessing.Process(target=run_cpp, args=(query_full_video[ind], query_part_name[ind]+"1080.ts", query_start_pos[ind], query_end_pos[ind], "1080p"))
        process.start()
    manifest_generate(("1080").split('.')[0], manifest1080)
    response = make_response("uploaded")
    response.headers['Access-Control-Allow-Origin'] = '*'
    return response



@app.route('/play/<path:pth>')

def play(pth):
    print(pth)
    if os.path.isfile(pth) == True:
        response = send_file(pth)
        response.headers['Access-Control-Allow-Origin'] = '*'
        return response
    else:
        print("No such file")
        response = flask.make_response("404 not found", 404)
        response.headers['Access-Control-Allow-Origin'] = '*'
        return response

        

if __name__ == "__main__":
    app.run()
