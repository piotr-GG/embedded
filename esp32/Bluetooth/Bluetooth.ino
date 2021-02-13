//Załaczenie bibliotek do obsługi Bluetooth LE
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
//Dyrektywy preprocesora 
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
//Funkcja przerwania obslugujaca zdarzenie otrzymania danych przez Bluetooth
class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        String received = "";
        for (int i = 0; i < received.length(); i++){
          received = received + value[i];
        }

        Serial.println("*********");
        Serial.print("received = ");
        Serial.println(received);
        //obsluga wyjsc na podstawie przeslanych danych
        if(received == "RED"){
          digitalWrite(23,HIGH);
          digitalWrite(22,LOW);
          digitalWrite(21,LOW);
        }
        if(received == "GREEN"){
          digitalWrite(22,HIGH);
          digitalWrite(23,LOW);
          digitalWrite(21,LOW);
        }
        if(received == "BLUE"){
          digitalWrite(21,HIGH);
          digitalWrite(22,LOW);
          digitalWrite(23,LOW);
        }
      }
    }
};

void setup() {
  //Konfiguracja pinow wyjscia
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Rozpoczynanie transmisji!");
  //Inicjalizacja komunikacji przez Bluetooth
  BLEDevice::init("ESP32 RGB");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  //Przypisanie przerwania
  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("");
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMinPreferred(0x12);
  //Rozpoczenie rozglasszania sieci
  BLEDevice::startAdvertising();
}

void loop() {
  delay(2000);
}
