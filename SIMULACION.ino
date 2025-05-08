// C++ code
//

#include <SoftwareSerial.h>

SoftwareSerial espSerial(2, 3); // RX, TX para el ESP8266

// Pines del sensor de humedad
const int sensorPin = A0;

// Pin del LED 
const int ledPin = 13;

// Pin del motor 
const int motorPin = 9;

// Umbral de humedad para activar el riego 
const int humedadUmbral = 500;

void setup()
{
  // Inicializa la comunicación serial a una velocidad de 9600 baudios
  Serial.begin(9600);

  // Define el pin del LED como una salida 
  pinMode(ledPin, OUTPUT);

  // Define el pin del motor como una salida
  pinMode(motorPin, OUTPUT);

  // Define el pin del sensor como una entrada 
  pinMode(sensorPin, INPUT);
  
  espSerial.begin(115200); // Velocidad de baudios común para el ESP8266
  delay(10);
  Serial.println("Iniciando ESP8266...");
  enviarComandoAT("AT+CWMODE=1", 1000, "OK"); // Establecer modo estación 
  enviarComandoAT("AT+CWJAP=\"SSID_DE_TU_WIFI\",\"PASSWORD_DE_TU_WIFI\"", 5000, "OK"); // Conectar a la red Wi-Fi 
}

void loop()
{
  // 1. Lee el valor del sensor de humedad
  int humedadValor = analogRead(sensorPin);

  // 2. Imprime el valor de humedad en el Monitor serie para ver qué lecturas obtienes
  Serial.print("Valor de humedad: ");
  Serial.println(humedadValor);

  // 3. Comprueba si la humedad está por debajo del umbral
  if (humedadValor < humedadUmbral) {
    // Si la humedad es baja, activa el riego
    Serial.println("Activando el riego");
    digitalWrite(motorPin, HIGH); // Enciende el motor 
    digitalWrite(ledPin, HIGH);   // Enciende el LED indicador

    // Mantén el riego durante un tiempo
    delay(5000); // Riega durante 5 segundos 

    // Apaga el riego
    digitalWrite(motorPin, LOW);  // Apaga el motor
    digitalWrite(ledPin, LOW);    // Apaga el LED
    Serial.println("Riego terminado");
  } else {
    // Si la humedad es suficiente, no riegues
    Serial.println("Humedad suficiente");
    digitalWrite(motorPin, LOW);
    digitalWrite(ledPin, LOW);
  }

  // Espera un poco antes de volver a leer el sensor
  delay(1000); // Espera 2 segundos
  
 
  String datos = "humedad=" + String(humedadValor) + "&riego=" + String(digitalRead(motorPin));
  enviarComandoAT("AT+HTTPINIT", 1000, "OK");
  enviarComandoAT("AT+HTTPPARA=\"URL\",\"http://tu_servidor_iot.com/api/datos?" + datos + "\"", 5000, "OK");
  enviarComandoAT("AT+HTTPGET", 5000, "OK");
  enviarComandoAT("AT+HTTPTERM", 1000, "OK");
  delay(500);
}

String enviarComandoAT(String comando, const int tiempoEspera, String respuestaEsperada) {
  Serial.print("Enviando comando: ");
  Serial.println(comando);
  espSerial.println(comando);
  long tiempoInicio = millis();
  String respuesta = "";
  while (millis() - tiempoInicio < tiempoEspera) {
    while (espSerial.available()) {
      char c = espSerial.read();
      respuesta += c;
    }
    if (respuesta.indexOf(respuestaEsperada) > -1) {
      Serial.println("Respuesta OK");
      Serial.println(respuesta);
      return respuesta;
    }
  }
  Serial.println("Timeout o error en la respuesta:");
  Serial.println(respuesta);
  return "";
}