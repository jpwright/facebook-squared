//Jeremy Blum & Jason Wright
//Facebook NYC Summer Hack-a-Thon 7/21/2012

#define DEBUG Serial
#define XBEE Serial1

void setup()
{
  //Setup interface to the Adafruit Visual Display
  //http://www.adafruit.com/products/338
  
  //Serial interface to Computer Via USB (for debugging only)
  DEBUG.begin(9600);
  
  //Serial interface to XBEE for Data transmit and receive
  XBEE.begin(9600);
  
  //Setup interface to the multiple Parallel LCD interfaces
}
void loop()
{
  //Serial Communication Debug
  //DEBUG.println("Test to the Debug");
  //XBEE.println("Test via XBEE");
}
