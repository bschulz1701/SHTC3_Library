#include "SHTC3.h" 

SHTC3 RH; //Instatiate RH sensor

const unsigned long Period = 1000; //Time between printing results 

void setup() {
	RH.begin(); //Setup RH with defaults
	Serial.begin(9600);
	Serial.println("Welcome to the Jungle..."); //Make humidity pun
}

void loop() {
	float AtmosData[2] = {0}; //Create array for envrio data
	RH.Wake(); //Wake RH sensor
	RH.GetData(AtmosData); //Read data from sensor into array
	RH.Sleep(); //Sleep sensor again to reduce power

	Serial.print("Humidity = ");
	Serial.print(AtmosData[0]); //Print RH
	Serial.print(" %\t");
	Serial.print("Temperature = ");
	Serial.print(AtmosData[1]); //Print temp
	Serial.print(" ºC\n"); 
	delay(Period); //Wait for one period before new print
}