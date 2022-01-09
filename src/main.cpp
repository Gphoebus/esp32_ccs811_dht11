/******************************************************************
Mesure de co2 temperature humidite pression
les capteurs sont :
ccs811 ---> mesure eco2 et ecov
le dht 11 est remplacé par un bme280
Un serveur webb est opérationnel en mode ap sur http://192.168.4.1/

********************************************************************/
#include <Arduino.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>   // I2C library
#include "ccs811.h" // CCS811 library

#include <serveur_webb.h>

#include <Ledrgb.h>
Ledrgb maled = Ledrgb(12, 13, 14);

// Replace with your network credentials
const char *ssid = "phoebus_gaston";
const char *password = "phoebus09";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

long previousMillis = 0;
#define INTERVAL 60000 // 300000

#define DHTPIN 2 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// ---- wiring mwake on gpio 23 --------
CCS811 ccs811(23); // nWAKE on 23

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C

time_t lheure;
String ladate = "";
String heure = "";
time_t utcCalc;

const char *soft_ap_ssid = "truc2";
const char *soft_ap_password = "";

const char *PARAM_INPUT_1 = "fname";
const char *PARAM_INPUT_2 = "lname";
const char *PARAM_INPUT_3 = "commentaire";

const char *PARAM_INPUT_4 = "calibrate";
const char *PARAM_INPUT_5 = "state";

String numsalle = "garage";
String numsonde = "2";
String commentaire = "";

String boutton = "le bouton";

String letat_calibration = "on";

boolean transmit = false;

int ppm = 0;
float humidity = 0.0f;
float temperature = 0.0f;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
int ALTITUDE = 240;
int getP(double Pact, double temp) {
  return int(Pact * pow((1 - ((0.0065 * ALTITUDE) / (temp + 0.0065 * ALTITUDE + 273.15))), -5.257));
}

void printValues()
{
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" °C");

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}

void horodatage()
{
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  formattedDate = timeClient.getFormattedDate();
  // Extract date
  int splitT = formattedDate.indexOf("T");
  ladate = formattedDate.substring(0, splitT);

  // Extract time
  heure = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
}
int readCO2()
{
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2, &etvoc, &errstat, &raw);

  // Print measurement results based on status
  if (errstat == CCS811_ERRSTAT_OK)
  {
    Serial.print("CCS811: ");
    Serial.print("eco2= ");
    Serial.print(eco2);
    Serial.print(" ppm  ");
    Serial.print("etvoc= ");
    Serial.print(etvoc);
    Serial.print(" ppb  ");
    // Serial.print("raw6=");  Serial.print(raw/1024); Serial.print(" uA  ");
    // Serial.print("raw10="); Serial.print(raw%1024); Serial.print(" ADC  ");
    // Serial.print("R="); Serial.print((1650*1000L/1023)*(raw%1024)/(raw/1024)); Serial.print(" ohm");
    Serial.println();
    return eco2;
  }
  else if (errstat == CCS811_ERRSTAT_OK_NODATA)
  {
    Serial.println("CCS811: waiting for (new) data");
  }
  else if (errstat & CCS811_ERRSTAT_I2CFAIL)
  {
    Serial.println("CCS811: I2C error");
  }
  else
  {
    Serial.print("CCS811: errstat=");
    Serial.print(errstat, HEX);
    Serial.print("=");
    Serial.println(ccs811.errstat_str(errstat));
  }
  return -1;
}

// Replaces placeholder with button section in your web page
String processor(const String &var)
{
  // Serial.println(var);
  if (var == "numsalle")
  {
    return String(numsalle);
  }
  else if (var == "numsonde")
  {
    return String(numsonde);
  }
  else if (var == "co2")
  {
    return String(ppm);
  }
  else if (var == "humidite")
  {
    return String(humidity);
  }
  else if (var == "temperature")
  {
    return String(temperature);
  }
  else if (var == "date")
  {
    return String(ladate);
  }
  else if (var == "heure")
  {
    return String(heure);
  }
  else if (var == "etat")
  {
    return String(letat_calibration);
  }
  else if (var == "pression")
  {
    return String(getP((bme.readPressure() / 100.0F), bme.readTemperature()));
  }
  if (var == "BUTTONPLACEHOLDER")
  {
    String buttons = "";
    buttons += "<h4>Calibration </h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + boutton + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}

void send_Request(byte *Request, int Re_len)
{
  while (!Serial1.available())
  {
    Serial1.write(Request, Re_len); // Send request to S8-Sensor
    delay(50);
  }
}

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  unsigned status;

  // default settings
  // status = bme.begin();
  // You can also pass in a Wire library object like &Wire2
  status = bme.begin(0x76, &Wire);
  if (!status)
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1)
      delay(10);
  }

  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(soft_ap_ssid, soft_ap_password);
  IPAddress IP = WiFi.softAPIP();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    // delay(500);
    maled.bleue_clignote(4, 125);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Mac address  ");
  numsonde = WiFi.macAddress();
  Serial.println(numsonde);

  Serial.print("AP IP address: ");
  Serial.println(IP);
  Serial.println(WiFi.localIP());
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);

  dht.begin();

  Serial.println();

  // -----------  initialisation ccs811 --------------
  // Enable I2C
  Wire.begin();

  // Enable CCS811
  ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  bool ok = ccs811.begin();
  if (!ok)
    Serial.println("setup: CCS811 begin FAILED");

  // Print CCS811 versions
  Serial.print("setup: hardware    version: ");
  Serial.println(ccs811.hardware_version(), HEX);
  Serial.print("setup: bootloader  version: ");
  Serial.println(ccs811.bootloader_version(), HEX);
  Serial.print("setup: application version: ");
  Serial.println(ccs811.application_version(), HEX);

  // Start measuring
  ok = ccs811.start(CCS811_MODE_1SEC);
  if (!ok)
    Serial.println("setup: CCS811 start FAILED");
  // ----------- fin initialisation ccs811 --------------------

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2))
              {
                String inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
                String inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
                String inputMessage3 = request->getParam(PARAM_INPUT_3)->value();
                // digitalWrite(inputMessage1.toInt(), inputMessage2.toInt());
                numsalle = inputMessage1;
                numsonde = inputMessage2;
                commentaire = inputMessage3;
                Serial.print(inputMessage1);
                Serial.print(" ");
                Serial.println(inputMessage2);
                request->send_P(200, "text/html", index_html, processor);
                transmit = true;
              }
              else
              {
                horodatage();
                ppm = readCO2();

                temperature = bme.readTemperature();
                humidity = bme.readHumidity();

                request->send_P(200, "text/html", index_html, processor);
              }
            });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/calibration", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String rep1;
    String rep2;
    Serial.println("calibration");
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    if (request->hasParam(PARAM_INPUT_4) && request->hasParam(PARAM_INPUT_5)) {
      rep1 = request->getParam(PARAM_INPUT_4)->value();
      rep2 = request->getParam(PARAM_INPUT_5)->value();
      Serial.print("State ");
      Serial.println(rep2);      

      if (rep2=="1")
      {
        letat_calibration="Ok";
        Serial.println("ok");
      }
      if (rep2=="0")
      {
        letat_calibration="NOk";
        Serial.println("nok");
      }
      request->send(200, "text/plain", letat_calibration);
    }
    request->send(200, "text/plain", "OK"); });

  // Start server
  server.begin();

  timeClient.begin();
  horodatage();
  printValues();
}
void loop()
{

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL)
  {

    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }
    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    formattedDate = timeClient.getFormattedDate();
    Serial.println(formattedDate);

    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    Serial.print("DATE: ");
    Serial.println(dayStamp);
    // Extract time
    timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);
    Serial.print("HOUR: ");
    Serial.println(timeStamp);
    int leco2 = readCO2();
    String CO2s = "CO2: " + String(leco2);
    Serial.print(CO2s);
    Serial.println(" ppm");
    int P = getP((bme.readPressure() / 100.0F), bme.readTemperature());
    Serial.print(P);
    Serial.println(" hPa");
    Serial.print(bme.readTemperature());
    Serial.print(" deg C ");    
    Serial.print(bme.readHumidity());
    Serial.print(" %");  

    if (leco2 < 1000)
    {

      maled.verte(true);
    }
    else if (leco2 >= 1000 && leco2 < 1500)
    {
      maled.orange(true);
    }
    else if (leco2 >= 1500)
    {
      maled.rouge(true);
    }

    previousMillis = currentMillis;
  }
}