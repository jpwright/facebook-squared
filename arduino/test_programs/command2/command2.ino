#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
int display_width = 84;
int display_height = 48;

void setup() {
  Serial.begin(57600);
  display.begin();
  display.setContrast(50);
  display.clearDisplay();   // clears the screen and buffer
  Serial.write("f\n");
  delay(1000);
  Serial.write("e0\n");
  delay(1000);
}

void loop() {
  //Wait until we have received a '.', indicating the start of a command
  while (Serial.read() != '.'); //Throw away any data before the '.'
  while (Serial.available() == 0); //Wait till we've got some data
  
  //Read in the command byte
  int val = Serial.read();
  
  //Printing to LCD
  if (val == 'p')
  {
    while(!Serial.available());
    int line_num = Serial.read();
    while(!Serial.available());
    boolean done = false;
    String command = "";
    int pos = 0;
    //Read 16 chars, then stop
    while(!done) {
      while(!Serial.available());
      int strchar = Serial.read();
      if(strchar == '\n') {
        done = true;
      } else {
         command[pos] = strchar;
         pos++;
      }
    }
    Serial.print('s'); //success
  }
  
  if (val == 'i')
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
  }
  
}
