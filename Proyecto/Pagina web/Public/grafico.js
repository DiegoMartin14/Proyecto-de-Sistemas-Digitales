// Inicializar el gráfico con el lapso de tiempo seleccionado
async function initializeChart() {
  const timeRange = document.getElementById("timeRange").value;
  await updateChart(parseInt(timeRange));

  document
    .getElementById("timeRange")
    .addEventListener("change", async (event) => {
      const selectedDays = parseInt(event.target.value);
      await updateChart(selectedDays);
    });
}

// Función para actualizar el gráfico con nuevos datos
async function updateChart(days) {
  try {
    const dataByTank = await fetchData(days);

    temperatureChart.data.datasets.forEach((dataset, index) => {
      dataset.data = dataByTank[index + 1] || [];
    });

    temperatureChart.update(); // Actualizar el gráfico con los nuevos datos
    
    // Resetear el zoom al valor predeterminado
    if (temperatureChart.resetZoom) {
      temperatureChart.resetZoom();
    }
  } catch (error) {
    console.error("Error al actualizar el gráfico:", error);
  }
}


async function fetchData(days) {
  const now = new Date();
  let startDate, endDate;

  // Calcular las fechas según el período seleccionado
  if (days === 1) {
    // Hoy (en UTC)
    startDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate() - 1, 3, 0, 0, 0)); // Medianoche UTC
    endDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), 2, 59, 59, 999)); // Final del día UTC
  } else if (days === 2) {
    // Ayer (en UTC)
    startDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate() - 2, 3, 0, 0, 0)); // Medianoche UTC de ayer
    endDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate() - 1, 2, 59, 59, 999)); // Final del día UTC de ayer
  } else if (days === 7) {
    // Última semana (en UTC)
    startDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate() - 8, 3, 0, 0, 0)); // Medianoche UTC hace 7 días
    endDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), 2, 59, 59, 999)); // Final del día UTC
  } else if (days === 14) {
    // Últimas 2 semanas (en UTC)
    startDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate() - 15, 3, 0, 0, 0)); // Medianoche UTC hace 14 días
    endDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), 2, 59, 59, 999)); // Final del día UTC
  } else if (days === 30) {
    // Último mes (en UTC)
    startDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth() - 1, now.getUTCDate() - 1, 3, 0, 0, 0)); // Medianoche UTC hace 30 días
    endDate = new Date(Date.UTC(now.getUTCFullYear(), now.getUTCMonth(), now.getUTCDate(), 2, 59, 59, 999)); // Final del día UTC
  }

  // Llamar a la función para obtener los datos
  const startEpoch  = startDate.getTime();
  const endEpoch = endDate.getTime();

  // Ahora puedes usar startEpoch y endEpoch para hacer la consulta a Firestore
  console.log("startDate:", startDate);
  console.log("endDate:", endDate);
  console.log("startEpoch:", startEpoch);
  console.log("endEpoch:", endEpoch);


  // Convertir a segundos para Firestore
  const startDateEpoch = Math.floor((startDate.getTime() / 1000) - 10800);
  const endDateEpoch = Math.floor((endDate.getTime() / 1000) - 10800);

  const dataByTank = { 1: [], 2: [], 3: [], 4: [], 5: [], 6: [] };

  const querySnapshot = await window.db
    .collection("Produccion")
    .where(
      firebase.firestore.FieldPath.documentId(),
      ">=",
      startDateEpoch.toString()
    )
    .where(
      firebase.firestore.FieldPath.documentId(),
      "<=",
      endDateEpoch.toString()
    )
    .orderBy(firebase.firestore.FieldPath.documentId()) // Ordenar por ID (epoch time)
    .get();

  querySnapshot.forEach((doc) => {
    const docData = doc.data();
    const epochTime = parseInt(doc.id); // Convertir el ID a número
    const fechaHoraUTC = new Date((epochTime + 10800) * 1000); // Convertir epoch time a Date en milisegundos

    const fechaHora = fechaHoraUTC;

    if (dataByTank[docData.Tanque]) {
      dataByTank[docData.Tanque].push({
        x: fechaHora, // fecha para el eje X
        y: parseFloat(docData.temperatura), // Convierte la temperatura a número para el eje Y
      });
    }
  });

  return dataByTank;
}

// Configuración del gráfico
const ctx = document.getElementById("temperatureChart").getContext("2d");
let temperatureChart = new Chart(ctx, {
  type: "line",
  data: {
    datasets: [
      { label: "Tanque 1", data: [], borderColor: "red", fill: false },
      { label: "Tanque 2", data: [], borderColor: "blue", fill: false },
      { label: "Tanque 3", data: [], borderColor: "green", fill: false },
      { label: "Tanque 4", data: [], borderColor: "orange", fill: false },
      { label: "Tanque 5", data: [], borderColor: "purple", fill: false },
      { label: "Tanque 6", data: [], borderColor: "cyan", fill: false },
    ],
  },
  options: {
    scales: {
      x: {
        type: "time",
        time: {
          unit: "day",
          tooltipFormat: "PP",
          displayFormats: {
            day: "MMM d",
            hour: "HH:mm",
          },
        },
        title: {
          display: true,
          text: "Fecha",
        },
        bounds: "data", 
      },
      y: {
        title: {
          display: true,
          text: "Temperatura (°C)",
        },
        beginAtZero: true,
      },
    },
    plugins: {
      tooltip: {
        callbacks: {
          title: function (tooltipItems) {
            const date = tooltipItems[0].label;
            return [`${date}`];
          },
          label: function (tooltipItem) {
            const datasetLabel = tooltipItem.dataset.label || "";
            const temperature = tooltipItem.raw.y.toFixed(1) + " °C";
          
            // Crear la fecha y sumar 3 horas
            const originalTime = new Date(tooltipItem.raw.x);
            originalTime.setHours(originalTime.getHours() - 3);
          
            // Formatear la hora con el ajuste
            const time = originalTime.toISOString().substr(11, 5) + " hs";
          
            return [`${datasetLabel}: ${temperature}`, time];
          }
          
        },
      },
      zoom: {
        zoom: {
          wheel: {
            enabled: true, 
            speed: 0.1, 
          },
          pinch: {
            enabled: true, 
          },
        },
        pan: {
          enabled: true, 
          mode: "xy", 
          speed: 10, 
        },
      },
    },
  },
});
