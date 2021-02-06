  float abs_humidity = (6.112 * pow(2.71828, (17.67 * temperature)/ (temperature + 243.5)) * rel_humidity * 2.1674) / (273.15 + temperature);
#include <WiFi.h>  //Dodanie biblioteki do obsługi WiFI 
#include "DHT.h"i  //Dodanie biblioteki do obsługi czujnika DHT
#include <ThingSpeak.h> //Dodanie biblioteki do obsługi Thingspeak

String apiKey = "9FUSTO9ZF8P089OI";         // Kod API do kanału Thingspeal
const char *ssid =  "********";    // login do sieci WiFI
const char *pass =  "*********";             // hasło do sieci WiFI
const char* server = "api.thingspeak.com";  //nazwa serwera 
WiFiClient client;                          //obiekt do obsługi połączenia

const long CHANNEL = 1033265;                //ID kanału Thingspeak       
const char *WRITE_API = "9FUSTO9ZF8P089OI";  //Kod API do kanału Thingspeak
const int sensor = 23; //Numer pinu czujnika
DHT dht; //Obiekt czujnika

void setup() {
  //konfiguracja pinow wejscia
  pinMode(sensor, INPUT);
  Serial.begin(9600);
  while(!Serial);
  //inicjalizacja czujnika
  dht.setup(sensor);
  //laczenie z siecia WiFi
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  //wydrukowanie danych sieci 
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Serwer DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Maska podsieci: ");
  Serial.println(WiFi.subnetMask()); 
  Serial.print("Adres bramy sieci: ");
  Serial.println(WiFi.gatewayIP());
  
  ThingSpeak.begin(client); //rozpoczenie polaczenia z ThingSpeak 
}

void loop() {
  float  rel_humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  //obliczenie wartosci wilgotnosci bezwzglednej i punktu rosy
  float abs_humidity = (6.112 * pow(2.71828, (17.67 * temperature)/ (temperature + 243.5)) * rel_humidity * 2.1674) / (273.15 + temperature);
  
  if (dht.getStatusString() == "OK") {
      ThingSpeak.setField(1, abs_humidity);
      ThingSpeak.setField(2, temperature);
      // Zapis na kanał Thingspeak
      int x = ThingSpeak.writeFields(CHANNEL, WRITE_API);
      if (x == 200) {
        Serial.println("Channel update successful.");
      }
      else {
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    client.stop();
    delay(10000);
  }
}
