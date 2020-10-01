/*
 * 
 * All the resources for this project: https://randomnerdtutorials.com/
 * Modified by Rui Santos
 * 
 * Created by FILIPEFLOP
 * 
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

Servo myservo;

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#define SERVO_PIN 3

//the card UID which will rotate the servo
const char* ACCES_ENABLE_UID = "02 25 3E 34";

const int SERVO_STARTING_POSITION = 0; //degree
const int SERVO_ROTATE_DEGREE = 90; //degree
const int SERVO_ROTATING_DELAY = 10; // rotate speed, the smaller the faster

//when servo was rotated and you take off the card:
//how match time you want to stay the servo befor rotating the starting position? 
//!! it can't be 0 becas the card reader sometime don't senses the card and the servo will go back to starig position !!
const int OPEN_DELAY = 3000; //ms


//if the servo rotated the given degree it is true, if the servo is in the staring prosison it is false
bool isServoRotated = false;

//timer
unsigned long delayStart = millis();

void setup() 
{
  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  myservo.attach(SERVO_PIN);
  myservo.write(SERVO_STARTING_POSITION);

}
void loop() 
{ 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {

  //if take off the card and elapsed time (OPEN_DELAY) the servo back to SERVO_STARTING_POSITION
  if(isServoRotated && ((millis() - delayStart) > OPEN_DELAY)){
    Serial.println("servo going back to the starting position");
    setServoPos(SERVO_STARTING_POSITION);
    isServoRotated = false;
  }
    return;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
     Serial.print("select card : ");
    return;
  }
  
  //Show UID on serial monitor
  String content = showUID();
  
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == ACCES_ENABLE_UID)
  {
    Serial.println("Authorized access");
    Serial.println();
    
    delayStart = millis();   // reset delay
    if(!isServoRotated){
      setServoPos(SERVO_ROTATE_DEGREE);  
    }

    delay(500);
  }
 
 else   {
    Serial.println(" Access denied");
    Serial.println("servo going back to the starting position");
    setServoPos(SERVO_STARTING_POSITION);
    isServoRotated = false;
    delay(500);
  }
} 

String showUID(){
    Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();

  return content;
}

void setServoPos(int degree){
  isServoRotated = true;
  Serial.print("rotate the servo degree: ");
  Serial.println(degree);
  int pos = SERVO_STARTING_POSITION;
  for (pos = SERVO_STARTING_POSITION; pos <= degree; pos += 1) {
    myservo.write(pos);
    delay(SERVO_ROTATING_DELAY);
  }
}
