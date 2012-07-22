Facebook Summer of Hack NYC 2012
================================
* Jeremy Blum (https://www.github.com/sciguy14)
* Jason Wright (https://github.com/jpwright)

Serial Interface Documentation
------------------------------
* General Syntax
  * All commands start with a '.' character
  * All commands end with a newline '\n' character
  * first character indicates the command
  * followed by comma seperated arguments
  * Computer can only repond to requests from the hardware
  * Arduino responds on executions completed with 'z' ACK character followed the same command char plus a newline
* Special Characters, there is never more than one on a line
  * 0 <l> is like
  * 1 <c> is comment
  * 2 <p> is place
  * 3 <s> is squared
  * 4 <u> is link
  * 5 <d> is desc [DEPRECATED]
  * 6 <e> is ellipsis (...)
* From the Hardware
  * Hello 									= .h\n
  * Request Feed 							= .f\n
  * Expand Item								= .e[feed # to expand]\n 
  * Next Item (contextual)					= .n\n
  * Previous Item (contextual)				= .p\n
  * "Like" Item (contextual)				= .l\n
  * Request Graphical Display Contents  	= .g\n
  * Close									= .c\n
* From the Computer
  * Hello ID								= .h[String containing User First Name]\n
  * Return Feed Contents					= .f[string for display 1],[string for display 2],[etc]...\n
  * Return Expanded View Contents			= .e[string for display 1],[string for display 2],[etc]...\n
  * Return Graphical Display Binary Data	= .g[binary data string representing all pixels]\n
  * Write a line to a display			 	= .w[display ID char (t,1,2,3)],[display line # char (0,1,2,3),[String of Data]\n
  
 External Resouces
 -----------------
 Adafruit Graphical LCD Arduino Library:  
 * https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library
 * https://github.com/adafruit/Adafruit-GFX-Library
 
 LiquidCrystal Arduino Library Mod
 ---------------------------------
 There is known issue with addressing 16x4 LCDS
 See Here: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1253367247/6
 This fix was applied
