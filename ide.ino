const int trigPin = 9;
const int echoPin = 10;
const int ledPin = 13;
const int switchPin = 2;
bool helmetDetected = false;  // Track helmet status from Python

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);  // Switch ON = LOW
  digitalWrite(ledPin, LOW);
  Serial.println("Arduino Ready");
}

void loop() {
  // Ultrasonic sensor
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  if (duration == 0) {
    Serial.println("NO_OBJECT");
    helmetDetected = false;
    digitalWrite(ledPin, LOW);
    delay(50);
    return;
  }
  
  int distance = duration * 0.034 / 2;
  
  if (distance < 100 && distance > 2) {
    Serial.println("TRIGGER");
    
    // **CRITICAL: Check for Python commands EVERY loop**
    while (Serial.available() > 0) {
      char cmd = Serial.read();
      Serial.print("Received: "); Serial.println(cmd);  // DEBUG
      
      if (cmd == 'H') {
        helmetDetected = true;
      } else if (cmd == 'N') {
        helmetDetected = false;
      }
    }
    
    // **LED GLOWS: Helmet + Switch ON**
    if (helmetDetected && digitalRead(switchPin) == LOW) {
      digitalWrite(ledPin, HIGH);  // TURN ON LED
      Serial.println("LED ON - Helmet + Switch OK");
    } else {
      digitalWrite(ledPin, LOW);
      Serial.println("LED OFF");
    }
    
    delay(100);  // Fast loop for serial reading
  } else {
    Serial.println("NO_OBJECT");
    helmetDetected = false;
    digitalWrite(ledPin, LOW);
  }
}
