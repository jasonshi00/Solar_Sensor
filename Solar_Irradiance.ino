#include <WiFi.h>
#include <WebServer.h>
<<<<<<< HEAD
#include <Preferences.h>
=======
#include <ESP32Time.h>
>>>>>>> 752d38c06c78264b39287c0220f33897e53937d5

Preferences preferences;
/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

int analogValue;

int data[9999];
unsigned long lastMillis = millis();
const long interval = 900000; //15 minutes

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
  rtc.setTime(30, 24, 15, 11, 4, 2023); // 11th April 2023 15:24:30
  Serial.println("HTTP server started");
}
void loop() {
  
  if (millis() - lastMillis >= 2*60*1000UL) {
    lastMillis = millis();  //get ready for the next iteration
    myFunction();
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
