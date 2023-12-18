const form = document.querySelector('form');
form.addEventListener('submit', function(event) {
  event.preventDefault();
  formData = new FormData(form);
  fetch(http://127.0.0.1:5000/upload', {
    method: 'POST',
    body: formData,
  });
  form.reset();
  
});
