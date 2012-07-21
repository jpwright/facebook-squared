//Jeremy Blum & Jason Wright
//Facebook NYC Summer Hack-a-Thon 7/21/2012

//Definitions
#define DEBUG Serial
#define XBEE Serial1

/* 
  LCD
  The circuit:
 * LCD RS pin to digital pin 27
 * LCD EN pin to digital pin 26
 * LCD D4 pin to digital pin 25
 * LCD D5 pin to digital pin 24
 * LCD D6 pin to digital pin 23
 * LCD D7 pin to digital pin 22
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 */
#include <LiquidCrystal.h>
LiquidCrystal lcd(27, 26, 25, 24, 23, 22);

void setup()
{
  //Setup interface to the Adafruit Visual Display
  //http://www.adafruit.com/products/338
  
  //Serial interface to Computer Via USB (for debugging only)
  DEBUG.begin(9600);
  
  //Serial interface to XBEE for Data transmit and receive
  XBEE.begin(9600);
  
  //Setup interface to the multiple Parallel LCD interfaces
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Jeremy's Display");
  lcd.setCursor(0,1);
  lcd.print("The PhysiBook");
}
void loop()
{
  //Serial Communication Debug
  //DEBUG.println("Test to the Debug");
  //XBEE.println("Test via XBEE");
}
