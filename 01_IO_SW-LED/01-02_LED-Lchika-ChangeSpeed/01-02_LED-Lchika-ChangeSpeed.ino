//Connect SingleLED to OUT connector

#define ledPin 25
#define on HIGH
#define off LOW

void setup() {
  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(ledPin, on); 
  delay(150); //msec (1000msec=1sec)
  digitalWrite(ledPin, off);
  delay(150); //msec (1000msec=1sec)
  digitalWrite(ledPin, on); 
  delay(150); //msec (1000msec=1sec)
  digitalWrite(ledPin, off);
  delay(200); //msec (1000msec=1sec)
  digitalWrite(ledPin, on); 
  delay(1500); //msec (1000msec=1sec)
  digitalWrite(ledPin, off);
  delay(2000); //msec (1000msec=1sec)
}
