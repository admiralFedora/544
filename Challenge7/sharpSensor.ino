  int ir = 5;
  int led = 11;
  int input = 0;
  int distance = 0;
  void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(ir,INPUT);

}
  void loop() 
  {
    input = analogRead(ir);
    distance = map(input,512,0,15,150);
    if(distance<130){
      digitalWrite(led, HIGH);
      }
      else{
        digitalWrite(led, LOW);
        }
    Serial.println(distance);
    delay(200);
  }

