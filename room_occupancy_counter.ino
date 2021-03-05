
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h> // blynk
#include <ESP8266WiFi.h> // wifi

char blynk_auth[] = "RSRhlehn1wxfcHbcp9HoIpVuM-EjMTEG";
char wifi_ssid[] = "admin";
char wifi_pass[] = "12345678";

WidgetLCD lcd(V1);

// declaring sensor in array
int sensor1[] = {D7, D8}; // {echoPin, trigPin}
int sensor2[] = {D2, D3}; // {echoPin, trigPin}
int buzzer = D5; // buzzer

int sensor1Initial;
int sensor2Initial;

long duration, distance;

int maxDistance = 30;
int maxPeople = 6;
int currentPeople, previousPeople = 0;

String sequence = "";
int timeoutCounter = 0;

// ************************************
void setup() {
  Serial.begin(9600);
  Blynk.begin(blynk_auth, wifi_ssid, wifi_pass);

  pinMode(buzzer, OUTPUT);

  // giving initial value
  sensor1Initial = maxDistance;
  sensor2Initial = maxDistance;

  lcd.clear();
  lcd.print(0, 0, "People in a room:");
}

// ************************************
void loop() {

  int sensor1Val = measureDistance(sensor1);
  int sensor2Val = measureDistance(sensor2);

  previousPeople = currentPeople;

  // process the data.
  if (sensor1Val < sensor1Initial && sequence.charAt(0) != '1') {
    // return sequence 12. If sensor1 is detected first and sensor2 is detected second.
    sequence += "1";
    Serial.print("Sequence: ");
    Serial.println(sequence);
  } else if (sensor2Val < sensor2Initial && sequence.charAt(0) != '2') {
    // return sequence 21. If sensor2 is detected first and sensor1 is detected second.
    sequence += "2";
    Serial.print("Sequence: ");
    Serial.println(sequence);
    Serial.println(sensor2Val);
  }

  if (sequence.equals("12") && currentPeople > 0) {
    // count out going people from romm and subtract the no of people in room.
    currentPeople--;
    sequence = "";
    delay(550);
  }
  else if (sequence.equals("21") ) {
    // count new entering people and add.
    currentPeople++;
    sequence = "";
    delay(550);
  }

  //Resets the sequence if it is invalid or timeouts
  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 200) {
    sequence = "";
  }

  if (sequence.length() == 1) {
    timeoutCounter++;
  } else {
    timeoutCounter = 0;
  }

  // shows the total no of people in room.
  if (previousPeople < currentPeople || previousPeople > currentPeople) {
    Serial.print("Total : ");
    Serial.println(currentPeople);
    lcd.print(2, 1, currentPeople); // to print the current people in blynk
  }

  
  if (currentPeople > maxPeople) {
    tone(buzzer, 1700);
  }
  else {
    noTone(buzzer);
  }
  
  Blynk.run();
}

// ************************************
// measure the distance and convert into cm
int measureDistance(int pin[]) {
  pinMode(pin[1], OUTPUT); // pin[1] is trigPin where pin[0] is echoPin
  pinMode(pin[0], INPUT); // pin[0] is echoPin

  digitalWrite(pin[1], LOW);
  delayMicroseconds(2);
  digitalWrite(pin[1], HIGH);
  delayMicroseconds(10);
  digitalWrite(pin[1], LOW);
  duration = pulseIn(pin[0], HIGH);
  distance = duration / 29 / 2;
  return distance;
}
