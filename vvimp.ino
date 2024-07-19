#include <ESP8266WebServer.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>
#include <ESP8266WiFi.h>

#define DHTTYPE DHT22
#define DHTPIN 14  //D5 pin = GPIO pin 14
#define DS18B20 2  //D4 pin = GPIO pin 2
#define REPORTING_PERIOD_MS 1000

float temperature, humidity, bodytemperature;

/*Put your SSID & Password*/
const char *ssid = "Fastnet";      // Enter SSID here
const char *password = "priyanka15@"; // Enter Password here

DHT dht(DHTPIN, DHTTYPE);
;                                  //--> Initialize DHT sensor, DHT dht(Pin_used, Type_of_DHT_Sensor);
uint32_t tsLastReport = 0;
OneWire oneWire(DS18B20);
DallasTemperature sensors(&oneWire);

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(115200);
  pinMode(16, OUTPUT);
  delay(100);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  Serial.println(F("Connecting to "));
  Serial.println(ssid);

  // Connect to your local Wi-Fi network
  WiFi.begin(ssid, password);

  // Check if Wi-Fi is connected to the Wi-Fi network
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (attempts == 20)
  {
    Serial.println(F("Failed to connect to WiFi. Please check your credentials."));
    while (1)
    {
      delay(1000);
    }
  }

  Serial.println("");
  Serial.println(F("WiFi connected..!"));
  Serial.print(F("Got IP: "));
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handle_OnConnect); // Update to use HTTP_GET
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println(F("HTTP server started"));
}

void loop()
{
  server.handleClient();
  sensors.requestTemperatures();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    bodytemperature = sensors.getTempCByIndex(0);

    temperature = t;
    humidity = h;

    Serial.print("Room Temperature: ");
    Serial.print(t);
    Serial.println("°C");

    Serial.print("Room Humidity: ");
    Serial.print(h);
    Serial.println("%");

    Serial.print("Body Temperature: ");
    Serial.print(bodytemperature);
    Serial.println("°C");

    Serial.println("*********************************");
    Serial.println();
    tsLastReport = millis();
  }
}

void handle_OnConnect()
{
  server.send(200, "text/html", SendHTML(temperature, humidity, bodytemperature));
}

void handle_NotFound()
{
  server.send(404, "text/plain", "Not found");
}


  String SendHTML(float temp, float hum, float bodytemp) {
    String html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css'><title>Temperature Monitoring System</title><style>body{font-family: Arial, sans-serif;margin: 0;padding: 0;background-color: #f4f4f4;}header{background-color: #333;color: #fff;padding: 10px;text-align: center;}section{max-width: 800px;margin: 20px auto;padding: 20px;background-color: #fff;box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);}footer{background-color: #333;color: #fff;padding: 10px;text-align: center;}.sensors{margin-top: 20px;}.sensor{display: flex;align-items: center;justify-content: space-between;}.sensor i{margin-right: 10px;}.sensor-labels{flex-grow: 1;margin-left: 10px;}.units{font-size: 80%;}</style></head><body><header><h1>Temperature Monitoring System</h1></header><section><h2>Welcome to My Webpage!</h2><div class='sensors'><p class='sensor'><i class='fas fa-thermometer-half' style='color:#0275d8'></i><span class='sensor-labels'> Room Temperature </span>" + String(temp) + "<sup class='units'>°C</sup></p><hr></div><div class='sensors'><p class='sensor'><i class='fas fa-tint' style='color:#5bc0de'></i><span class='sensor-labels'> Room Humidity </span>" + String(hum) + "<sup class='units'>%</sup></p><hr></div><div class='sensors'><p class='sensor'><i class='fas fa-thermometer-full' style='color:#d9534f'></i><span class='sensor-labels'> Body Temperature </span>" + String(bodytemp) + "<sup class='units'>°C</sup></p><hr></div></section><footer><p>&copy; 2023 Temperature Monitoring System. All rights reserved.</p></footer></body></html>";
    return html;
}

