//Jeremy Blum & Jason Wright
//Facebook NYC Summer Hack-a-Thon 7/21/2012

//Serial Definitions
#define DEBUG Serial
#define XBEE Serial1

//LEDs
#define DEBUGLED  13

//LCD Hookups
#define  RS     27
#define  ENTOP  26
#define  EN1    50
#define  EN2    51
#define  EN3    52
#define  D4     25
#define  D5     24
#define  D6     23
#define  D7     22

//Selection Buttons
#define BUTTON1  42
#define BUTTON2  43
#define BUTTON3  44

//Control Buttons
#define HOMEBUTTON  45
#define LIKEBUTTON  46

//Cover Detect
#define COVERDETECT  47

//Debounce Tracking Variables
boolean lastButton1 = LOW;
boolean currentButton1 = LOW;
boolean lastButton2 = LOW;
boolean currentButton2 = LOW;
boolean lastButton3 = LOW;
boolean currentButton3 = LOW;
boolean lastHomeButton = LOW;
boolean currentHomeButton = LOW;
boolean lastLikeButton = LOW;
boolean currentLikeButton = LOW;
boolean bookOpen = false;

//LCD Setup
#include <LiquidCrystal.h>
LiquidCrystal lcdtop(RS, ENTOP, D4, D5, D6, D7);
LiquidCrystal lcd1(RS, EN1, D4, D5, D6, D7);
LiquidCrystal lcd2(RS, EN2, D4, D5, D6, D7);
LiquidCrystal lcd3(RS, EN3, D4, D5, D6, D7);

//LCD Special Characters
byte like[8] = {
 B00000,
 B00100,
 B00100,
 B11011,
 B11001,
 B11001,
 B11111,
};

byte comment[8] = {
 B11111,
 B10001,
 B10001,
 B10001,
 B11111,
 B01100,
 B01000,
};


byte place[8] = {
 B01110,
 B10001,
 B10101,
 B10001,
 B01110,
 B01110,
 B00100,
};

void setup()
{
  //Setup interface to the Adafruit Visual Display
  //http://www.adafruit.com/products/338
  
  //Serial interface to Computer Via USB (for debugging only)
  DEBUG.begin(9600);
  
  //Serial interface to XBEE for Data transmit and receive
  XBEE.begin(9600);
  
  //LED Setup
  pinMode(DEBUGLED, OUTPUT);
  
  //Setup interface to the multiple Parallel LCD interfaces
  lcdtop.begin(16, 2);
  lcd1.begin(16, 4);
  lcd2.begin(16, 4);
  lcd3.begin(16, 4);
  DEBUG.println("Program Start");
}
void loop()
{ 
  //Check to see if the Book was just opened.  If so, clear all and Display Welcome Messages.
  //If the Cover is open...
  if(digitalRead(COVERDETECT) && !bookOpen)
  {
    digitalWrite(DEBUGLED, HIGH);
    bookOpen = true;
    DEBUG.println("The Book has been Opened");
    XBEE.println(".h"); //Tell python we're starting up fresh.
    lcdtop.clear();
    lcd1.clear();
    lcd2.clear();
    lcd3.clear();
    lcdtop.setCursor(0,0);
    lcd1.setCursor(0,0);
    lcd2.setCursor(0,0);
    lcd3.setCursor(0,0);
    lcdtop.print("This is faceBOOK");
    lcdtop.setCursor(0,1);
    lcdtop.print("Jeremy");
    delay(2000);
  }
  //Book has been Closed
  else if(!digitalRead(COVERDETECT) && bookOpen)
  {
    digitalWrite(DEBUGLED, LOW);
    bookOpen = false;
    DEBUG.println("The Book has been Closed");
    XBEE.println(".c"); //Tell python we've closed the book.
    lcdtop.clear();
    lcd1.clear();
    lcd2.clear();
    lcd3.clear();
    lcdtop.setCursor(0,0);
    lcdtop.print("Peace Out.");
  }
  
  
  
}
