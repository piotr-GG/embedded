//Dodanie biblioteki do obslugi czujnika
#include "DHT.h"
#include <EEPROM.h>

//definicja ilosci bajtow do zapisu danych
#define EEPROM_SIZE 8


//Numer pinu czujnika
const int sensor = 23;
//Obiekt czujnika
DHT dht;

float max_abs_humidity;
float max_temperature;

void setup() {
  //konfiguracja pinow wejscia
  pinMode(sensor, INPUT);
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Wzgledna wilgotnosc, Bezwzgledna wilgotnosc, punkt rosy");
  //inicjalizacja czujnika
  dht.setup(sensor);
  //inicjalizacja zapisu do EEPROM 
  EEPROM.begin(EEPROM_SIZE);

  //wczytanie przechowywaniej wartosci max wilgotnosci i temperatury
  EEPROM.get(0, max_abs_humidity);
  EEPROM.get(sizeof(float), max_temperature);
  
//  max_abs_humidity = EEPROM.read(0) / 10.0;
//  max_temperature = EEPROM.read(4) / 10.0;

  Serial.print("Maksymalna zapisana wilgotnosc: ");
  Serial.println(max_abs_humidity);
  Serial.print("Maksymalna zapisana temperatura: ");
  Serial.println(max_temperature);
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

  //wydrukowanie wynikow na monitor szeregowy przy statusie OK czujnika
  if (dht.getStatusString() == "OK") {

    Serial.print(abs_humidity);
    Serial.print(" g/m3 ");
    Serial.print(temperature);
    Serial.println("*C ");
    
    if(abs_humidity > max_abs_humidity){
      //zapis do pamieci EEPROM wartosci wilgotnosci
      max_abs_humidity = abs_humidity;
      EEPROM.put(0, max_abs_humidity);
      Serial.print("Nowa wartosc max wilgotnosci zapisana do EEPROM: ");
      Serial.println(max_abs_humidity);
      //zatwierdzenie zmian 
      EEPROM.commit();
    }
    if(temperature > max_temperature){
      //zapis do pamieci EEPROM wartosci temperatury
      max_temperature = temperature;
      EEPROM.put(sizeof(float), max_temperature);
      Serial.print("Nowa wartosc max temperatury zapisana do EEPROM: ");
      Serial.println(max_temperature);
      //zatwierdzenie zmian
      EEPROM.commit();
    }
  }
  delay(2000);
}
