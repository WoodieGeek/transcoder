document.querySelectorAll('.table tr').forEach(function(row) {
  row.addEventListener('click', function() {
    var manifest = this.children[2].textContent;
    console.log('!!! ' + manifest);

    const videoPlayer = document.createElement('video');
    videoPlayer.src = manifest;
    document.body.appendChild(videoPlayer);
    videoPlayer.play();
  });
});
