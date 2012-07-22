//Jeremy Blum & Jason Wright
//Facebook NYC Summer Hack-a-Thon 7/21-22/2012

//Libraries
#include <LiquidCrystal.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

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

//Graphical LCD
#define GLCD_SCLK  34
#define GLCD_DIN   35
#define GLCD_DC    36
#define GLCD_CS    37
#define GLCD_RST   38
Adafruit_PCD8544 display = Adafruit_PCD8544(GLCD_SCLK, GLCD_DIN, GLCD_DC, GLCD_CS, GLCD_RST);
int display_width = 84;
int display_height = 48;

//Accelerometer Setup
#define ACCELY       15
#define YNEUTRAL     347
#define YDOWNTHRESH  315
#define YUPTHRESH    375
//-1 = previous
//+1 = next
//0  = neutral
int lastTurnState = 0;
unsigned long lastMillis = 0;

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

void setup()
{
  //Setup interface to the Adafruit Visual Display
  //http://www.adafruit.com/products/338
  
  //Serial interface to Computer Via USB (for debugging only)
  DEBUG.begin(57600);
  
  //Serial interface to XBEE for Data transmit and receive
  XBEE.begin(57600);
  
  //LED Setup
  pinMode(DEBUGLED, OUTPUT);
  
  //Setup Graphical LCD
  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  
  //Setup interface to the multiple Parallel LCD interfaces
  lcdtop.begin(16, 2);
  lcd1.begin(16, 4);
  lcd2.begin(16, 4);
  lcd3.begin(16, 4);
  DEBUG.println("Program Start");
  
  //Setup LCD Special Characters for all necessary LCDs
  lcdtop.createChar(0, like);
  lcd1.createChar(0, like);
  lcd2.createChar(0, like);
  lcd3.createChar(0, like);
  lcd1.createChar(1, comment);
  lcd2.createChar(1, comment);
  lcd3.createChar(1, comment);
  lcd1.createChar(2, place);
  lcd2.createChar(2, place);
  lcd3.createChar(2, place);
  lcdtop.createChar(3, squared);
  lcd1.createChar(4, link);
  lcd2.createChar(4, link);
  lcd3.createChar(4, link);
  lcd1.createChar(5, desc);
  lcd2.createChar(5, desc);
  lcd3.createChar(5, desc);
  lcd1.createChar(6, ellipsis);
  lcd2.createChar(6, ellipsis);
  lcd3.createChar(6, ellipsis);
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
    lcdtop.print("Welcome to");
    lcdtop.setCursor(0,1);
    lcdtop.print("face(book)");
    lcdtop.setCursor(10,1);
    lcdtop.write(3);
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
  
  /*********************************
  * BUTTON CHECKING W/ DEBOUNCING 
  **********************************/
  //Home Button
  currentHomeButton = debounce(lastHomeButton, HOMEBUTTON);
  if (lastHomeButton == LOW && currentHomeButton == HIGH)
  {
    XBEE.println(".f");
    DEBUG.println("Feed Requested");
  }
  lastHomeButton = currentHomeButton;
  
  //Like Button
  currentLikeButton = debounce(lastLikeButton, LIKEBUTTON);
  if (lastLikeButton == LOW && currentLikeButton == HIGH)
  {
    XBEE.println(".l");
    DEBUG.println("You liked this Story");
  }
  lastLikeButton = currentLikeButton;
  
  //Screen 1 Button
  currentButton1 = debounce(lastButton1, BUTTON1);
  if (lastButton1 == LOW && currentButton1 == HIGH)
  {
    XBEE.println(".e1");
    DEBUG.println("Story 1 Expansion Requested");
  }
  lastButton1 = currentButton1;
  
  //Screen 2 Button
  currentButton2 = debounce(lastButton2, BUTTON2);
  if (lastButton2 == LOW && currentButton2 == HIGH)
  {
    XBEE.println(".e2");
    DEBUG.println("Story 2 Expansion Requested");
  }
  lastButton2 = currentButton2;
  
  //Screen 3 Button
  currentButton3 = debounce(lastButton3, BUTTON3);
  if (lastButton3 == LOW && currentButton3 == HIGH)
  {
    XBEE.println(".e3");
    DEBUG.println("Story 3 Expansion Requested");
  }
  lastButton3 = currentButton3;
  
  /*********************************
  * PAGE TURN DETECTION
  **********************************/
  //We don't wanna run this constantly.  Use millis
  if (millis() > lastMillis + 500)
  {
    int currentYval = analogRead(ACCELY);
    if ((currentYval < YDOWNTHRESH) && lastTurnState == 0)
    {
      XBEE.println(".p");
      DEBUG.println("Previous Entry Requested");
      lastTurnState = -1;
    }
    else if ((currentYval > YUPTHRESH) && lastTurnState == 0)
    {
      XBEE.println(".n");
      DEBUG.println("Next Entry Requested");
      lastTurnState = 1;
    }
    else if ((currentYval <= YUPTHRESH) && (currentYval >= YDOWNTHRESH))
    {
      lastTurnState = 0;
    }
    lastMillis = millis();
  }
  
  /*********************************
  * SERIAL COMMUNICATION PROTOCOL
  **********************************/
  if (XBEE.available())
  {
    //Wait until we have received a '.', indicating the start of a command
    while (XBEE.read() != '.'); //Throw away any data before the '.'
    while (XBEE.available() == 0); //Wait till we've got some data
    //Read in the command byte
    int val = XBEE.read();
    
    //Hello Response
    if (val == 'h')
    {
      String text = readString();
      lcdtop.setCursor(0,0);
      lcdtop.print("Connected as:");
      lcdtop.setCursor(0,1);
      lcdtop.print(text.substring(0,16)); // we should only print up to 16 chars max.
      DEBUG.println("Connection Established");
      XBEE.println(".zh"); //Success
      delay(1000);
      //We're live!  Ask for news.
      XBEE.println(".f");
    }
    
    //Write to LCD
    if (val == 'w')
    {
      while(!XBEE.available());
      int display_id = XBEE.read();
      while(XBEE.read() != ',');
      while(!XBEE.available());
      int line_num = XBEE.read() - '0';
      while(XBEE.read() != ',');
      String text = readString();
      //Write the info to the appropriate LCD Location
      if (display_id == 't')
      {
        lcdtop.setCursor(0,line_num);
        lcdtop.print("                ");
        LCDPrintWithSpecialChars(lcdtop, text, line_num);
      }
      else if (display_id == '1')
      {
        lcd1.setCursor(0,line_num);
        lcd1.print("                ");
        LCDPrintWithSpecialChars(lcd1, text, line_num);
      }
      else if (display_id == '2')
      {
        lcd2.setCursor(0,line_num);
        lcd2.print("                ");
        LCDPrintWithSpecialChars(lcd2, text, line_num);
      }
      else if (display_id == '3')
      {
        lcd3.setCursor(0,line_num);
        lcd3.print("                ");
        LCDPrintWithSpecialChars(lcd3, text, line_num);
      }
      DEBUG.print("\"");
      DEBUG.print(text);
      DEBUG.print("\" Printed to Display #");
      DEBUG.write(display_id);
      DEBUG.print(" on line ");
      DEBUG.println(line_num);
      XBEE.println(".zw"); //success
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
        while(!XBEE.available());
        int strchar = XBEE.read();
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
      XBEE.println(".zg"); //success
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
  while(!XBEE.available());
  boolean done = false;
  char buffer[16] = "              ";
  int pos = 0;
  boolean special_coming = false;
  //Read up to 18 chars, then stop
  while(!done && pos < 16) {
    while(!XBEE.available());
    int strchar = XBEE.read();
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
  if (!done) while(XBEE.read() != '\n');
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

