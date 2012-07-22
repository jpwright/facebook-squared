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
* From the Hardware
  * Hello 									= .h\n
  * Request Feed 							= .f[number of screens]\n
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
