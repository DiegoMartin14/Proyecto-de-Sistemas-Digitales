// Llama a estas funciones después de que la página haya cargado
document.addEventListener("DOMContentLoaded", () => {
  actualizarTarjetas();
});

// Función para actualizar el nombre de la cerveza en las tarjetas
function actualizarTarjetas() {
  for (let i = 1; i <= 6; i++) {
    // Escuchar los cambios en la colección 'Configuración'
    firebase
      .firestore()
      .collection("Configuracion")
      .doc(`Tanque${i}`)
      .onSnapshot(
        (configDoc) => {
          // Obtiene el Nombre del documento, usa '-' si no existe
          const nombreCerveza = configDoc.exists
            ? configDoc.data().Nombre
            : "-";
          // Actualiza el contenido del elemento HTML correspondiente
          document.getElementById(`nombre-${i}`).textContent = nombreCerveza;
        },
        (error) => {
          // Maneja errores al escuchar el documento
          console.error(
            `Error al escuchar el nombre de cerveza del Tanque ${i}:`,
            error
          );
        }
      );

  // Escucha los cambios en la colección 'Produccion', ordenados por ID en orden descendente
  window.db
    .collection("Produccion")
    .orderBy(firebase.firestore.FieldPath.documentId(), "desc") // Ordenar por ID (epoch time) descendente
    .onSnapshot((querySnapshot) => {
      const ultimaTemperaturaPorTanque = {};

      querySnapshot.forEach((doc) => {
        const docData = doc.data();
        const tanque = docData.Tanque; // Asegúrate de que este campo exista
        const temperatura = parseFloat(docData.temperatura); // Asegúrate de que este campo exista

        // Guardar la temperatura más reciente para cada tanque solo si no existe ya
        if (!ultimaTemperaturaPorTanque[tanque]) {
          ultimaTemperaturaPorTanque[tanque] = temperatura;
        }
      });

      // Actualizar cada tarjeta con la última temperatura obtenida para cada tanque
      for (let i = 1; i <= 6; i++) {
        const temperatura = ultimaTemperaturaPorTanque[i] !== undefined ? `${ultimaTemperaturaPorTanque[i]} °C` : "- °C";
        const temperaturaElemento = document.getElementById(`temp-${i}`);
        if (temperaturaElemento) {
          temperaturaElemento.textContent = temperatura;
        }
      }
    }, (error) => {
      console.error("Error al escuchar las temperaturas en tiempo real:", error);
    });
  }

  // actualizo la fecha y hora de las ultimas mediciones
  firebase
    .firestore()
    .collection("Produccion")
    .onSnapshot(
      (snapshot) => {
        // Solo actualiza si hay cambios en los documentos
        if (!snapshot.empty) {
          // Obtener el primer documento (último documento por ID)
          const doc = snapshot.docs[snapshot.docs.length - 1];
          const epochTime = parseInt(doc.id); // Convertir el ID a número

          // Comprobar si el epochTime es válido
          if (!isNaN(epochTime)) {
            const fechaHora = convertirEpochATimestamp(epochTime); // Convertir a fecha y hora

            // Mostrar la fecha y hora en la página web
            document.getElementById("ultimaMedicion").textContent = fechaHora; // Actualizar el contenido del elemento
          } else {
            console.error(`ID no válido: ${doc.id}`);
            document.getElementById("ultimaMedicion").textContent = "-";
          }
        }
      },
      (error) => {
        console.error("Error al escuchar la colección 'Produccion':", error);
      }
    );
}

// Función para convertir epochTime a fecha y hora
function convertirEpochATimestamp(epochTime) {
  const fechaHoraUTC = new Date(epochTime * 1000); // Convertir epoch time a Date en milisegundos

  const opcionesFecha = { year: "numeric", month: "2-digit", day: "2-digit" };
  const opcionesHora = {
    hour: "2-digit",
    minute: "2-digit",
    hour12: false,
    timeZone: "UTC", // Asegurar que use UTC
  };

  // Formatear la fecha
  const fechaFormateada = fechaHoraUTC.toLocaleDateString(
    "es-ES",
    opcionesFecha
  );
  // Formatear la hora
  const horaFormateada = fechaHoraUTC.toLocaleTimeString("es-ES", opcionesHora);

  return `${fechaFormateada} ${horaFormateada} hs`; // Devolver fecha y hora formateadas
}
