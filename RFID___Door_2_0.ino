//  WORKS FOR ARDUINO UNO  \\


#include <Servo.h> 
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//


MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance
MFRC522::MIFARE_Key key;

Servo myservo;  // create servo object to control a servo 

const int sensorPin = A0;
int greenLight = 4;
int redLight = 3;
int reedSwitch = 5; 
bool recentlyClosed = false;

void lockDoor(){
  delay(1000);
  myservo.attach(2);      //Pin 2 is used for the servo
  myservo.write(1);    //Correct degree --> 82
  
  digitalWrite(greenLight, LOW);
  digitalWrite(redLight, HIGH);
  Serial.println("Door Locked");
  delay(1000);
  myservo.detach();
}

void unlockDoor(){
  myservo.attach(2);      //Pin 2 is used for the servo
  myservo.write(179);    //Correct degree --> 132
  digitalWrite(greenLight, HIGH);
  digitalWrite(redLight, LOW);
  Serial.println("Door Unlocked");
  delay(1000);
  myservo.detach();  
}

boolean doorOpen(){
  if (digitalRead(reedSwitch) == LOW){
    Serial.println("Door is OPEN");
    return true;
  }
  return false;
}

boolean doorClosed(){
  if (digitalRead(reedSwitch) == HIGH){
    Serial.println("Door is CLOSED");
    return true;  
  }
  return false;
}

boolean sensorActivated(){
  int val = analogRead(sensorPin);
  if (val > 450){
    Serial.println("Sensor Activated");
    return true;
  }
  else
    return false;
}

void buzzCorrect(){
  analogWrite(6, 100);
  delay(500);
  analogWrite(6, 0);
}

void buzzIncorrect(){
  analogWrite(6, 1000);
  digitalWrite(7, HIGH);
  delay(500);
  analogWrite(6, 0);
  digitalWrite(7, LOW);
}

boolean verifyCorrectCard(){
  String correctCode1 = String("936b3413");      //Master Key(white card)
  String correctCode2 = String("1192e3b");      //Blue Keychain
  String correctCode3 = String("4e7c94a");      //Gray Keychain
  String correctCode4 = String("4ff5722");      //Blue Tag
  String correctCode5 = String("4839722");      //Red Tag
  String readCode = String("");
  for (byte i = 0; i < 4; i++) { // check only the first four bytes in the UID
    
    readCode += String(mfrc522.uid.uidByte[i], HEX);
      
  } 
  if (readCode == correctCode1 || readCode == correctCode2 || readCode == correctCode3 || readCode == correctCode4 || readCode == correctCode5){
    Serial.println("Access Granted");
    buzzCorrect();
    return true;
  }
  else{
    buzzIncorrect();
    return false;
  }
}




void setup(){
  
  Serial.begin(9600);		// Initialize serial communications with the PC
  while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  pinMode(greenLight, OUTPUT);
  pinMode(redLight, OUTPUT);  
  pinMode(reedSwitch, INPUT);
  pinMode(7, OUTPUT);
} 
 
 
void loop(){

  if ( doorClosed() ){
  
    if (recentlyClosed){
      delay(500);
      lockDoor();
      recentlyClosed = false;  
    } 
    digitalWrite(redLight, HIGH);
    
    if ( sensorActivated() ){ 
      unlockDoor();
      delay(5000);
      if ( doorClosed() ){
        lockDoor();
      }
    }
    //CHECK IF A CARD IS FLASHED / READ THE CARD
    if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
      delay (50);
      return;
    }
    //UNLOCK DOOR IF THE CORRECT CARD IS FLASHED
    if ( verifyCorrectCard() ){
      Serial.println("Card");
      unlockDoor();
      delay(5000);
      if ( doorClosed() ){
        delay(500);
        lockDoor();
      }
    }
  
  }
  
  if ( doorOpen() ){
    recentlyClosed = true;
  }

  
  
} 
