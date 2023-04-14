#include <WiFi.h>
#include <WebServer.h>
//#include <ESP32Time.h>
#include <ArduinoJson.h>
#include <fstream> 
#include "FS.h"
#include "SPIFFS.h"

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server(80);

const int SIZE_OF_DLI = 9999;
int data[96];
int DLI[SIZE_OF_DLI];
unsigned long lastMillis = millis();
unsigned long lastMillisDay = millis();
const unsigned long interval = 1000UL; //15 minutes  right now its 1 seconds
const unsigned long day = 96000UL;//a day   right now its 1.6 minutes
int indexOfArray = 0;
int indexOfDLI = 0;
StaticJsonDocument<1536> doc;

int sumOfData() {
  float total = 0;
  for (int i = 0; i < 96; i++) {
    total = total + (3.6*.001* float(data[i]) * 0.25);
  }
  return (int)total;
}

int avgDLI() {
  int avg = 0;
  for (int i = 0; i < indexOfDLI; i++) {
    avg += DLI[i];
  }
  Serial.print("avgDLI: ");
  Serial.println(avg/indexOfDLI);
  return (avg/indexOfDLI);
}

const String processDLI() {
  int avgDLIInt = avgDLI();
  float distance = 99999;
  const char* item_name = "";
  if (indexOfDLI == 0) {
    return String(item_name);
  }
  for (JsonObject item : doc.as<JsonArray>()) {
    int DLI_low = item["DLI"]["low"]; 
    int DLI_high = item["DLI"]["high"]; 
    float avg_DLI_item = (DLI_low + DLI_high) / 2.0;
    if (abs(avg_DLI_item - float(avgDLIInt)) < distance) {
      distance = abs(avg_DLI_item - float(avgDLIInt));
      item_name = item["name"];
    }
  }
  Serial.print("distance: ");
  Serial.println(distance);
  return String(item_name);
}
void handleRoot() {
 String s = SendHTML(); //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleAnalog() {
 int a = analogRead(4);
 String analogValue = String(a);
    
 server.send(200, "text/plane", analogValue); //Send ADC value only to client ajax request
}

void handleArray() {
  String strOutput = "";
  for (int i = 0; i < indexOfDLI; i++) {
    int a = DLI[i];
    strOutput += "<li>" + String(a) + "</li>";
  }
  server.send(200, "text/plane", strOutput);

}

void handlePlantName() {
  String plantName = processDLI();
  server.send(200, "text/plane", plantName);
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}



void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handleRoot);
  server.on("/readAnalog", handleAnalog);
  server.on("/readArray", handleArray);
  server.on("/readPlantName", handlePlantName);
  
  server.onNotFound(handle_NotFound);
  
  server.begin();
  // rtc.setTime(30, 24, 15, 11, 4, 2023); // 11th April 2023 15:24:30
  Serial.println("HTTP server started :)");

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File file = SPIFFS.open("/plantDatabase.json", "r");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  DeserializationError error = deserializeJson(doc, file);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
}
void loop() {

  if (indexOfArray >= 96) {
    indexOfArray = 0;    
  }
  if (indexOfDLI >= SIZE_OF_DLI) {
    indexOfDLI = 0;
  }
  if (millis() - lastMillis >= interval) {
    lastMillis = millis();  //get ready for the next iteration
    int analogValue = analogRead(4);
    data[indexOfArray] = analogValue;
    Serial.print("data :");
    Serial.println(data[indexOfArray]);

    indexOfArray++;
  }
  if (millis() - lastMillisDay >= day) {
    lastMillisDay = millis();
    DLI[indexOfDLI] = sumOfData();
    Serial.print("DLI: ");
    Serial.println(DLI[indexOfDLI]);
    indexOfDLI++;
  }
  
  server.handleClient();
  delay(100);

}



String SendHTML(){
  //HTML
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  //STYLES
  ptr +="<style>html { font-family: TT Ramillas; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px; background-color: #F9F6EE} h1 {color: #063;margin: 50px auto 30px;} h3 {color: #063;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #063;border: none;color: green;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #063;}\n";
  ptr +=".button-on:active {background-color: #063;}\n";
  ptr +=".button-off {background-color: #063;}\n";
  ptr +=".button-off:active {background-color: #063;}\n";
  ptr +="p {font-size: 13px;color: #063;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  //STYLE END
  ptr +="</head>\n";
  //BODY
  ptr +="<body>\n";
  ptr +="<h1>Solar Irradiance Sensor</h1>\n";
  ptr +="<h3>Average Irradiance Level:<br>";
  ptr +="Best Plants For Your Garden!</h3>\n";

  ptr +="<h6>Data: ";
  ptr += "<span id=\"analogValue\">0</span>";
  ptr += "<h6>Best Plant: ";
  ptr += "<span id=\"plantName\">waiting for Data</span>";
  ptr += "<h6>Data list:</h6>";
  ptr += "<ul id =\"dataList\"></ul>";
  ptr += "</p>\n";

  //SCRIPT
  ptr +="<script>";
  //2000mSeconds update rate
  ptr += "setInterval(function() {getData(); getArrayData(); getPlantName();}, 2000);\n";
  ptr += "function getData() {";
  ptr += "var xhttp = new XMLHttpRequest();";
  ptr += "xhttp.onreadystatechange = function() {";
  ptr += "document.getElementById(\"analogValue\").innerHTML = this.responseText; };";
  ptr += "xhttp.open(\"GET\", \"readAnalog\", true); xhttp.send(); }\n";
// for array
  ptr += "function getArrayData() {";
  ptr += "var xhttp = new XMLHttpRequest();"; 
  ptr += "xhttp.onreadystatechange = function() {";
  ptr += "document.getElementById(\"dataList\").innerHTML = this.responseText; };";
  ptr += "xhttp.open(\"GET\", \"readArray\", true); xhttp.send(); }\n";
// for plantName
   ptr += "function getPlantName() {";
  ptr += "var xhttp = new XMLHttpRequest();"; 
  ptr += "xhttp.onreadystatechange = function() {";
  ptr += "document.getElementById(\"plantName\").innerHTML = this.responseText; };";
  ptr += "xhttp.open(\"GET\", \"readPlantName\", true); xhttp.send(); }\n";
  //SCRIPT END
  ptr += "</script>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
