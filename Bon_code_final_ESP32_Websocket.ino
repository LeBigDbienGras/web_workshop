#include <WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "PoleDeVinci_IFT";
const char* password = "*c.r4UV@VfPn_0";

// Création d'un serveur WebSocket sur le port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Définition des broches
const int ledPin = 6;
const int motorPin = 5;
const int photoresistorPin = 3;
bool ledState = false;
bool motorState = false;

void setup() {
  Serial.begin(115200);
  
  // Connexion au WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connexion au WiFi...");
  }
  Serial.println("WiFi connecté. Adresse IP : ");
  Serial.println(WiFi.localIP());
  
  // Démarrer le serveur WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(motorPin, OUTPUT);
  digitalWrite(motorPin, LOW);
}

void loop() {
  webSocket.loop(); // Gère les événements WebSocket
  delay(10);
  
  // Lire la valeur de la photorésistance
  int lightValue = analogRead(photoresistorPin);

  // Envoyer la valeur de la photorésistance via WebSocket toutes les secondes
  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 1000) {
    webSocket.broadcastTXT(String("LIGHT_VALUE:") + lightValue);
    lastSendTime = millis();
    Serial.print("Valeur de la photorésistance : ");
    Serial.println(lightValue);
  }

  // Contrôler la LED et le moteur selon leurs états
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  digitalWrite(motorPin, motorState ? HIGH : LOW);
}
// Fonction pour gérer les événements WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*)payload);
    Serial.println(message);

    if (message.startsWith("LED_BRIGHTNESS:")) {
      int brightness = message.substring(15).toInt(); // Extrait la valeur de luminosité
      analogWrite(ledPin, brightness); // Ajuste la luminosité de la LED
      Serial.print("Luminosité de la LED réglée à : "); // Affiche la luminosité réglée
      Serial.println(brightness);

    } else if (message == "LED_ON") {
      ledState = true;
      Serial.println("LED activée.");

    } else if (message == "LED_OFF") {
      ledState = false;
      digitalWrite(ledPin, LOW);
      Serial.println("LED éteinte.");

    } else if (message == "MOTOR_ON") {
      motorState = true;
      Serial.println("Moteur activé.");

    } else if (message == "MOTOR_OFF") {
      motorState = false;
      digitalWrite(motorPin, LOW);
      Serial.println("Moteur éteint.");
    }
  }
}
