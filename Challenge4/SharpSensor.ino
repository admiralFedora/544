int ir = 5;

void setup() {
  Serial.begin(9600);
  pinMode(ir,INPUT);

}

void loop() {
  
  float volts = analogRead(ir)*0.0048828125;   // value from sensor * (5/1024) - if running 3.3.volts then change 5 to 3.3
  float distance = 65*pow(volts, -1.10);          // worked out from graph 65 = theretical distance / (1/Volts)S - luckylarry.co.uk
  Serial.println(distance);                       // print the distance
  delay(100);                                     // arbitary wait time.


}
