#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

const int pinReleu = 2;//Digital D2
const int pinButon = 8;int stareButon = 0;
const int pinBuzzer = 15;//Analog A1
const int pinLedAlbastru = 16;//Analog A2
const int pinLedRosu = 17;//Analog A3

LiquidCrystal_I2C lcd(0x27,16,2);

const int pin_rst = 9; const int pin_ss = 10; MFRC522 mfrc522(pin_ss, pin_rst);
String cartela = "DC C1 28 83";String card = "A3 7F 3B 1A";

// FPS (TX) is connected to pin 4 (Arduino's Software RX)
// FPS (RX) is connected through a converter to pin 5 (Arduino's Software TX)
const int deget1 = 0;const int deget2 = 1;
FPS_GT511C3 fps(4, 5); // (Arduino SS_RX = pin 4, Arduino SS_TX = pin 5)

void setup() {// put your setup code here, to run once:
  fps.Open();         //send serial command to initialize fps
  SPI.begin();
  mfrc522.PCD_Init();//Initializare MFRC522
  lcd.begin();
  lcd.backlight();
  digitalWrite(pinReleu, HIGH);
  pinMode(pinButon, INPUT);
  pinMode(pinReleu, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinLedAlbastru, OUTPUT);
  pinMode(pinLedRosu, OUTPUT);

  
  lcd.print("Initializare");
  tone(pinBuzzer, 1600);
  digitalWrite(pinLedAlbastru, HIGH);
  digitalWrite(pinLedRosu, HIGH);
  delay(600);
  noTone(pinBuzzer);
  delay(250);
  digitalWrite(pinLedAlbastru, LOW);
  lcd.clear();
 
}
void loop() {// put your main code here, to run repeatedly:
  int id;
  stareButon = digitalRead(pinButon);
  digitalWrite(pinLedRosu, HIGH);
  lcd.print("Apropiati       ");
  lcd.setCursor(2, 1);
  lcd.print("  cartela       ");

  if( stareButon == LOW){
    digitalWrite(pinLedRosu, LOW);
    digitalWrite(pinReleu, LOW);
    tone(pinBuzzer, 4000);
    delay(200);
    noTone(pinBuzzer);
    digitalWrite(pinLedAlbastru, HIGH);
    delay(1500);
    digitalWrite(pinLedAlbastru, LOW);
    digitalWrite(pinReleu, HIGH);
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) { // Look for new cards
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) { // Select one of the cards
    return;
  }

  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++){
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();


  if(content.substring(1) == cartela || content.substring(1) == card){

    fps.SetLED(true);   //turn on LED so fps can see fingerprint
    lcd.clear();
    tone(pinBuzzer, 3000);
    delay(150);
    noTone(pinBuzzer);
    lcd.print("Scanati degetul");
    lcd.setCursor(2,1);
    lcd.print("Tineti apasat");
    delay(3000);

    if (fps.IsPressFinger()){
        fps.CaptureFinger(false);
        id = fps.Identify1_N();
    }else{
      lcd.clear();
      lcd.print("Sesiunea");
      lcd.setCursor(2, 1);
      lcd.print("  a expirat");
      delay(1500);
      lcd.clear();
      fps.SetLED(false);   //turn off LED so fps can't see fingerprint
    return;
    }
    if ( id == deget1  || id == deget2 ) {

      lcd.clear(); 
      lcd.print("Acces permis ");
      lcd.setCursor(2,1);
      lcd.print(" ID: ");
      lcd.print(id);
      digitalWrite(pinLedRosu, LOW);
      digitalWrite(pinReleu, LOW);
      tone(pinBuzzer, 4000);
      digitalWrite(pinLedAlbastru, HIGH);
      delay(1500);
      lcd.clear();
      noTone(pinBuzzer);
      digitalWrite(pinLedAlbastru, LOW);
      digitalWrite(pinReleu, HIGH);
      fps.SetLED(false);   //turn off LED so fps can't see fingerprint
  }else if ( id != deget1 || id != deget2){
    lcd.clear();
    lcd.print("Acces interzis");
    lcd.setCursor(2,1);
    lcd.print("  ID Invalid");
    digitalWrite(pinLedRosu, HIGH);
    digitalWrite(pinLedAlbastru, LOW);
    tone(pinBuzzer, 500);
    digitalWrite(pinReleu, HIGH);
    delay(1000);
    lcd.clear();
    noTone(pinBuzzer);
    digitalWrite(pinLedRosu, LOW);
    fps.SetLED(false);   //turn off LED so fps can't see fingerprint
  }
    
  }
  else if (content.substring(1) != cartela || content.substring(1) != card){
    lcd.clear();
    lcd.print("Acces interzis");
    lcd.setCursor(2,1);
    lcd.print(" Tag Invalid");
    digitalWrite(pinLedRosu, HIGH);
    digitalWrite(pinLedAlbastru, LOW);
    tone(pinBuzzer, 500);
    digitalWrite(pinReleu, HIGH);
    delay(1000);
    lcd.clear();
    noTone(pinBuzzer);
    digitalWrite(pinLedRosu, LOW);
    fps.SetLED(false);   //turn off LED so fps can't see fingerprint
  }
}
