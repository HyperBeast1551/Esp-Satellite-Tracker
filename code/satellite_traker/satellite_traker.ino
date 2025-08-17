#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <LiquidCrystal_I2C.h>  // I²C LCD library
#include <ArduinoJson.h>

// === Wi-Fi and API Setup ===
const char* ssid = "FTTH-6A36_EXT";
const char* password = "rajesh4007";
const String API_KEY = "JGYW6B-THQMLM-V9XP7A-5IEI";
const String OBSERVER_LAT = "28.6139";
const String OBSERVER_LON = "77.2090";
const String OBSERVER_ALT = "0";

ESP8266WebServer server(80);
WiFiClientSecure client;

// === Change: Initialize I²C LCD (address 0x27, 16 columns, 2 rows) ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Satellite Data Struct ===
struct Satellite {
  String name, localTime, utcTime, azimuth, ra, decl;
  float lat, lon, altKm, altMi, elevation;
};

Satellite sat;

// === Timers ===
unsigned long lastFetchTime = 0;
const unsigned long refreshInterval = 10000;

unsigned long lastLCDUpdate = 0;
int lcdPage = 0;
const unsigned long lcdInterval = 2000;

// === HTML Page ===
const char* htmlForm = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="10">
  <title>Satellite Tracker</title>
  <style>
    body {
      background-color: #0f172a;
      color: #f8fafc;
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      text-align: center;
      padding: 20px;
    }
    h2 {
      color: #38bdf8;
      font-size: 2em;
    }
    form, .preset {
      background: #1e293b;
      padding: 20px;
      border-radius: 10px;
      display: inline-block;
      box-shadow: 0 0 20px rgba(56, 189, 248, 0.3);
      margin-bottom: 20px;
    }
    input[type="text"] {
      padding: 10px;
      border: none;
      border-radius: 5px;
      font-size: 1em;
      margin-right: 10px;
    }
    input[type="submit"], .preset a {
      padding: 10px 20px;
      background-color: #38bdf8;
      border: none;
      border-radius: 5px;
      font-weight: bold;
      color: #0f172a;
      cursor: pointer;
      text-decoration: none;
      margin: 5px;
      display: inline-block;
    }
    input[type="submit"]:hover, .preset a:hover {
      background-color: #0ea5e9;
    }
  </style>
</head>
<body>
  <h2>Track a Satellite</h2>
  <form action="/track">
    <input type="text" name="id" placeholder="Enter NORAD ID" required>
    <input type="submit" value="Track">
  </form>
  <div class="preset">
    <h3>Popular Satellites</h3>
    <a href="/track?id=25544">SPACE STATION</a>
    <a href="/track?id=35937">SES 1</a>
    <a href="/track?id=33591">NOAA 19</a>
    <a href="/track?id=29155">GOES 13</a>
    <a href="/track?id=25338">NOAA 15</a>
    <a href="/track?id=28654">NOAA 18</a>
    <a href="/track?id=25994">TERRA</a>
    <a href="/track?id=27424">AQUA</a>
    <a href="/track?id=38771">METOP-B</a>
    <a href="/track?id=37849">SUOMI NPP</a>
    <a href="/track?id=36411">GOES 15</a>
    <a href="/track?id=40967">FOX-1A (AO-85)</a>
    <a href="/track?id=27607">SAUDISAT 1C</a>
    <a href="/track?id=41332">KMS-4</a>
    <a href="/track?id=37820">TIANGONG 1</a>
    <a href="/track?id=40069">METEOR M2</a>
    <a href="/track?id=25724">ASIASAT 3S</a>
    <a href="/track?id=36031">NSS 12</a>
    <a href="/track?id=31314">AGILE</a>
    <a href="/track?id=40146">MEASAT 3B</a>
  </div>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlForm);
}

void handleTrack() {
  if (!server.hasArg("id")) {
    server.send(400, "text/plain", "No satellite ID provided.");
    return;
  }
  String id = server.arg("id");
  if (millis() - lastFetchTime > refreshInterval) {
    fetchSatelliteData(id);
    lastFetchTime = millis();
  }

  String html = "<html><head><meta http-equiv='refresh' content='10'>";
  html += "<style>body{background:#0f172a;color:#f8fafc;font-family:'Segoe UI',sans-serif;padding:20px;text-align:center;}h2{color:#38bdf8;}div.card{background:#1e293b;padding:20px;border-radius:10px;box-shadow:0 0 20px rgba(56,189,248,0.3);display:inline-block;text-align:left;}a{color:#38bdf8;text-decoration:none;}a:hover{text-decoration:underline;}</style>";
  html += "</head><body><h2>Satellite Data</h2><div class='card'>";
  html += "<strong>Name:</strong> " + sat.name + "<br>";
  html += "<strong>Lat:</strong> " + String(sat.lat) + "<br>";
  html += "<strong>Lon:</strong> " + String(sat.lon) + "<br>";
  html += "<strong>Altitude:</strong> " + String(sat.altKm) + " km / " + String(sat.altMi) + " mi<br>";
  html += "<strong>Azimuth:</strong> " + sat.azimuth + "<br>";
  html += "<strong>Elevation:</strong> " + (sat.elevation > -900 ? String(sat.elevation) : "N/A") + "<br>";
  html += "<strong>UTC:</strong> " + sat.utcTime + "<br>";
  html += "<strong>Local:</strong> " + sat.localTime + "<br>";
  html += "<strong>RA:</strong> " + sat.ra + "<br>";
  html += "<strong>Declination:</strong> " + sat.decl + "<br><br>";
  html += "<a href='https://www.google.com/maps?q=" + String(sat.lat) + "," + String(sat.lon) + "' target='_blank'>View on Map</a>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void fetchSatelliteData(String id) {
  String url = "https://api.n2yo.com/rest/v1/satellite/positions/" + id + "/" + OBSERVER_LAT + "/" + OBSERVER_LON + "/" + OBSERVER_ALT + "/2/&apiKey=" + API_KEY;
  client.setInsecure();
  HTTPClient https;
  if (!https.begin(client, url)) return;

  int code = https.GET();
  if (code <= 0) {
    https.end();
    return;
  }

  String payload = https.getString();
  StaticJsonDocument<4096> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    https.end();
    return;
  }

  sat.name = doc["info"]["satname"].as<String>();
  JsonObject pos = doc["positions"][0];
  sat.lat = pos["satlatitude"];
  sat.lon = pos["satlongitude"];
  sat.altKm = pos["sataltitude"];
  sat.altMi = sat.altKm * 0.621371;
  sat.azimuth = pos.containsKey("azimuth") ? String(pos["azimuth"].as<float>(), 2) + " deg" : "N/A";
  sat.elevation = pos.containsKey("elevation") ? pos["elevation"].as<float>() : -999;

  long timestamp = pos["timestamp"];
  sat.localTime = timestamp != 0 ? formatLocalTime(timestamp) : "N/A";
  sat.utcTime = timestamp != 0 ? formatUTCTime(timestamp) : "N/A";
  sat.ra = pos.containsKey("ra") ? pos["ra"].as<String>() : "N/A";
  sat.decl = pos.containsKey("dec") ? pos["dec"].as<String>() : "N/A";
  https.end();
}

String formatLocalTime(long timestamp) {
  time_t t = timestamp + (5 * 3600 + 30 * 60);
  struct tm *ptm = localtime(&t);
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return String(buffer);
}

String formatUTCTime(long timestamp) {
  time_t t = timestamp;
  struct tm *ptm = gmtime(&t);
  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return String(buffer);
}

void updateLCD() {
  lcd.clear();
  switch (lcdPage) {
    case 0:
      lcd.setCursor(0, 0); lcd.print(sat.name.substring(0, 16));
      lcd.setCursor(0, 1); lcd.print("Lat:"); lcd.print(sat.lat, 1);
      break;
    case 1:
      lcd.setCursor(0, 0); lcd.print("Lon:"); lcd.print(sat.lon, 1);
      lcd.setCursor(0, 1); lcd.print("Alt:"); lcd.print(sat.altKm, 0); lcd.print("km");
      break;
    case 2:
      lcd.setCursor(0, 0); lcd.print("UTC: "); lcd.print(sat.utcTime);
      lcd.setCursor(0, 1); lcd.print("IST: "); lcd.print(sat.localTime);
      break;
    case 3:
      lcd.setCursor(0, 0); lcd.print("Az:"); lcd.print(sat.azimuth);
      lcd.setCursor(0, 1); lcd.print("El:"); lcd.print(sat.elevation);
      break;
  }
  lcdPage = (lcdPage + 1) % 4;
}

void setup() {
  Serial.begin(115200);
  lcd.init();       // I²C LCD init
  lcd.backlight();  // Turn on backlight
  lcd.clear();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi Connected");
  server.on("/", handleRoot);
  server.on("/track", handleTrack);
  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - lastLCDUpdate > lcdInterval) {
    updateLCD();
    lastLCDUpdate = millis();
  }
}
