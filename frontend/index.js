var $table = $('.table');
var data = [];

async function CallHTTP() {
    let url = "http://localhost:5000/videos";

    let response = await fetch(url);

    // alert(response.status);
    if (response.ok) {
        let text = response.text()
        // let text = "{name: 'Cats', desc: 'Some info...', manifestUrl: '?'}, {name: 'Dogs', desc: 'Some info...', manifestUrl: '?'}, {name: 'Nature', desc: 'Some info...', manifestUrl: '?'}, {name: 'Fruits', desc: 'Some info...', manifestUrl: '?'}";
        data = JSON.parse(text);
    } else {
        alert("Ошибка HTTP: code " + response.status);
    }
}

CallHTTP();
// TEST
// var grains = [
//     {
//         name: 'Cats',
//         desc: 'Some info...',
//         manifestUrl: '?'
//     },
//     {
//         name: 'Dogs',
//         desc: 'Some info...',
//         manifestUrl: '?'
//     },
//     {
//         name: 'Nature',
//         desc: 'Some info...',
//         manifestUrl: '?'
//     }, 
//     {
//         name: 'Fruits',
//         desc: 'Some info...',
//         manifestUrl: '?'
//     }
// ];
// END TEST

function buildTable() {
    $table.append('<tr> <td class = "main"> Name </td> <td class = "main"> Description </td> <td class = "main"> Preview </td> </tr>');

    for(var i = 0; i < data.length; ++i) {
       $table.append('<tr> <td class = "name">' + data[i].name + '</td> <td class = "description">' + data[i].desc + '</td> <td> </td> </tr>');
    }
}

buildTable();
