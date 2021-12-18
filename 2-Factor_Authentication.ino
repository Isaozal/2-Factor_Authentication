#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const int buzzp =  A1;
const int buzzn =  A2;
const String authorized_card = "C4D6B33";               // this is the ID of my card. You can get your ID, when you print out your "cardId" in "loop" method 
const byte row = 4;
const byte col = 3;
char buttons[row][col] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

char key_code[4];
char password[4] = {'4', '3','2','1'};
unsigned int k=0;
int i=0;

byte row_pins[row] = {8, 7, 6, 5};
byte col_pins[col] = {4, 3, 2};

LiquidCrystal_I2C lcd(0x27,16,2);
MFRC522 rfid_card(10, 9);
Keypad kpad = Keypad(makeKeymap(buttons), row_pins, col_pins, row, col);
MFRC522::MIFARE_Key key; 

void setup() {
  
  SPI.begin();                                                          // initialize the SPI bus communication protocol
  rfid_card.PCD_Init();                                                 // initialize for the controlling mfrc522
  lcd.begin(16,2);
  lcd.backlight();
  pinMode(buzzp, OUTPUT);
  pinMode(buzzn, OUTPUT);
}

void loop() {

  lcd.setCursor(0,0); lcd.print("Welcome!");
  lcd.setCursor(0,1); lcd.print("Show your card.");

  if(!rfid_card.PICC_IsNewCardPresent()) { return; }                   // if there is no new card, turn back to the begin 
  if(!rfid_card.PICC_ReadCardSerial()) { return; }                     // for example when you try 2 cards at the same time 

  lcd.clear(); lcd.setCursor(0,0); lcd.print("User ID:");
  String cardId = "";

  for(byte i = 0; i < rfid_card.uid.size; i++)                          // for our card the size is 4
  {
    cardId.concat(String(rfid_card.uid.uidByte[i], HEX));               // add every element of uidByte into the "cardId" in hexadecimal form and convert it to the string
  }

  cardId.toUpperCase();
  lcd.setCursor(0,1); lcd.print(cardId); 

  if(cardId == authorized_card) { authorizedAccess(); }
  else { nonAuthorizedAccess(); }
}

// this method runs, if the user hold an authenticated card to the sensor
void authorizedAccess()
{
  int check = 0; int star = 10;
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Authorized Access!");
  lcd.setCursor(0,1); lcd.print("Password:");
  displayBlink();
  
  while(check == 0) {
    
    char key = kpad.getKey();                               // get input from keypad
    
    if(key != NO_KEY){
     lcd.setCursor(star,1); lcd.print("*");                 // give a star(*) for each input on keypad
     key_code[i++]=key; k=i; star++;
    }

     if(k == sizeof(password)){
      if(!strncmp(password,key_code, sizeof(password))){   
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("Correct Password!");
        lcd.setCursor(0,1); lcd.print("Welcome!");
        buzz();
        i=k=0; check = 1;
        homeScreen();
      }
      else {    
        i=k=0; check = 1;
        lcd.clear(); lcd.setCursor(0,0); lcd.print("Wrong Password!");
        warn();
        homeScreen(); 
      }
     }
  }
}

// this method runs, if the user hold an unauthenticated card to the sensor
void nonAuthorizedAccess()
{
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Access Denied!");
  lcd.setCursor(0,1); lcd.print("Update your card");
  warn();
  homeScreen();
}

// helper method for returning to homescreen
void homeScreen() {
  delay(3000); lcd.clear();
  lcd.setCursor(0,0); lcd.print("Welcome!");
  lcd.setCursor(0,1); lcd.print("Show your card."); 
}

// helper method for visual and auditorial warning for wrong input
void warn() {
  for(int i=0; i < 3; i++){
    lcd.noDisplay(); digitalWrite(buzzp, HIGH);digitalWrite(buzzn, LOW); delay(250);
    lcd.display(); digitalWrite(buzzp, LOW);digitalWrite(buzzn, LOW); delay(700);
  }   
}

// helper method for auditorial support for right password
void buzz(){
  
    for(int i=0; i < 2; i++){
      digitalWrite(buzzp, HIGH);digitalWrite(buzzn, LOW); delay(100);
      digitalWrite(buzzp, LOW);digitalWrite(buzzn, LOW); delay(100);
  } 
}

// helper method to make screen transitions easier for user
void displayBlink(){
  lcd.noDisplay(); delay(500); lcd.display();
}
