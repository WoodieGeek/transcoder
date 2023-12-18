var $table = $('.table');

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
    for(var i = 0; i < data.length; ++i) {
        $table.append('<tr> <td class = "name">' + data[i].name + '</td> <td class = "description">' + data[i].description + '</td> <td> <a href = ' + data[i].manifest_url + ' class = "table-link"> Link </a> </td> </tr>');
    }
}

buildTable();
