# 🛰️ DIY Satellite Tracker (ESP8266 + 16x2 LCD + Web Interface)

A Wi-Fi-powered real-time satellite tracker built using **ESP8266 NodeMCU**, a **16x2 LCD (non-I2C)** display, and the **N2YO Satellite Tracking API**.

It shows satellite information on an LCD and serves a stylish live-tracking web page. Designed to be beginner-friendly — no servo motors, no external databases, just real-time data!

---

## 🧰 Hardware Required

| Component                  | Quantity |
|---------------------------|----------|
| ESP8266 NodeMCU (ESP-12E) | 1        |
| 16x2 LCD (No I2C)         | 1        |
| 10k Potentiometer         | 1        |
| Breadboard + Jumper Wires | Several  |
| Micro USB Cable           | 1        |
| Internet Connection       | ✅        |

---

## 🔌 Wiring Diagram

| LCD Pin | Connects To (ESP8266 GPIO) |
|---------|----------------------------|
| RS      | D2                         |
| E       | D1                         |
| D4      | D6                         |
| D5      | D7                         |
| D6      | D5                         |
| D7      | D4                         |
| VSS     | GND                        |
| VDD     | 3.3V                       |
| V0      | Middle pin of 10k pot      |
| RW      | GND                        |
| A (LED) | 3.3V (via 220Ω resistor)   |
| K (LED) | GND                        |

---

## 🔧 Software Setup

### 1. Install Arduino IDE  
Download: https://www.arduino.cc/en/software

### 2. Add ESP8266 Support  
- Go to `File → Preferences`  
- Add this URL to **Additional Board URLs**:  
http://arduino.esp8266.com/stable/package_esp8266com_index.json
- Then go to `Tools → Board → Boards Manager`  
Search for **ESP8266**, click **Install**

### 3. Install Libraries  
Go to `Sketch → Include Library → Manage Libraries`, and install:

- `ESP8266WiFi`
- `ESP8266WebServer`
- `ESP8266HTTPClient`
- `LiquidCrystal`
- `ArduinoJson` (install version 6.x)

---

## 📥 Upload the Code

1. Open Arduino IDE  
2. Select board: `NodeMCU 1.0 (ESP-12E Module)`  
3. Paste the final project code  
4. Insert your **Wi-Fi credentials** and **N2YO API key**
5. Upload the sketch (🔼 arrow)

---

## 🌐 How to Use

1. After uploading, open **Serial Monitor** at `115200 baud`
2. Note the IP address (e.g., `192.168.1.100`)
3. Open a browser and go to `http://<your_esp_ip>`
4. Enter a satellite NORAD ID (e.g., `25544` for ISS)
5. See live satellite info and its movement — auto-refreshes every 10s

---

## 📟 LCD Display Screens

Every **2 seconds**, the display cycles through:

1. **Name** & **Latitude**  
2. **Longitude** & **Altitude**  
3. **UTC Time** & **Local Time (IST)**  
4. **Azimuth** & **Elevation**

---

## 🔐 Get a Free N2YO API Key

1. Go to https://www.n2yo.com/api/
2. Sign up and get your free key
3. Paste it in your code:
const String API_KEY = "YOUR_API_KEY";

---

## 🧠 Beginner Tips

Always use 3.3V (not 5V) for LCD if using NodeMCU directly
If LCD shows nothing, adjust the potentiometer (contrast)
To test, use NORAD ID 25544 for the International Space Station
If the web page doesn’t load, ensure you are on the same Wi-Fi network

---

## 🌍 Advanced Ideas (Optional)

Use Google Maps link to show satellite location
Add button to manually cycle LCD pages
Add buzzer or LED to signal satellite passing overhead
Save last satellite ID in EEPROM

---

## 📸 Screenshots


---

## 🧪 Tested With

NodeMCU ESP-12E
Arduino IDE 2.3.2
Chrome & Firefox Browsers
Internet Wi-Fi (2.4GHz)

---

## 🙌 Credits

Powered by N2YO API
Inspired by the global space enthusiast community

---

## 💬 For questions or improvements, feel free to open an issue or drop a PR!
