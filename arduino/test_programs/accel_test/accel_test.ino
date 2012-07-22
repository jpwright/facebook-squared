#define ACCELY       1
#define YNEUTRAL     347
#define YDOWNTHRESH  315
#define YUPTHRESH    375
//-1 = previous
//+1 = next
//0  = neutral
int lastTurnState = 0;



void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int currentYval = analogRead(ACCELY);
  if ((currentYval < YDOWNTHRESH) && lastTurnState == 0)
  {
    Serial.println("previous");
    lastTurnState = -1;
  }
  else if ((currentYval > YUPTHRESH) && lastTurnState == 0)
  {
    Serial.println("next");
    lastTurnState = 1;
  }
  else if ((currentYval <= YUPTHRESH) && (currentYval >= YDOWNTHRESH))
  {
    lastTurnState = 0;
  }
  
  
  delay(300);
}


