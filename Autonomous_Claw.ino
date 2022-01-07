#include <NewPing.h>
#include <Servo.h>

#define OPEN_LIGHT_CHANNEL 7
#define CLOSED_LIGHT_CHANNEL 6

#define VCC_PIN 12
#define TRIGGER_PIN 11    // sonar trigger pin will be attached to Arduino pin 11
#define ECHO_PIN 10       // sonar echo pint will be attached to Arduino pin 10
#define GROUND_PIN 9

#define ON 1
#define OFF 0

#define OPEN 0
#define CLOSED 1

#define MAX_DISTANCE 500  // maximum distance set to 500 cm

#define MAX_READINGS 10

//Sonar Setup
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // initialize NewPing

//Servo Setup
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  // put your setup code here, to run once:

  //Lights
  pinMode(OPEN_LIGHT_CHANNEL, OUTPUT);
  pinMode(CLOSED_LIGHT_CHANNEL, OUTPUT);

  //initial condition of lights
  digitalWrite(OPEN_LIGHT_CHANNEL, HIGH);
  digitalWrite(CLOSED_LIGHT_CHANNEL, LOW);

  //Sonar sensor
  Serial.begin(9600);
  pinMode(ECHO_PIN, INPUT) ;  
  pinMode(TRIGGER_PIN, OUTPUT) ;
  pinMode(GROUND_PIN, OUTPUT);    // tell pin 10 it is going to be an output
  pinMode(VCC_PIN, OUTPUT);       // tell pin 13 it is going to be an output
  digitalWrite(GROUND_PIN,LOW);   // tell pin 10 to output LOW (OV, or ground)
  digitalWrite(VCC_PIN, HIGH) ;   // tell pin 13 to output HIGH (+5V)

  // Servo
  myservo.attach(13);  // attaches the servo on pin 13 to the servo object
  myservo.write(0);   // sets servo to 0 degrees at startup

}

void loop() {
  // put your main code here, to run repeatedly:


  int claw_status = OPEN;        //0 means open, 1 means closed
  double sum_readings;           // sum of 10 consecutive readings
  double average_reading;        // average out of 10 consecutive readings

  while(1) {    //infinite loop

    sum_readings = 0.0;

    // Obtain sum of 10 consecutive readings
    for (int i = 0; i < MAX_READINGS; i++) {
      int DISTANCE_IN_CM = sonar.ping_cm();
      Serial.println(DISTANCE_IN_CM);
      sum_readings += DISTANCE_IN_CM;
      delay(1);
    }

    // obtain average reading
    average_reading = sum_readings / MAX_READINGS;
    

    //if claw is open and ground is sensed
    if ((average_reading < 15) && (claw_status == OPEN)) {
      
      delay(2000);  //2 second delay after sensing ground

      for (int i = 0; i <= 180; i++) {
          myservo.write(i);
          delay(2);  //delay 2 ms per degree turn
      }

      // Turn Closed light on and open light off
      digitalWrite(CLOSED_LIGHT_CHANNEL, HIGH);
      digitalWrite(OPEN_LIGHT_CHANNEL, LOW);

      claw_status = CLOSED;
      Serial.println("Claw Closed");

      delay(3000);    // 3 Second delay so claw doesn't immediately sense ground again
    }

    //if claw is open and ground is sensed
    else if ((average_reading < 15) && (claw_status == CLOSED)) {

      delay(2000);  //2 second delay after sensing ground
      
      for (int i = 180; i >= 0; i--) {
          myservo.write(i);
          delay(2);  //delay 2 ms per degree turn
      }

      // Turn Closed light off and open light on
      digitalWrite(CLOSED_LIGHT_CHANNEL, LOW);
      digitalWrite(OPEN_LIGHT_CHANNEL, HIGH);

      claw_status = OPEN;
      Serial.println("Claw Open");
      delay(3000);      // 3 Second delay so claw doesn't immediately sense ground again
    }
  }
}
