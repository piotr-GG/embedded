//Dodanie biblioteki do obslugi czujnika
#include <WiFi.h>
#include "DHT.h"




String apiKey = "********";     //  Enter your Write API key from ThingSpeak
const char *ssid =  "********";     // replace with your wifi ssid and password
const char *pass =  "*********";
const char* server = "api.thingspeak.com";
WiFiClient client;


//Numer pinu czujnika
const int sensor = 23;
//Obiekt czujnika
DHT dht;

void setup() {
  //konfiguracja pinow wejscia
  pinMode(sensor, INPUT);
  Serial.begin(9600);
  while(!Serial);
  //inicjalizacja czujnika
  dht.setup(sensor);

  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  //pobranie wartosci odczytow z czujnika
  float  rel_humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  //obliczenie wartosci wilgotnosci bezwzglednej i punktu rosy
  float abs_humidity = (6.112 * pow(2.71828, (17.67 * temperature)/ (temperature + 243.5)) * rel_humidity * 2.1674) / (273.15 + temperature);
  double gamma = log(rel_humidity/100.0) + (18.678 * temperature) / (257.14 + temperature);
  double dew_point = (257.14 * gamma) / (18.678 - gamma);

  //wydrukowanie wynikow na monitor szeregowy przy statusie OK czujnika
  if (dht.getStatusString() == "OK") {
     if (client.connect(server,80))                                 //   "184.106.153.149" or api.thingspeak.com
        {  
                                
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(abs_humidity);
        postStr += "&field2=";
        postStr += String(temperature);
        postStr += "\r\n\r\n";
    
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);
                            
        Serial.println("%. Send to Thingspeak.");
    }
    client.stop();
    Serial.println("Waiting...");
    delay(10000);

  }
  
}
