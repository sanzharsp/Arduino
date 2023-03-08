#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>


long randNumber;
const char* ssid = "sanzhar_sapar";
const char* password = "SanzharSapar0207";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
// Domain Name with full URL Path for HTTP POST Request
//const char* serverName = "https://quantumbackend.pythonanywhere.com/api/v1/electron/post";
const char* serverName = "http://quantumbackend.pythonanywhere.com/api/v1/electron/post";

int analogPin = A0;                 // Указываем порт OUT датчика ACS712

const int averageValue = 500;       // Переменная для хранения значения количества считывания циклов 
long int sensorValue = 0;           // Переменная для хранения значения с датчика
 
float voltage = 0;                  // Переменная для хранения значения напряжения
float current = 0;     



void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

  // Random seed is a number used to initialize a pseudorandom number generator
  randomSeed(analogRead(0));
}

void loop() {

  WiFiClient client;
  for (int i = 0; i < averageValue; i++)    // Повторяем цикл
  {
    sensorValue += analogRead(analogPin);   // Считываем и записываем показания
    delay(2);                               // Пауза 2 мкс
  }

  sensorValue = sensorValue / averageValue; // Делим полученное значение 
  voltage = sensorValue * 5.0 / 1024.0;     // Расчет напряжения
  current = (voltage - 2.5) / 0.185;        // Расчет тока

  //Send an HTTP POST request every 10 seconds
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // Specify content-type header
      // If you need an HTTP request with a content type: application/json, use the following:
     http.addHeader("Content-Type", "application/json");



    float average = 0;
 
    StaticJsonBuffer<300> JSONbuffer;   //Declaring static JSON buffer
    JsonObject& JSONencoder = JSONbuffer.createObject();
 
    JSONencoder["qr"] = voltage;
    JSONencoder["id_in_electron"] = 23;
 
    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

      // Send HTTP POST request
      int httpResponseCode = http.POST(JSONmessageBuffer);
      
      /*
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(httpRequestData);*/
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Voltage: ");
      Serial.println(voltage);
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
