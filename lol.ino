//#include <WiFi.h>
#include <WebSocketClient.h>
#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include <Ticker.h>

DHTesp dht;
Ticker timer;

const char* ssid     = "free";
const char* password = "12345677";
 
char path[] = "/arduino";
char host[] = "192.168.43.56";
 
WebSocketClient webSocketClient;
WiFiClient client;
const int output5 = 5;
void setup() {
  Serial.begin(115200);
  //Initialise LED pin
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
 // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  dht.setup(2, DHTesp::AUTO_DETECT);
 
  delay(5000);
 
  if (client.connect(host, 8000)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }
 
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }
 
}

float humidity,temperature;

void getData() {
  delay(2000);
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  Serial.print("Humidity : ");
  Serial.print(humidity);
  Serial.print("\tTemperature : ");
  Serial.println(temperature);
}
 
void loop() {
  String data;
  getData();
  if (client.connected()) {
    Serial.println("Client Connected!");
    String alpha = String(temperature);
    String beta = String(humidity);
    webSocketClient.sendData(alpha);
    webSocketClient.sendData(beta);
 
    webSocketClient.getData(data);
    if (data.length() > 0) {
      if(data=="true"){
        Serial.println("IT IS TRUE");
        digitalWrite(output5, HIGH);
        digitalWrite(2, HIGH);
      }
      else{
        Serial.println("F");
        digitalWrite(output5, LOW);
        digitalWrite(2, LOW);
      }
      Serial.print("Received data: ");
      Serial.println(data);
    }
 
  } else {
    Serial.println("Client disconnected.");
  }
 
  delay(3000);
 
}
