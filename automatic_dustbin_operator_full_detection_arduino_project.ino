#include <Servo.h>

Servo servoMotor;

// Outside Ultrasonic Sensor (for hand detection)
const int trigPin1 = 9;
const int echoPin1 = 10;

// Inside Ultrasonic Sensor (for trash level)
const int trigPin2 = 11;
const int echoPin2 = 12;

// Servo Pin
const int servoPin = 6;

// Buzzer Pin
const int buzzerPin = 5;

long duration;
int distance1;
int distance2;

// ---------------- SETTINGS ----------------
const int detectRange = 20;     // cm, hand detection range
const int holdTime = 4000;      // ms, how long to keep lid open
const int binFullLevel = 8;     // cm, distance from sensor to trash (tune based on bin size)

// Servo Angles )
const int openAngle = 365;      // angle for lid fully open
const int closeAngle = 0;      // angle for lid closed
const int servoDelay = 900;     // ms, time for servo to reach position
// const int servoDelay1 = 2000;
// ------------------------------------------

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(buzzerPin, OUTPUT);

  servoMotor.attach(servoPin);
  servoMotor.write(closeAngle);   // start with lid closed

  Serial.begin(9600);
  Serial.println("Smart Dustbin with Full Detection Ready...");
}

// Function to measure distance 
int getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long dur = pulseIn(echo, HIGH, 20000); // timeout 20ms
  int dist = dur * 0.034 / 2;
  return dist;
}

// Function for beeps
void beep(int times, int freq = 1500, int duration = 200, int gap = 200) {
  for (int i = 0; i < times; i++) {
    tone(buzzerPin, freq, duration);
    delay(duration + gap);
    noTone(buzzerPin);
  }
}

void loop() {
  // Measure hand detection
  distance1 = getDistance(trigPin1, echoPin1);

  // Measure trash level
  distance2 = getDistance(trigPin2, echoPin2);

  Serial.print("Hand Distance: ");
  Serial.print(distance1);
  Serial.print(" cm | Trash Level Distance: ");
  Serial.print(distance2);
  Serial.println(" cm");

  // 🚨 Check if bin is full
  if (distance2 > 0 && distance2 < binFullLevel) {
    // servoMotor.write(openAngle);
    // delay(servoDelay1);
    Serial.println("⚠️ BIN FULL!");
    beep(5, 1500, 400, 300);   // Double beep at higher pitch
    delay(1000);               // small pause before rechecking
    return; // Skip opening lid if full
  }

  // ✋ Check for hand detection
  if (distance1 > 0 && distance1 < detectRange) {
    // Open lid
    servoMotor.write(openAngle);
    delay(servoDelay);
    Serial.println("Lid Opened");

    delay(holdTime);

    // Close lid
    servoMotor.write(closeAngle);
    delay(servoDelay);
    Serial.println("Lid Closed");

    // ✅ Single beep when closing
    beep(1, 1500, 400, 300);

    delay(1000); // pause before next detection
  }

  delay(1000);
}
