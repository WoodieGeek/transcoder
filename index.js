var $table = $('.grains');
// var grains = [];

async function CallHTTP() { // DOESN'T WORK (at now)
    let url = "some adress";

    let response = await fetch(url);

    if (response.ok) {
        let text = response.text();
        let data = JSON.parse(text);

        // grains = data;
    } else {
        alert("Ошибка HTTP: code " + response.status);
    }
}

CallHTTP();
// TEST
var grains = [
    {
        name: 'Cats',
        desc: 'Some info...'
        },
    {
        name: 'Dogs',
        desc: 'Some info...'
    },
    {
        name: 'Nature',
        desc: 'Some info...'
    }, 
    {
        name: 'Fruits',
        desc: 'Some info...'
    }
];
// END TEST

function buildTable() {
    $table.append('<tr> <td class = "main"> Name </td> <td class = "main"> Description </td> <td class = "main"> Preview </td> </tr>');

    for(var i = 0; i < grains.length; ++i) {
       $table.append('<tr> <td class = "name">' + grains[i].name + '</td> <td class = "description">' + grains[i].desc + '</td> <td> </td> </tr>');
    }
}

buildTable();