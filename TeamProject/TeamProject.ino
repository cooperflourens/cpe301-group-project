//Cooper Flourens & 
//CPE Project



/* OVERALL TO DO LIST:
 *  Push Date/Time when start and stop
 *  Get Push Button Working
 *  implement LCD Screen showing Temp/Humidity
 *  Fully Troubleshoot Disabled State
 *  Print Error during ERROR state
 *  Angle of Output
 *  Record Time/Date when switching from active mode
 *  Project Overview Document
 *  I may have missed something, I'll come back to this later ~Cooper
 */



#include <dht.h>

//LED Pins
int red = A15;
int green = A14;
int blue = A13;
//for yellow, it is red + green led active

//Water Sensor Pin
int water = A0;
int level = 0;
//Water Level Minimum
int waterLevelMin = 125; 

//Temp/Humidity Sensor Pin
dht DHT;
int dhtSensor = 8;
int x = 30; //minimum temperature
int y = 20; //maximum temperature

//Fan Pin
int fanPin = 4;

//Button Info
int buttonPin = 50;
int buttonSignal = 0;
int buttonPrevious = 0;
int buttonState = 0;




void setup() {
  // get base info from sensors (water level, temperature, humidity)
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(water, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(fanPin, OUTPUT);

  Serial.begin(9600);
  
  // turn on active state for blue LED
  activeState();
}

void loop() {
  //Delay for the temperature/humidity sensor
  delay(2000);

  
  /*Button Press Attempt
  buttonPrevious = buttonSignal;
  buttonSignal = digitalRead(buttonPin);
  if(buttonSignal != buttonPrevious) {
    //in the situation the button has been pressed
    while(buttonSignal != buttonPrevious) {
      disabledState();
      buttonPrevious = buttonSignal;
      buttonSignal = digitalRead(buttonPin);
    }
    idleState();
  }*/

  
  // Water Level Processes

  if (waterLevelCheck() < waterLevelMin) {
    errorState();
    idleState();
  }






  //MY TEMPERATURE HUMIDITY SENSOR IS BROKEN, theoretically this works tho


  
  //Temperature processes
  DHT.read11(dhtSensor);
  Serial.print("Temperature - ");
  float temperature = DHT.temperature;
  Serial.println(temperature);

  //x and y need to be defined at header or input here
  if (temperature > x && temperature < y) {
    activeState();
  } else {
    idleState();
  }




  //Humidity Processes
  Serial.print("Humidity - ");
  Serial.println(DHT.humidity);
  //display humidity to screen instead of console

  
  // angle of output control
  // up and down button? joystick maybe?
  //disabled state



  
}

int waterLevelCheck() {
  level = analogRead(water);
  return level;
}

//needs to be implemented
void startFan() {
  digitalWrite(fanPin, HIGH);
}

//needs to be implemented
void stopFan() {
  digitalWrite(fanPin, LOW);
}

//needs to be implemented
void recordDate() {
  // push time and date to host computer
}



void activeState() {
   //turn on blue light
   digitalWrite(red, LOW);
   digitalWrite(green, LOW);
   digitalWrite(blue, HIGH);
   //turn on fan
   startFan();
}

void disabledState() {
  //turn on yellow light
  digitalWrite(blue, LOW);
  digitalWrite(green, HIGH);
  digitalWrite(red, HIGH);
  stopFan();
}

void idleState() {
  //turn on yellow light
  digitalWrite(blue, LOW);
  digitalWrite(green, HIGH);
  digitalWrite(red, HIGH);
  stopFan();
}

void errorState() {
  stopFan();
  while(waterLevelCheck() < 300) {
  //turn on red light
  digitalWrite(blue, LOW);
  digitalWrite(green, LOW);
  digitalWrite(red, HIGH);
  //display ERROR
  }
}
