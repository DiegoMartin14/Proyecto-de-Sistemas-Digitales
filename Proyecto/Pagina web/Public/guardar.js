// Función para validar el formulario
function validarFormulario() {
  // Obtener los valores de los campos
  const nombre = document.getElementById("tipoCerveza").value;
  const tempMax = document.getElementById("maxTemp").value;
  const tempMin = document.getElementById("minTemp").value;

  // Validar que ninguno de los campos esté vacío
  if (nombre === "" || tempMax === "" || tempMin === "") {
      alert("Por favor, complete todos los campos antes de guardar la configuración.");
      return false; // No guarda ni envía el formulario
  }

  // Si todos los campos están llenos, llamar a la función guardar
  guardar();
}

// Función para guardar datos en Firestore
async function guardar() {
    const tanqueSeleccionado = document.getElementById("tank-select").value;
    const nombre = document.getElementById("tipoCerveza").value;
    const tempMax = document.getElementById("maxTemp").value;
    const tempMin = document.getElementById("minTemp").value;
  
    try {
      await db
        .collection("Configuracion")
        .doc(`Tanque${tanqueSeleccionado}`)
        .set({
          Nombre: nombre,
          TempMax: tempMax,
          TempMin: tempMin,
        });
      console.log(
        `Documento para Tanque ${tanqueSeleccionado} escrito exitosamente!`
      );
      document.getElementById("tipoCerveza").value = "";
      document.getElementById("minTemp").value = "";
      document.getElementById("maxTemp").value = "";
    } catch (error) {
      console.error("Error al escribir el documento: ", error);
    }
  }
  
  window.guardar = guardar;