//stałe do sterowania pinami IO
const int PIR = 23;
const int LED_RED = 22;
const int LED_GREEN = 21;
void setup() {
  //deklaracja wejść i wyjść
  pinMode(PIR, INPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  //rozpoczęcie transmisji szeregowej
  Serial.begin(9600);
  Serial.println("START");
}

void loop() {
  
  if (digitalRead(PIR) == HIGH){
    //JESLI STAN WYSOKI, TO WYKRYTO RUCH
    //ZAPALENIE CZERWONEJ DIODY
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    Serial.println("WYKRYTO RUCH");
    delay(10 * 1000); // czekanie 10 s
  } else {
    //ZAPALENIE ZIELONEJ DIODY
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }
}
