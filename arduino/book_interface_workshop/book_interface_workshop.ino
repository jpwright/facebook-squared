//Jeremy Blum & Jason Wright
//Facebook NYC Summer Hack-a-Thon 7/21-22/2012

//Libraries
#include <LiquidCrystal.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

//LCD Hookups
#define  RS     8
#define  EN     9
#define  D4     10
#define  D5     11
#define  D6     12
#define  D7     13

//Graphical LCD
#define GLCD_SCLK  7
#define GLCD_DIN   6
#define GLCD_DC    5
#define GLCD_CS    4
#define GLCD_RST   3
Adafruit_PCD8544 display = Adafruit_PCD8544(GLCD_SCLK, GLCD_DIN, GLCD_DC, GLCD_CS, GLCD_RST);
int display_width = 84;
int display_height = 48;

//Control Buttons
#define HOMEBUTTON  40
#define NEXTBUTTON  41

//LCD Setup
LiquidCrystal lcdtop(RS, EN, D4, D5, D6, D7);

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
byte squared[8] = {
 B11000,
 B00100,
 B01000,
 B10000,
 B11100,
 B00000,
 B00000,
};
byte link[8] = {
 B00000,
 B00100,
 B00010,
 B11111,
 B00010,
 B00100,
 B00000,
};
byte desc[8] = {
 B00000,
 B01000,
 B01100,
 B01110,
 B01100,
 B01000,
 B00000,
};
byte ellipsis[8] = {
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B10101,
};

//Special Character location temp variables
int likeIndex = -1;
int commentIndex = -1;
int placeIndex = -1;
int linkIndex = -1;
int descIndex = -1;
int ellipsisIndex = -1;

boolean lastNextButton = LOW;
boolean currentNextButton = LOW;
boolean lastHomeButton = LOW;
boolean currentHomeButton = LOW;

void setup()
{
  //Setup interface to the Adafruit Visual Display
  //http://www.adafruit.com/products/338
  
  //Serial interface to XBEE for Data transmit and receive
  Serial.begin(57600);

  
  //Setup Graphical LCD
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  
  //Setup interface to the multiple Parallel LCD interfaces
  lcdtop.begin(16, 2);
  //Serial.println("Program Start");
  
  //Setup LCD Special Characters for all necessary LCDs
  lcdtop.createChar(0, like);
  lcdtop.createChar(1, comment);
  lcdtop.createChar(2, place);
  lcdtop.createChar(3, squared);
  lcdtop.createChar(4, link);
  lcdtop.createChar(5, desc);
  lcdtop.createChar(6, ellipsis);

}
void loop()
{ 
  //Check to see if the Book was just opened.  If so, clear all and Display Welcome Messages.
  //If the Cover is open...
    Serial.println(".h"); //Tell python we're starting up fresh.
    lcdtop.clear();
    lcdtop.setCursor(0,0);
    lcdtop.print("Welcome to");
    lcdtop.setCursor(0,1);
    lcdtop.print("face(book)");
    lcdtop.setCursor(10,1);
    lcdtop.write(3);
  
  /*********************************
  * BUTTON CHECKING W/ DEBOUNCING 
  **********************************/
  //Home Button
  currentHomeButton = debounce(lastHomeButton, HOMEBUTTON);
  if (lastHomeButton == LOW && currentHomeButton == HIGH)
  {
    Serial.println(".f");
  }
  lastHomeButton = currentHomeButton;
  
  
  currentNextButton = debounce(lastNextButton, NEXTBUTTON);
  if (lastNextButton == LOW && currentNextButton == HIGH)
  {
      Serial.println(".n");
      //DEBUG.println("Next Entry Requested");
  }
  
  /*********************************
  * SERIAL COMMUNICATION PROTOCOL
  **********************************/
  if (Serial.available())
  {
    //Wait until we have received a '.', indicating the start of a command
    while (Serial.read() != '.'); //Throw away any data before the '.'
    while (Serial.available() == 0); //Wait till we've got some data
    //Read in the command byte
    int val = Serial.read();
    
    //Hello Response
    if (val == 'h')
    {
      String text = readString();
      lcdtop.setCursor(0,0);
      lcdtop.print("Connected as:");
      lcdtop.setCursor(0,1);
      lcdtop.print(text.substring(0,16)); // we should only print up to 16 chars max.
      //DEBUG.println("Connection Established");
      Serial.println(".zh"); //Success
      delay(1000);
      //We're live!  Ask for news.
      Serial.println(".f");
    }
    
    //Write to LCD
    if (val == 'w')
    {
      while(!Serial.available());
      int display_id = Serial.read();
      while(Serial.read() != ',');
      while(!Serial.available());
      int line_num = Serial.read() - '0';
      while(Serial.read() != ',');
      String text = readString();
      //Write the info to the appropriate LCD Location

        lcdtop.setCursor(0,line_num);
        lcdtop.print("                ");
        LCDPrintWithSpecialChars(lcdtop, text, line_num);

      Serial.println(".zw"); //success
    }
    
    //Graphical LCD Control
    if (val == 'g')
    {
      display.clearDisplay();
      boolean done = false;
      String command = "";
      int x = 17;
      int y = 0;
      while(!done) {
        while(!Serial.available());
        int strchar = Serial.read();
        if(strchar == '\n') {
          done = true;
        } else {
           if(strchar == '1' && y < 49) {
             display.drawPixel(x, y, BLACK);
           }
           if(x<66) {
             x++;
           } else {
             x = 17;
             y++;
           }
        }
      }
      display.display();
      Serial.println(".zg"); //success
    }
  }
}

//Reads string in from Buffer.
//Note, they could be longer than 18 chars because we use special chars sequences for special graphics
//There will never be more than one special char per line.
String readString ()
{
  likeIndex = -1;
  commentIndex = -1;
  placeIndex = -1;
  linkIndex = -1;
  descIndex = -1;
  ellipsisIndex = -1;
  while(!Serial.available());
  boolean done = false;
  char buffer[16] = "              ";
  int pos = 0;
  boolean special_coming = false;
  //Read up to 18 chars, then stop
  while(!done && pos < 16) {
    while(!Serial.available());
    int strchar = Serial.read();
    if(strchar == '\n') {
      done = true;
    } else {
      if (strchar != '<' && strchar != '>')
      {
        buffer[pos] = strchar;
        pos++;
      }
      if (strchar == '<')
      {
        special_coming = true;
      }
      else if (special_coming)
      {
        special_coming = false;
        if (strchar == 'l') likeIndex = pos-1;
        if (strchar == 'c') commentIndex = pos-1;
        if (strchar == 'p') placeIndex = pos-1;
        if (strchar == 'u') linkIndex = pos-1;
        if (strchar == 'e') ellipsisIndex = pos-1;
      }
    }
  }
  if (!done) while(Serial.read() != '\n');
  String text = buffer;
  return text;
}

void LCDPrintWithSpecialChars(LiquidCrystal LCD, String text, int line_num)
{
  LCD.setCursor(0,line_num);
  text = text.substring(0,16);
  //like
  if (likeIndex != -1)
  {
    LCD.setCursor(likeIndex,line_num);
    LCD.write((byte)0x0);
    LCD.setCursor(0,line_num);
    LCD.print(text.substring(0,likeIndex));
    LCD.setCursor(likeIndex+1,line_num);
    LCD.print(text.substring(likeIndex+1));
  }
  else if (commentIndex != -1)
  {
    LCD.setCursor(commentIndex,line_num);
    LCD.write(1);
    LCD.setCursor(0,line_num);
    LCD.print(text.substring(0,commentIndex));
    LCD.setCursor(commentIndex+1,line_num);
    LCD.print(text.substring(commentIndex+1));
  }
  else if(placeIndex != -1)
  {
    LCD.setCursor(placeIndex,line_num);
    LCD.write(2);
    LCD.setCursor(0,line_num);
    LCD.print(text.substring(0,placeIndex));
    LCD.setCursor(placeIndex+1,line_num);
    LCD.print(text.substring(placeIndex+1));
  }
  else if(linkIndex != -1)
  {
    LCD.setCursor(linkIndex,line_num);
    LCD.write(4);
    LCD.setCursor(0,line_num);
    LCD.print(text.substring(0,linkIndex));
    LCD.setCursor(linkIndex+1,line_num);
    LCD.print(text.substring(linkIndex+1));
  }
  else if(ellipsisIndex != -1)
  {
    LCD.setCursor(ellipsisIndex,line_num);
    LCD.write(6);
    LCD.setCursor(0,line_num);
    LCD.print(text.substring(0,ellipsisIndex));
    LCD.setCursor(ellipsisIndex+1,line_num);
    LCD.print(text.substring(ellipsisIndex+1));
  }
  else
  {
    LCD.print(text);
  }
}

boolean debounce(boolean last, int buttonPin)
{
  boolean current = digitalRead(buttonPin);
  if (last != current)
  {
    delay(5);
    current = digitalRead(buttonPin);
  }
  return current;
}

