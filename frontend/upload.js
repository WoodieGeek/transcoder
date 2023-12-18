const form = document.querySelector('form');
form.addEventListener('submit', function(event) {
  event.preventDefault();
  formData = new FormData(form);
  fetch('http://localhost:500/upload', {
    method: 'POST',
    body: formData,
  });
  form.reset();
  
});
