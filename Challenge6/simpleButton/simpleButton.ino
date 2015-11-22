#define RedPin 7
#define BluePin 8
#define GreenPin 9
#define ButtonPin 10

int buttonState = 0;
int buttonCount = 0;
int timeSinceButton = 0;

void setup() 
{ 
  buttonState = 0;
  buttonCount = 0;
  pinMode(RedPin, OUTPUT);
  pinMode(BluePin, OUTPUT);
  pinMode(GreenPin, OUTPUT);
  pinMode(ButtonPin, INPUT);

  digitalWrite(RedPin, LOW);
  digitalWrite(BluePin, LOW);
  digitalWrite(GreenPin, LOW);
  
  // start serial
  Serial.begin(9600);

}

// looks for an xbee that just joined the network and starts sending to the last one that joined
void loop() 
{
  //buttonState = digitalRead(ButtonPin);
  if (digitalRead(ButtonPin) && (millis() - timeSinceButton) > 250)
  {
    //delay(250);
    buttonState = 1;
    timeSinceButton = millis();
    buttonCount++;
    Serial.println(buttonCount);
    if(buttonCount==1)
    {
      digitalWrite(RedPin, HIGH);
      digitalWrite(BluePin, LOW); 
      digitalWrite(GreenPin, LOW);
    }
    else if(buttonCount == 2)
    {
      digitalWrite(BluePin, HIGH); 
      digitalWrite(RedPin, LOW);
      digitalWrite(GreenPin, LOW);
    }
    else if(buttonCount == 3)
    {
      digitalWrite(GreenPin, HIGH);
      digitalWrite(RedPin, LOW); 
      digitalWrite(BluePin, LOW);
      buttonCount=0;
    }

  } else{
    buttonState = 0;
  }



  
}
