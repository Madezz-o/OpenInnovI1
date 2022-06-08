//#include <Arduino.h>
#include <ESP8266Wifi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Définition des constantes de l'application
#define STASSID "NomWIFI"
#define STAPSK "MdpWIFI"
#define DELAY 1000

// Définition des variables plus parlantes pour le reste du programme
const char *wifi_ssid = STASSID;
const char *wifi_password = STAPSK;

// Pour le serveur
const char *sadrr_serveur = "test.test.fr";
WiFiClient espClient;
PubSubClient client(espClient);

// Configuration du Wifi
void config_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);
}

// Configuration du capteur de vibration
void config_vibration_sensor()
{
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
      client.subscribe("");
    }
    else
    {
      // Attendre 3 secondes avant de recommencer
      delay(3000);
    }
  }
}

// Fonction de callback à chaque reception de message dans un canal abonné
void callback(char *topic, byte *payload, unsigned int length)
{
  // Traitement de la donnée arrivant sur le canal de communication
  if (strcmp(topic, "adresse") == 0)
  {
    String value = "";
    for (int i = 0; i < length; i++)
    {
      value += (char)payload[i];
    }

    String value_low = "{ value: 0 }";
    String value_high = "{ value: 1 }";

    // Augmentation du nombre de points selon le payload récupéré
    if (value == value_low)
    {
      Serial.println("Low Value");
    }
    if (value == value_high)
    {
      Serial.println("High Value");
    }
  }
}

void setup()
{
  // Configuration laison serie pour écrire dans la console
  Serial.begin(115200);

  config_vibration_sensor();
  config_wifi();

  // Configuration de la communication aevc le serveur
  client.setServer(sadrr_serveur, 1883);
  client.setCallback(callback);
}

void loop()
{
  delay(DELAY); // Délai de bouclage

  if (!client.connected())
  {
    connect();
  }

  client.loop();

  String nbPoint;
  //serializeJson(..., nbPoint);
  client.publish("adresse", const_cast<char *>(nbPoint.c_str()));
}