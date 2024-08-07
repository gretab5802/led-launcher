/*
 * Code for acceleromter from https://lastminuteengineers.com/mpu6050-accel-gyro-arduino-tutorial/
 */

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Servo.h>

Adafruit_MPU6050 mpu;

// Define 3 LED pins
int led_1 = 2;
int led_2 = 3;
int led_3 = 4;

// Value to track which LED has been activated (1-3)
int activated;

// Define buzzer pin
int buzzer = 13;

// Whether or not the buzzer has been activated
bool buzzed;

// Define 3 servo motors
Servo myservo1;
Servo myservo2;
Servo myservo3;

void setup(void) {
  Serial.begin(115200);

  pinMode(led_1, OUTPUT);
  pinMode(led_2, OUTPUT);
  pinMode(led_3, OUTPUT);

  pinMode(buzzer, OUTPUT);
  
  myservo1.attach(7);
  myservo2.attach(8);
  myservo3.attach(9);

  // Begin servos at position 0
  myservo1.write(0);
  myservo2.write(0);
  myservo3.write(0);
  
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  // set accelerometer range to +-8G
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);

  // set gyro range to +- 500 deg/s
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  // set filter bandwidth to 21 Hz
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}

void loop() {
  // Get new sensor events with the readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Print out the values
  Serial.print(a.acceleration.x);
  Serial.print(",");
  Serial.print(a.acceleration.y);
  Serial.print(",");
  Serial.print(a.acceleration.z);
  Serial.print(", ");
  Serial.print(g.gyro.x);
  Serial.print(",");
  Serial.print(g.gyro.y);
  Serial.print(",");
  Serial.print(g.gyro.z);
  Serial.println("");

  // Get servo position
  int servo_pos = myservo1.read();

  // Check range of x acceleration (left-right) and light LEDs accordingly
  if (4 <= a.acceleration.x && !buzzed) {
    digitalWrite(led_3, HIGH); 
    digitalWrite(led_2, LOW); 
    digitalWrite(led_1, LOW);
    activated = 3;
  }
  else if (-4 <= a.acceleration.x && !buzzed) { 
    digitalWrite(led_3, LOW); 
    digitalWrite(led_2, HIGH); 
    digitalWrite(led_1, LOW);
    activated = 2;
  }
  else if (a.acceleration.x < -4 && !buzzed) {
    digitalWrite(led_3, LOW); 
    digitalWrite(led_2, LOW); 
    digitalWrite(led_1, HIGH);
    activated = 1;
  }
  // Check range of y acceleration (forward-backward) and adjust buzzer/servos accordingly
  if (a.acceleration.y < -7 && buzzed) {
    // Activated value represents which LED is on and therefore which servo will move
    if (activated == 1) {
      myservo1.write(120);
    }
    if (activated == 2) {
      myservo2.write(120);
    }
    if (activated == 3) {
      myservo3.write(120);
    }
    buzzed = true;
  }
  // If y acceleration is not forward and buzzer isn't buzzing, reset servo position
  else if (a.acceleration.y > 1) {
    myservo1.write(0);
    myservo2.write(0);
    myservo3.write(0);
    buzzed = false;
  }
  // Activate buzzer if y accleration is backwards quick enough
  if (a.acceleration.y > 8) {
    tone(buzzer, 500);
    buzzed = true;
  }
  else if (a.acceleration.y < 1) {
    noTone(buzzer);
  }
  delay(100);
}
