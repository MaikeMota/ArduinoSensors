/*
	Ultrasonic.h - Library for Ping and HR-SC04 Ultrasonic Ranging Module.

	In order to use PING sensors (One wire for data only), use:
		PingUltrasonic myUltrasonic(dataPin);
	
	In order to use with two wire (Trigger and Echo) sensors, use:
		PingUltrasonic myUltrasonic(triggerPin, echoPin);


	Working range: 3 - 500

	For instructions, go to https://github.com/ivanseidel/Arduino-Sensors

	Modified by	Ivan Seidel Gomes, June, 2013.
	Released into the public domain.
*/

#ifndef Ultrasonic_h
#define Ultrasonic_h

#include <Arduino.h>
#include 

class Ultrasonic: public Thread, public DistanceInterface
{
private:
	int trigPin;
	int echoPin;

	long duration;
	float distance;

public:
	Ultrasonic(int _trigPin, int _echoPin = -1){
		trigPin = _trigPin;

		/*
			Ping sensors uses only one pin to trigger and echo
		*/
		if(echoPin < 0)
			echoPin = trigPin;
		else
			echoPin = _echoPin;

		pinMode(trigPin, OUTPUT);
		pinMode(echoPin, INPUT);

		minDistance = 3;
		maxDistance = 500;

		Thread::Thread();
		setInterval(50);
	}

	virtual float getDistance(){
		return distance;
	}

	virtual float readDistance(){
		// Set pin as output
		pinMode(trigPin, OUTPUT);

		// Make shure it's LOW (Erase state)
		digitalWrite(trigPin, LOW);
		delayMicroseconds(2);

		// Send pulse
		digitalWrite(trigPin, HIGH);
		delayMicroseconds(10);
		digitalWrite(trigPin, LOW);

		// Set pin as input
		pinMode(echoPin, INPUT);

		// Reads the pulse
		bool exited = false
		long startTime = micros();

		// Waits it to get HIGH
		while(digitalRead(echoPin) == LOW){
			if(micros() - startTime > 100){
				exited = true;
				break;
			}
		}

		// Reset timer
		startTime = micros();

		// Reads HIGH pulse time
		while(digitalRead(echoPin) == HIGH){
			if(micros() - startTime > 50000){
				exited = true;
				break;
			}
		}

		// Calculates duration
		duration = micros() - startTime;

		// If error, then doesen't update value
		if(!exited)
			distance = duration / 29.0 / 2;
		else
			distance = maxDistance;

		return getDistance();
	}

	/*
		Thread implementation
	*/
	virtual void run(){
		readDistance();

		runned();
	}

};

#endif