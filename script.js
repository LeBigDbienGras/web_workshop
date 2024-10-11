// Crée une connexion WebSocket à l'adresse IP spécifiée
const connection = new WebSocket('ws://10.1.224.39:81'); // Remplacez par l'adresse IP de votre ESP32
let ledActive = false;
let motorActive = false;

function controlLED() {
    // Crée un message selon l'état de la LED (activée ou désactivée)
    const message = ledActive ? "LED_OFF" : "LED_ON";
    connection.send(message);
    // Affiche le message dans la console pour le débogage
    console.log(`Envoi de la commande ${message}`);
    ledActive = !ledActive;
}

function controlMotor() {
    const message = motorActive ? "MOTOR_OFF" : "MOTOR_ON";
    connection.send(message);
    console.log(`Envoi de la commande ${message}`);
    motorActive = !motorActive;
}

function updateLEDBrightness(event) {
    // Récupère la valeur de luminosité à partir de l'élément d'entrée
    const brightness = event.target.value;
    connection.send(`LED_BRIGHTNESS:${brightness}`);
    console.log(`Envoi de la luminosité de la LED : ${brightness}`);
}

// Ajoute des écouteurs d'événements
document.querySelector('.big-button').addEventListener('click', controlLED);
document.querySelector('.motor-button').addEventListener('click', controlMotor);
document.querySelector('.led-slider').addEventListener('input', updateLEDBrightness);

// Fonction appelée lorsque la connexion WebSocket est ouverte
connection.onopen = () => console.log('WebSocket connecté');
connection.onmessage = event => {
    console.log('Message du serveur :', event.data);
    
    // Vérifiez si le message concerne la valeur de la lumière
    if (event.data.startsWith("LIGHT_VALUE:")) {
        // Extrait la valeur de lumière à partir du message reçu
        const lightValue = event.data.split(":")[1];
        // Met à jour le contenu de l'élément HTML avec la valeur de lumière, en effectuant une conversion pour ça augmente proportionnelement à la lumière
        document.getElementById('light-value').textContent = 4095-lightValue;
    }
};

// Fonction appelée en cas d'erreur de connexion WebSocket ou si elle est fermée
connection.onerror = error => console.error('WebSocket Error:', error);
connection.onclose = () => console.log('WebSocket déconnecté');