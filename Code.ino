#include <Servo.h>
#include <NewPing.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Motor control pins
int motor1Pin1 = 5;
int motor1Pin2 = 4;
int motor2Pin1 = 6;
int motor2Pin2 = 7;

// IR sensor pins
int irSensorLeft = 2;
int irSensorRight = 12;

#define TRIGGER_PIN 11
#define ECHO_PIN 8
#define MAX_DISTANCE 300
#define BUZZER_DISTANCE_THRESHOLD 20 // Distance threshold for buzzer in cm

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

const int buzzer = 13;
const int servoPin = 3;
int motorSpeed = 200; // Motor speed (0-255)

Servo servo_motor;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

char command = 'S'; // Initialize with 'S' to ensure motors are stopped initially
const unsigned long commandDuration1  = 400; // Command duration in milliseconds
const unsigned long commandDuration2  = 100; // Command duration in milliseconds
const unsigned long commandDuration3  = 600; // Command duration in milliseconds

void setup() {
  Serial.begin(9600);

  // Initialize motor pins
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // Initialize IR sensor pins
  pinMode(irSensorLeft, INPUT);
  pinMode(irSensorRight, INPUT);

  // Initialize buzzer pin
  pinMode(buzzer, OUTPUT);

  // Initialize servo
  servo_motor.attach(servoPin);
  servo_motor.write(90);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Display setup message
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(F("Ready for commands."));
  display.display();

  Serial.println("Ready for commands.");
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.read();
    Serial.print("Command received: ");
    Serial.println(command);

    display.clearDisplay();
    display.setCursor(0, 0);

    // Display the command
    if (command == 'F') {
      display.print("Forward");
      moveForward();
    } else if (command == 'B') {
      display.print("Backward");
      moveBackward();
    } else if (command == 'L') {
      display.print("Left");
      turnLeft();
    } else if (command == 'R') {
      display.print("Right");
      turnRight();
    } else if (command == 'S') {
      display.print("Stop");
      stopMoving();
    } else if (command == 'X') {
      display.print("Reverse Left");
      reverseLeft();
    } else if (command == 'Y') {
      display.print("Reverse Right");
      reverseRight();
    }
    display.display();
  }

  int irLeft = digitalRead(irSensorLeft);
  int irRight = digitalRead(irSensorRight);
  int distance = sonar.ping_cm();

  // Obstacle detection
  if (irLeft == LOW || irRight == LOW || distance < BUZZER_DISTANCE_THRESHOLD) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Obstacle at ");
    display.print(distance);
    display.println(" cm");
    display.display();
    stopAndBeep();
    moveBackward();
  }

  delay(100); // Add a small delay to allow for reading new commands
}

// Move forward
void moveForward() {
  analogWrite(motor1Pin1, motorSpeed);
  analogWrite(motor1Pin2, LOW);
  analogWrite(motor2Pin1, motorSpeed);
  analogWrite(motor2Pin2, LOW);
  delay(commandDuration1);
  stopMoving();
}

// Move backward
void moveBackward() {
  analogWrite(motor1Pin1, LOW);
  analogWrite(motor1Pin2, motorSpeed);
  analogWrite(motor2Pin1, LOW);
  analogWrite(motor2Pin2, motorSpeed);
  delay(commandDuration1);
  stopMoving();
}

// Turn right
void turnRight() {
  analogWrite(motor1Pin1, LOW);
  analogWrite(motor1Pin2, motorSpeed);
  analogWrite(motor2Pin1, motorSpeed);
  analogWrite(motor2Pin2, LOW);
  delay(commandDuration2);
  stopMoving();
}

// Turn left
void turnLeft() {
  analogWrite(motor1Pin1, motorSpeed);
  analogWrite(motor1Pin2, LOW);
  analogWrite(motor2Pin1, LOW);
  analogWrite(motor2Pin2, motorSpeed);
  delay(commandDuration2);
  stopMoving();
}

// Reverse left
void reverseLeft() {
  analogWrite(motor1Pin1, LOW);
  analogWrite(motor1Pin2, motorSpeed);
  analogWrite(motor2Pin1, motorSpeed);
  analogWrite(motor2Pin2, LOW);
  delay(commandDuration3);
  stopMoving();
}

// Reverse right
void reverseRight() {
  analogWrite(motor1Pin1, motorSpeed);
  analogWrite(motor1Pin2, LOW);
  analogWrite(motor2Pin1, LOW);
  analogWrite(motor2Pin2, motorSpeed);
  delay(commandDuration3);
  stopMoving();
}

// Stop moving
void stopMoving() {
  analogWrite(motor1Pin1, LOW);
  analogWrite(motor1Pin2, LOW);
  analogWrite(motor2Pin1, LOW);
  analogWrite(motor2Pin2, LOW);
}

// Stop and beep
void stopAndBeep() {
  stopMoving();
  digitalWrite(buzzer, HIGH); // Turn on buzzer
  delay(500); // Keep the beep for 500 milliseconds
  digitalWrite(buzzer, LOW); // Turn off buzzer

  // Display the obstacle detection message
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Obstacle detected!");
  display.display();
}
