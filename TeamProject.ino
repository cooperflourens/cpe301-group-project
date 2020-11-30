//Cooper Flourens & 
//CPE Project

#include <dht.h>

//LED Pins
int red = A15;
int yellow = A14;
int blue = A13;

//Water Sensor Pin
int water = A0;
int level = 0;

//Temp/Humidity Sensor Pin
dht DHT;
int dhtSensor = 7;
int x = 30; //maximum temperature
int y = 20; //minimum temperature



void setup() {
  // get base info from sensors (water level, temperature, humidity)
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(water, INPUT);
  Serial.begin(9600);
  // turn on blue LED because.... people like that
  
}

void loop() {
  
  digitalWrite(blue, HIGH);
  delay(2000);
  // Water Level Processes
  if (waterLevelCheck() < 150) {
    
    while(waterLevelCheck() < 300) {
      digitalWrite(blue, LOW);
      digitalWrite(red, HIGH);
      
      //display ERROR
    }
    digitalWrite(red, LOW);
    digitalWrite(blue, HIGH);
  }

  //MY TEMPERATURE HUMIDITY SENSOR IS BROKEN, theoretically this works tho
  //Temperature processes
  DHT.read11(dhtSensor);
  Serial.print("Temperature - ");
  float temperature = DHT.temperature;
  Serial.println(temperature);

  //x and y need to be defined at header or input here
  if (temperature > x || temperature < y) {
    startFan();
    recordDate();
  } else {
    stopFan();
  }


  //Humidity Processes
  Serial.print("Humidity - ");
  Serial.println(DHT.humidity);
  //display humidity to screen instead of console

  
  // angle of output control
  // up and down button? joystick maybe?

  if (offButtonPressed() == true) {
    
  }
  // if off button pressed, while (button not pressed) { mode == disabled }
  
}

int waterLevelCheck() {
  level = analogRead(water);
  return level;
}

//needs to be implemented
void startFan() {
  
}

//needs to be implemented
void stopFan() {
  
}

//needs to be implemented
void recordDate() {
  // push time and date to host computer
}


//needs to be implemented
bool offButtonPressed() {
  // this might have to be a toggle and IDK how to do that rn lol
 // if () { // if statement on the button pressed
 //   return true;
 // } else {
 //   return false;
 // }
}
