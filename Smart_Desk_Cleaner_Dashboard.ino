
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

#define WATER_SENSOR 34
#define FAN_PIN 26
#define MOTOR_IN1 27
#define MOTOR_IN2 14

// Adjust after checking your sensor readings
int waterThreshold = 2000;

bool waterDetected = false;

void handleRoot() {
  String page = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>Smart Desk Cleaner</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body {
        font-family: Arial;
        text-align: center;
        background: #eef3f8;
        padding: 20px;
      }
      .card {
        background: white;
        padding: 20px;
        margin: 15px auto;
        max-width: 350px;
        border-radius: 12px;
        box-shadow: 0 2px 8px #bbb;
      }
      h1 { color: #174a70; }
      .status { font-size: 22px; font-weight: bold; }
    </style>
  </head>
  <body>
    <h1>Smart Desk Cleaner</h1>
    <p>Live Monitoring Dashboard</p>

    <div class="card">
      <h3>Water Detection</h3>
      <div class="status" id="water">Checking...</div>
    </div>

    <div class="card">
      <h3>Suction Fan</h3>
      <div class="status" id="fan">Checking...</div>
    </div>

    <div class="card">
      <h3>Cotton Cleaning Roller</h3>
      <div class="status" id="roller">Checking...</div>
    </div>

    <script>
      function updateStatus() {
        fetch('/status')
          .then(response => response.json())
          .then(data => {
            document.getElementById('water').innerText =
              data.water ? 'WATER DETECTED' : 'NO WATER';

            document.getElementById('fan').innerText =
              data.fan ? 'ON' : 'OFF';

            document.getElementById('roller').innerText =
              data.roller ? 'ON' : 'OFF';
          });
      }

      setInterval(updateStatus, 1000);
      updateStatus();
    </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", page);
}

void handleStatus() {
  String json = "{";
  json += "\"water\":" + String(waterDetected ? "true" : "false") + ",";
  json += "\"fan\":" + String(waterDetected ? "true" : "false") + ",";
  json += "\"roller\":" + String(waterDetected ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pinMode(FAN_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  digitalWrite(FAN_PIN, LOW);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);

  server.begin();
}

void loop() {
  server.handleClient();

  int sensorValue = analogRead(WATER_SENSOR);

  Serial.println(sensorValue);

  // Typical raindrop sensor: lower reading means more water
  waterDetected = (sensorValue < waterThreshold);

  if (waterDetected) {
    digitalWrite(FAN_PIN, HIGH);

    digitalWrite(MOTOR_IN1, HIGH);
    digitalWrite(MOTOR_IN2, LOW);
  }
  else {
    digitalWrite(FAN_PIN, LOW);

    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
  }

  delay(200);
}
