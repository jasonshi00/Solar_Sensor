#include <WiFi.h>
#include <WebServer.h>

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

int analogValue;

void handleRoot() {
 String s = SendHTML(); //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void handleAnalog() {
 int a = analogRead(4);
 String analogValue = String(a);
 
 server.send(200, "text/plane", analogValue); //Send ADC value only to client ajax request
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
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
   int a = analogRead(4);
   Serial.println(a);
  delay(1);
}



String SendHTML(){
  //HTML
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  //STYLES
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 13px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  //STYLE END
  ptr +="</head>\n";
  //BODY
  ptr +="<body>\n";
  ptr +="<h1>Solar Irradiance Sensor</h1>\n";
  ptr +="<h3>Jason a cute fr Mode</h3>\n";
  ptr +="<p>Data: ";
  ptr += "<span id=\"analogValue\">0</span>";
  ptr += "</p>\n";

  //SCRIPT
  ptr +="<script>";
  //2000mSeconds update rate
  ptr += "setInterval(function() {getData();}, 2000);\n";
  ptr += "function getData() {";
  ptr += "var xhttp = new XMLHttpRequest();";
  ptr += "xhttp.onreadystatechange = function() {";
  ptr += "document.getElementById(\"analogValue\").innerHTML = this.responseText; };";
  ptr += "xhttp.open(\"GET\", \"readAnalog\", true); xhttp.send(); }\n";
  //SCRIPT END
  ptr += "</script>\n";

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}