void setup() {
  Serial.begin(9600);
}

void loop() {
  //Wait until we have received a '.', indicating the start of a command
  while (Serial.read() != '.'); //Throw away any data before the '.'
  while (Serial.available() == 0); //Wait till we've got some data
  
  //Read in the command byte
  int val = Serial.read();
  
  //LED Control
  if (val == 'p')
  {
    while(!Serial.available());
    int line_num = Serial.read();
    while(!Serial.available());
    boolean done = false;
    String command = "";
    int pos = 0;
    while(!done) {
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
}
