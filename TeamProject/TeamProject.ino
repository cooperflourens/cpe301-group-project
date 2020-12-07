
// Cooper Flourens & Landon Fox
// CPE Project

#include <Wire.h>
#include <DS3231.h>
#include <DHT.h>
#include <Servo.h>
#include <LiquidCrystal.h>


// led pins
const int red_pin = 11;
const int green_pin = 10;
const int blue_pin = 9;
const int yellow_pin = 8;

// button
const int button_pin = 2;
volatile int button_previous = 0;

// states: idle = 0, running = 1, error = 2, disabled = 3
volatile bool is_active = true;
int prev_state = 4;
int current_state = 0;

// clock
DS3231 clock;
RTCDateTime dt;

// water sensor
const int water_pin = A15;
int water_level = 0;
const int water_level_min = 200; // due to water corrosion of water sensor, this may need modification

// temp and humidity sensor
const int dht_pin = 3;
DHT dht( dht_pin, DHT11 );
int temp = 0;
const int min_temp = -10;
const int max_temp = 0;
int humidity = 0;

// fan
const int fan_pin = 41;

// servo
const int servo_pin = 35;
Servo servo;

// servo pot pin
const int pot_pin = A2;

// lcd display
const int rs = 22, en = 23, d4 = 26, d5 = 27, d6 = 30, d7 = 31;
LiquidCrystal lcd( rs, en, d4, d5, d6, d7 );


void setup() {
	// set pins to input/output mode
	pinMode( red_pin,      OUTPUT );
	pinMode( green_pin,    OUTPUT );
	pinMode( blue_pin,     OUTPUT );
	pinMode( yellow_pin,   OUTPUT );
	pinMode( button_pin,   INPUT  );
	pinMode( water_pin,    INPUT  );
	pinMode( fan_pin,      OUTPUT );
	pinMode( pot_pin,      INPUT  );

	// attach iterrupt to button to enable/disable on falling edge
	attachInterrupt( digitalPinToInterrupt( button_pin ), toggleActivity, FALLING );

	// begin serial
	Serial.begin( 9600 );

	// begin clock
	clock.begin();
	clock.setDateTime( __DATE__, __TIME__ );

	// lcd begin
	lcd.begin( 16, 2 );

	// attach servo to respective pin
	servo.attach( servo_pin );

	// idle state
	idleState();
}

void loop() {
	// if the system is active, consider active states
	if ( is_active ) {
		// get water level
		water_level = getWaterLevel();

		// error state
		if ( water_level < water_level_min ) {
			current_state = 2;
			delay( 500 );
			errorState();
		}

		else {
			// delay for dht sensor
			delay( 1000 );
			// get temp and humidity and ensure there are not nan
			float t = getTemp();
			float h = getHumidity();
			if ( !isnan( t ) )
				temp = t;
			if ( !isnan( h ) )
				humidity = h;
			// display temp and humidity on lcd
			displayTempHumidity();

			// running state
			if ( temp > max_temp ) {
				current_state = 1;
				runningState();
			}
			// idle state
			else if ( temp < min_temp ) {
				current_state = 0;
				idleState();
			}
		}
	}
	// if system is not active, go to disabled state
	else {
		current_state = 3;
		disabledState();
	}

	// adjust vent
	servo.write( analogRead( pot_pin ) );
}


// toggles is_active when button is pressed, used via interrupt
void toggleActivity() {
	// prevent button presses within 200 ms to debounce
	if ( button_previous - millis() > 200 ) {
		is_active ^= true;
		button_previous = millis();
	}
}

// returns current water level
int getWaterLevel() {
	return analogRead( water_pin );
}

// returns current temp
float getTemp() {
	return dht.readTemperature();
}

// returns current humidity
float getHumidity() {
	return dht.readHumidity();
}

// starts fan
void startFan() {
	digitalWrite( fan_pin, HIGH );
}

// stops fan
void stopFan() {
	digitalWrite( fan_pin, LOW );
}

// records date to Serial, format: "y-m-d h:m:s; string"
void recordDate( const char* string ) {
	dt = clock.getDateTime();
	Serial.print( dt.year );   Serial.print( "-"  );
	Serial.print( dt.month );  Serial.print( "-"  );
	Serial.print( dt.day );    Serial.print( " "  );
	Serial.print( dt.hour );   Serial.print( ":"  );
	Serial.print( dt.minute ); Serial.print( ":"  );
	Serial.print( dt.second ); Serial.print( "; " );
	Serial.println( string );
}

// displays temp and humidity to lcd, format: "Temp: temp(deg)C\nHumidity: humidity%"
void displayTempHumidity() {
	// display temp
	stopDisplay();
	lcd.print( "Temp: " );
	lcd.print( temp );
	lcd.print( ( char ) 223 );
	lcd.print( "C" );

	// display humidity
	lcd.setCursor( 0, 1 );
	lcd.print( "Humidity: " );
	lcd.print( humidity );
	lcd.print( "%" );
}

// displays error to lcd, format: "Error: Low water"
void displayError() {
	stopDisplay();
	lcd.print( "Error: Low water" );
}

// clears lcd
void stopDisplay() {
	lcd.clear();
}


// start disabled state
void disabledState() {
	if ( prev_state != current_state ) {
		// turn on yellow light
		digitalWrite( blue_pin,   LOW  );
		digitalWrite( green_pin,  LOW  );
		digitalWrite( red_pin,    LOW  );
		digitalWrite( yellow_pin, HIGH );

		// stop fan and record
		recordDate( "Stop fan" );
		stopFan();

		// stop display
		stopDisplay();

		// update state
		prev_state = current_state;
	}
}

// start idle state
void idleState() {
	if ( prev_state != current_state ) {
		// turn on green light
		digitalWrite( blue_pin,   LOW  );
		digitalWrite( green_pin,  HIGH );
		digitalWrite( red_pin,    LOW  );
		digitalWrite( yellow_pin, LOW  );

		// stop fan and record
		recordDate( "Stop fan" );
		stopFan();

		// update state
		prev_state = current_state;
	}
}

// start running state
void runningState() {
	if ( prev_state != current_state ) {
		// turn on blue light
		digitalWrite( red_pin,    LOW  );
		digitalWrite( green_pin,  LOW  );
		digitalWrite( blue_pin,   HIGH );
		digitalWrite( yellow_pin, LOW  );

		// start fan and record
		recordDate( "Start fan" );
		startFan();

		// update state
		prev_state = current_state;
	}
}

// start error state
void errorState() {
	if ( prev_state != current_state ) {
		// turn on red light
		digitalWrite( blue_pin,   LOW  );
		digitalWrite( green_pin,  LOW  );
		digitalWrite( red_pin,    HIGH );
		digitalWrite( yellow_pin, LOW  );

		// stop fan
		recordDate( "Stop fan" );
		stopFan();

		// display error
		displayError();

		// update state
		prev_state = current_state;
	}
}
