#include <TinyGPSPlus.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

const char *AP_SSID = "XPANAR";
const char *AP_PWD = "kiyabatkiyabat";

TinyGPSPlus gps;
Adafruit_BMP085 bmp;
WiFiMulti wifiMulti;
HTTPClient http;

const int laser = 13;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  delay(3000);
  //Set the pin as an input pullup
  pinMode(laser, INPUT);

  wifiMulti.addAP(AP_SSID, AP_PWD);

  if (!bmp.begin()) 
  {
	Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
	while (1) {}
}
}
void loop() {
postDataToServer();
delay(2000);
}

int get_laser()
{    bool value = digitalRead(laser);
    Serial.print("laser = ");
    Serial.println(value);
    return value;
    delay(100);
}

String get_gps()
{  double lat;
  double lng;
  String data="";
  
    while (Serial2.available() > 0)
    if (gps.encode(Serial2.read()))
      {
        Serial.print(F("Location: "));
  if (gps.location.isValid()){
    Serial.print("Lat: ");
    Serial.print(gps.location.lat(), 6);
    lat = gps.location.lat();
    Serial.print(F(","));
    Serial.print("Lng: ");
    Serial.print(gps.location.lng(), 6);
    lng = gps.location.lng();
    Serial.println();
    break;
  }  
  else
  {Serial.println(F("INVALID"));
  break;}
      }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {Serial.println(F("No GPS detected: check wiring."));
   while (true);
  }
  data = String(lat,8);
  data = data + ",";
  data = data + String(lng,8);
  Serial.println(data);
  return data;
}

int get_presure()
{ Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  int val = bmp.readPressure(); 
    Serial.println(" Pa");
  delay(100);
  return val;
}

void postDataToServer() {
  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  if (wifiMulti.run() == WL_CONNECTED) {
    http.begin("https://aplish.my.salesforce-sites.com/logger/services/apexrest/log");  
    http.addHeader("Content-Type", "application/json");         
    StaticJsonDocument<200> doc;
    // Add values in the document
    //
    doc["deviceId"] = "123123";
    doc["time"] = 1351824120;
    doc["deviceId"] = "breach in box";
    doc["level"] = "red";
    doc["location"] = get_gps(); 
    doc["Pressure"] = get_presure();
    doc["laser"] = get_laser();
    // Add an array.
    //
    JsonArray data = doc.createNestedArray("data");
   // data.add(48.756080);
    //data.add(2.302038);
     
    String requestBody;
    serializeJson(doc, requestBody);
     
    int httpResponseCode = http.POST(requestBody);
 
    if(httpResponseCode>0){
       
      String response = http.getString();                       
      Serial.print("httpResponseCode = ");
      Serial.println(httpResponseCode);
      Serial.print("response = ");
      Serial.println(response);
     
    }
  }
  Serial.println("----------------------------------------------");
}