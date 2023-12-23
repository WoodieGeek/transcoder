var $table = $('.table');

async function playVideo(url) {
    const videoPlayerContainer = document.getElementById("video-player");
    console.log(videoPlayerContainer);
    videoPlayerContainer.innerHTML = ""; // Clear previous video
    
    const video = document.createElement("video");
    video.controls = true;
    videoPlayerContainer.appendChild(video);
    
    if (Hls.isSupported()) {
        const hls = new Hls();
        hls.loadSource(url);
        hls.attachMedia(video);
        hls.on(Hls.Events.MANIFEST_PARSED, function () {
          video.play();
        });
    } else if (video.canPlayType("application/vnd.apple.mpegurl")) {
        video.src = url;
        video.addEventListener("loadedmetadata", function () {
          video.play();
        });
    } else {
        console.error("HLS playback is not supported in this browser.");
    }
}

async function buildTable() {
    let url = "http://localhost:5000/videos";
    let response = await fetch(url);

    var data = [];
    if (response.ok) {
        let text = (await response.text()).toString()

        data = JSON.parse(text);
    } else {
        alert("Ошибка HTTP: code " + response.status);
    }

    $table.append('<tr> <td class = "main"> Name </td> <td class = "main"> Description </td> <td class = "main"> Video </td> </tr>');

    data = data["videos"];
    for (var i = 0; i < data.length; ++i) {
        $table.append('<tr> <td class = "name">' + data[i].name + '</td> <td class = "description">' + data[i].description + '</td> <td> <a href = ' + data[i].manifest_url + ' class = "table-link"> Link </a> </td> </tr>');

	console.log(data[i].manifest_url);
        $table[$table.length - 1].addEventListener('click', () => {
            playVideo(data[$table.length - 1].manifest_url);
        });
    }
}

buildTable();

