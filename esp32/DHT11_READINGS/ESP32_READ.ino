#include "BluetoothSerial.h"
#include "DHT.h"
#include "math.h"

BluetoothSerial SerialBT;
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
  SerialBT.begin("ESP32test"); //Bluetooth device name
}

void loop() {
  //pobranie wartosci odczytow z czujnika
  float  rel_humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  //obliczenie wartosci wilgotnosci bezwzglednej i punktu rosy
  float abs_humidity = (6.112 * pow(2.71828, (17.67 * temperature)/ (temperature + 243.5)) * rel_humidity * 2.1674) / (273.15 + temperature);
  double gamma = log(rel_humidity/100.0) + (18.678 * temperature) / (257.14 + temperature);
  double dew_point = (257.14 * gamma) / (18.678 - gamma);
  //jesli polaczenie jest nawiazane, to wyslij dane z czujnikow
  if(SerialBT.hasClient()){
    String array = String(abs_humidity, 3) + " " + String(dew_point, 3);
    writeString(array);
    Serial.println(array);
  }
  delay(2000);
}

//Funkcja do wypisania ciagu tekstowego znak po znaku
void writeString(String stringData) { 

  for (int i = 0; i < stringData.length(); i++)
  {
    SerialBT.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
  }
  SerialBT.write(13);

}
