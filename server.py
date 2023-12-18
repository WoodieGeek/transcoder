from flask import Flask
import json
app = Flask(__name__)


json_file = open('map.json', 'r')
videos_json = json.load(json_file)


@app.route("/video")
def get():
    return videos_json



if __name__ == "__main__":
    app.run()
