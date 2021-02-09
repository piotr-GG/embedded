#include <WiFi.h>
#include <HTTPClient.h>

#include "DHT.h"

const char* ssid     = "****";
const char* password = "*****";
// nazwa serwera
const char* serverName = "http://esp32-sensors.000webhostapp.com/post-esp-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "DHT11";
//Numer pinu czujnika
const int sensor = 23;
//Obiekt czujnika
DHT dht;

void setup() {
  //konfiguracja pinow wejscia
  pinMode(sensor, INPUT);
  //inicjalizacja czujnika
  dht.setup(sensor);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}
void loop() {
  //pobranie wartosci odczytow z czujnika
  float  rel_humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  //obliczenie wartosci wilgotnosci bezwzglednej i punktu rosy
  float abs_humidity = (6.112 * pow(2.71828, (17.67 * temperature)/ 
  (temperature + 243.5)) * rel_humidity * 2.1674) / (273.15 + temperature);
  double gamma = log(rel_humidity/100.0) + (18.678 * temperature) / (257.14 + temperature);
  double dew_point = (257.14 * gamma) / (18.678 - gamma);
  
  //Wyslanie danych jesli siec jest polaczona
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    //Nawiazanie polaczenia
    http.begin(serverName);
    
    // przygotowanie request HTTP POST
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                          +  "&value1=" + String(abs_humidity,3)
                          + "&value2=" + String(dew_point, 3) + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    //Wyslanie HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    if (httpResponseCode>0) {
      Serial.print("Data successfully posted to HTTP");
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Zakonczenie polaczenia
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  delay(3000);  
}
