#include <TinyGPS.h> 
#include <SoftwareSerial.h> 
#include <Wire.h> 
 
SoftwareSerial Gsm(3, 2); 
char phone_no[] = "+94778989570"; 
 
TinyGPS gps; 
int state; 
String textMessage; 
float vout; 
boolean countStatus; 
int beat, bpm; 
unsigned long millisBefore; 
bool buttonPressed = false; 
 
void setup()  
{ 
  Serial.begin(9600); 
  Gsm.begin(9600); 
  delay(2000); 
  Serial.print("AT+CMGF=1\r"); 
  delay(100); 
  Serial.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100); 
  pinMode(4, INPUT); 
  pinMode(A3, OUTPUT); 
  digitalWrite(A3, LOW);  
} 
 
void loop() 
{ 
  if (Gsm.available() > 0) { 
    textMessage = Gsm.readString(); 
    textMessage.toUpperCase(); 
    delay(10); 
  } 
   
  int sensorValue = analogRead(A0); 
  if (countStatus == 0) { 
    if (sensorValue > 516) { 
28 
 
      countStatus = 1; 
      beat++; 
      Serial.println("Beat Detected!"); 
      Serial.print("Beat : "); 
      Serial.println(beat); 
    } 
  } else { 
    if (sensorValue < 513) { 
      countStatus = 0; 
    } 
  } 
   
  if (millis() - millisBefore > 15000) { 
    bpm = beat * 4; 
    beat = 0; 
    Serial.print("BPM : "); 
    Serial.println(bpm); 
    millisBefore = millis(); 
  } 
  delay(1);   
 
  if (bpm <= 100 && digitalRead(4) == HIGH) { 
    sendAlert(); 
  } 
   
  if (bpm > 100) { 
    for (int i = 0; i < 30; i++) { 
      digitalWrite(A3, HIGH); 
      delay(250); 
      digitalWrite(A3, LOW); 
      delay(250); 
       
   
      if (digitalRead(4) == HIGH) { 
        buttonPressed = true; 
        break;  
      } 
    } 
     
    if (!buttonPressed) { 
      sendAlertHeart(); 
    } 
  } else { 
    buttonPressed = false; 
29 
 
  } 
} 
 
void sendAlert() { 
  getGps(); 
  Serial.println("Sending Alert..."); 
  float flat, flon; 
  unsigned long age; 
  gps.f_get_position(&flat, &flon, &age); 
   
  digitalWrite(A3, HIGH); 
   
  Gsm.print("AT+CMGF=1\r"); 
  delay(400); 
  Gsm.print("AT+CMGS=\""); 
  Gsm.print(phone_no); 
  Gsm.println("\""); 
 
  Gsm.println("Alert: I need help"); 
  Gsm.print("http://maps.google.com/maps?q=loc:"); 
  Gsm.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6); 
  Gsm.print(","); 
  Gsm.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6); 
  delay(200); 
  Gsm.println((char)26); 
  delay(200); 
  Gsm.println(); 
  Serial.println("SMS Sent"); 
   
  Serial.println("Making Call..."); 
  delay(20000); 
  Gsm.println("ATD+94778869570;"); 
  delay(60000); 
  Gsm.println("ATH"); 
  delay(1000); 
   
  digitalWrite(A3, LOW);  
} 
void sendAlertHeart() { 
  getGps(); 
  Serial.println("Sending Alert..."); 
  float flat, flon; 
  unsigned long age; 
  gps.f_get_position(&flat, &flon, &age); 
30 
 
   
  digitalWrite(A3, HIGH); 
   
  Gsm.print("AT+CMGF=1\r"); 
  delay(400); 
  Gsm.print("AT+CMGS=\""); 
  Gsm.print(phone_no); 
  Gsm.println("\""); 
 
  Gsm.println("Alert: Heartbeat exceeded 100 BPM.Help needed."); 
  Gsm.print("http://maps.google.com/maps?q=loc:"); 
  Gsm.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6); 
  Gsm.print(","); 
  Gsm.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6); 
  delay(200); 
  Gsm.println((char)26); 
  delay(200); 
  Gsm.println(); 
  Serial.println("SMS Sent"); 
   
  Serial.println("Making Call..."); 
  delay(20000); 
  Gsm.println("ATD+94778869570;"); 
  delay(60000); 
  Gsm.println("ATH"); 
  delay(1000); 
  digitalWrite(A3, LOW);  
} 
 
void getGps() 
{ 
  bool newData = false; 
  unsigned long chars; 
  unsigned short sentences, failed; 
  for (unsigned long start = millis(); millis() - start < 2000;) { 
    while (Serial.available()) { 
      char c = Serial.read(); 
      Serial.print(c); 
      if (gps.encode(c)) 
        newData = true; 
    } 
  } 
} 
