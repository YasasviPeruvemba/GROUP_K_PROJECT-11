//#include <WiFi.h>
#include <WebSocketClient.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
#include "DHTesp.h"
#include <Ticker.h>

DHTesp dht;
Ticker timer;

#ifndef STASSID
#define STASSID "IITI"
#define STAPSK  ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* hosta = "fwiiti1.iiti.ac.in";
const int httpsPort = 8003;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate
//iiti fingerprint
const char fingerprint[] PROGMEM = "53 40 64 12 9F F2 65 94 1D 50 6C 3B FE 4A D4 0B A2 6D 8D 3B";
//const char fingerprint[] PROGMEM = "";
 
char path[] = "/arduino";
char host[] = "https://mighty-anchorage-26613.herokuapp.com/";
 
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
  dht.setup(2, DHTesp::AUTO_DETECT);
  delay(2000); 
  IITICon();
   while(1){
    if (client.connect(host, 8000)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
    break;
  } else {
    Serial.println("Handshake failed.");
  }
  }
}

void IITICon(){
  WiFiClientSecure client;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  Serial.print("Connecting to ");
  Serial.println(hosta);

  Serial.printf("Using fingerprint '%s'\n", fingerprint);
  client.setFingerprint(fingerprint);

  if (!client.connect(hosta, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  String url = "/index.php?zone=iiti_auth";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + hosta + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n" +
               "Content-Length: 52\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n\r\n"  +
               "auth_user=bnitin&auth_pass=nitin@vcr5&accept=Sign+In\r\n"
               );

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
//  if (line.startsWith("{\"state\":\"success\"")) {
//    Serial.println("esp8266/Arduino CI successfull!");
//  } else {
//    Serial.println("esp8266/Arduino CI has failed");
//  }
  Serial.print("reply was : ");
//  Serial.println("==========");
  Serial.println(line);
//  Serial.println("==========");
//  Serial.println("closing connection");
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
      if(data=="ON"){
        Serial.println("IT IS TRUE");
        digitalWrite(output5, LOW);
        //digitalWrite(2, HIGH);
      }
      else{
        Serial.println("F");
        digitalWrite(output5, HIGH);
        //digitalWrite(2, LOW);
      }
      Serial.print("Received data: ");
      Serial.println(data);
    }
 
  } else {
    Serial.println("Client disconnected.");
  }
 
}
