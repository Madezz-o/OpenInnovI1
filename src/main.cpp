#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Définition des constantes de l'application
#define STASSID "Livebox-8990"
#define STAPSK "DPmjLVakLHXz9rZQdf"
#define DELAY 1000
int val;
int touch = 0;
int oldNbTouch = 0;

// Définition des variables plus parlantes pour le reste du programme
const char *wifi_ssid = STASSID;
const char *wifi_password = STAPSK;

// Pour le serveur
const char *sadrr_serveur = "31.34.157.18";
WiFiClient espClient;
PubSubClient client(espClient);

DynamicJsonDocument jsonPoints(1024);

// Configuration du Wifi
void config_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

// Connexion au serveur
void connect()
{
  String clientId = "ESP8266Client-";
  clientId += String(random(0xffff), HEX);

  while (!client.connected())
  {
    if (client.connect(clientId.c_str()))
    {
      client.subscribe("EPSI/OpenInnov/Cible1");
    }
    else
    {
      // Attendre 3 secondes avant de recommencer
      delay(3000);
    }
  }
}

// Configuration du capteur de vibration
void config_vibration_sensor()
{
  int val = digitalRead(D1);
  //Serial.println(val); // Lecture de l'état du capteur sur l'entrée digitale 7
  if (val == HIGH)
  {
    touch++;
    Serial.println("Nombre de touché : ");
    Serial.println(touch);
    delay(500);
  }
  else
  {
  }
}

void setup()
{
  // Configuration laison serie pour écrire dans la console
  Serial.begin(74880);
  pinMode(D1, INPUT); // buttonpin en tant qu'entrée

  config_wifi();
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  // Configuration de la communication aevc le serveur
  client.setServer(sadrr_serveur, 1884);
}

void loop()
{
  delay(DELAY); // Délai de bouclage
  config_vibration_sensor();

  if (!client.connected())
  {
    connect();
  }

  client.loop();

  if (oldNbTouch != touch)
  {
    jsonPoints["touch"] = touch;
    String nbPoint_json;
    serializeJson(jsonPoints, nbPoint_json);
    client.publish("EPSI/OpenInnov/Cible1", const_cast<char *>(nbPoint_json.c_str()));
    oldNbTouch = touch;
  }
}