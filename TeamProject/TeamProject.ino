
// Cooper Flourens & Landon Fox
// CPE Project


/* OVERALL TO DO LIST:
 *  Push Date/Time when start and stop
 *  Angle of Output
 *  Project Overview Document
 */


#include <DHT.h>
#include <LiquidCrystal.h>


// LED Pins
int red_pin = 4;
int green_pin = 3;
int blue_pin = 2;

//Button Info
int button_pin = 6;
int button_signal = 0;
int button_previous = 0;
int button_state = 0;
volatile bool is_active = true;

// Water Sensor Pin
int water_pin = A15;
int water_level = 0;
// Water Level Minimum
int water_level_min = 125; 

// Temp/Humidity Sensor Pin
int dht_pin = 8;
DHT dht( dht_pin, DHT11 );
int temp = 0;
int min_temp = 20;
int max_temp = 30;

int humidity = 0;

//Fan Pin
int fan_pin = 4; // TODO: decide fan pin

// LiquidCrystal lcd display
const int rs = 22, en = 23, d4 = 26, d5 = 27, d6 = 30, d7 = 31;
LiquidCrystal lcd( rs, en, d4, d5, d6, d7 );


void setup() {
	// get base info from sensors (water level, temperature, humidity)
	pinMode( red_pin,    OUTPUT );
	pinMode( green_pin,  OUTPUT );
	pinMode( blue_pin,   OUTPUT );
	pinMode( button_pin, INPUT  );
	pinMode( water_pin,  INPUT  );
	pinMode( fan_pin,    OUTPUT );

	attachInterrupt( digitalPinToInterrupt( button_pin ), toggleActivity, HIGH );

	Serial.begin( 9600 );
	lcd.begin( 16, 2 );

	// turn on idle state
	idleState();
}

void loop() {
	if ( is_active ) {
		water_level = getWaterLevel();

		// error state
		if ( water_level < water_level_min )
			errorState();
		else {
			// delay for dht sensor
			delay( 2000 );
			temp = getTemp();
			humidity = getHumidity();

			// running state
			if ( temp > max_temp )
				runningState();
			// idle state
			else if ( temp < min_temp )
				idleState();
		}
	}
	else
		disabledState();
}


void toggleActivity() {
	is_active ^= 0x01;
}

int getWaterLevel() {
	water_level = analogRead( water_pin );
	return water_level;
}

float getTemp() {
	return dht.readTempature();
}

float getHumidity() {
	return dht.readHumidity();
}

//needs to be implemented
void startFan() {
	digitalWrite( fan_pin, HIGH );
}

//needs to be implemented
void stopFan() {
	digitalWrite( fan_pin, LOW );
}

//needs to be implemented
void recordDate( char* string ) {
	// push time and date to host computer using I^2C
}

void displayTempHumidity() {
	// display temp
	stopDisplay();
	lcd.print( "Temp: " );
	lcd.print( temp );
	lcd.print( ( char ) 223 );
	lcd.print( "C" )

	// display humidity
	lcd.setCursor( 0, 1 );
	lcd.print( "Humidity: " );
	lcd.print( humidity );
	lcd.print( "%" );
}

void displayError() {
	stopDisplay();
	lcd.print( "Error: Low water" );
}

void stopDisplay() {
	lcd.clear();
}

// DECIDE: will these functions be ran every loop?
void disabledState() {
	//turn on yellow light
	digitalWrite( blue_pin, LOW );
	digitalWrite( green_pin, HIGH );
	digitalWrite( red_pin, HIGH );

	stopFan();

	stopDisplay();
}

void idleState() {
	// turn on green light
	digitalWrite( blue_pin, LOW );
	digitalWrite( green_pin, HIGH );
	digitalWrite( red_pin, LOW );

	recordDate( "Stop fan" ); // TODO: this will be displayed every loop, which is not desired
	stopFan();

	displayTempHumidity();
}

void runningState() {
	//turn on blue light
	digitalWrite( red_pin, LOW );
	digitalWrite( green_pin, LOW );
	digitalWrite( blue_pin, HIGH );

	recordDate( "Start fan" );
	startFan();

	displayTempHumidity();
}

void errorState() {
	//turn on red light
	digitalWrite( blue_pin, LOW );
	digitalWrite( green_pin, LOW );
	digitalWrite( red_pin, HIGH );

	recordDate( "Stop fan" );
	stopFan();

	displayError();
}
