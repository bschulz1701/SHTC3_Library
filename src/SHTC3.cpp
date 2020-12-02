#include <SHTC3.h>

SHTC3::SHTC3() 
{

}

int SHTC3::begin()
{
	//Only use isEnabled() if using particle
	#if defined(ARDUINO) && ARDUINO >= 100 
		Wire.begin();
	#elif defined(PARTICLE)
		if(!Wire.isEnabled()) Wire.begin(); //Only initialize I2C if not done already //INCLUDE FOR USE WITH PARTICLE 
	#endif

	Wire.beginTransmission(SHTC3_ADR);
	Wire.write(0x00);
	return Wire.endTransmission(); //Return I2C status, just to check if device is connected to bus 

}

int SHTC3::Sleep()
{
	int Error = WriteCMD(Commands::SLEEP); //Send sleep command 
	delay(1); //Ensure sleep enables before any other commands sent 
	return Error; //Return I2C status
}

int SHTC3::Wake(bool ClearData)
{
	// if(ClearData) {
	// 	while(Wire.available()){
	// 		Wire.read(); //Clear buffer
	// 	}
	// }
	int Error = WriteCMD(Commands::WAKE); //Send wakeup command
	delay(1); //Wait >240 us for startup
	return Error; //Return I2C error 
}



int SHTC3::GetData(float *Data) //Return array of data 
{
	int Error = WriteCMD(Commands::READ_RH_LP); //FIX! Make variable??
	delay(1); //Wait > 0.8ms for updated values 
	unsigned long Timeout = millis(); //Keep track of timeout
	while(Wire.available() && (millis() - Timeout) < 25){
		Wire.read(); //Clear buffer
	}

	Wire.requestFrom(SHTC3_ADR, 6);
	Timeout = millis();
	while(Wire.available() < 6 && (millis() - Timeout) < 25); //Wait for all bytes or 25 ms
	uint8_t RH_High = Wire.read(); //Must be unsigned since humidity is never negative
	uint8_t RH_Low = Wire.read();
	uint8_t RH_CRC = Wire.read();
	uint16_t RH = (RH_High << 8) | RH_Low; 

	uint8_t Temp_High = Wire.read();
	uint8_t Temp_Low = Wire.read();
	uint8_t Temp_CRC = Wire.read();
	uint16_t Temp = (Temp_High << 8) | Temp_Low;

	// Serial.printlnf("RH_HIGH = %X, RH_LOW = %X, TEMP_HIGH = %X, TEMP_LOW = %X", RH_High, RH_Low, Temp_High, Temp_Low); //DEBUG!

	//FIX! Set error values if I2C error or out of range
	// Data[0] = (100.0*(float)(RH_High << 8 | RH_Low))/65536.0; //Calculate and return RH
	Data[0] = (int32_t)((uint32_t)RH_High << 8 | RH_Low);
	Data[0] = 100.0*Data[0]/65536.0;
	Data[1] = -45.0 + (175.0*(float)(Temp_High << 8 | Temp_Low))/65536.0;

	// Serial.println(TestCRC(RH, RH_CRC));
	// Serial.println(TestCRC(Temp, Temp_CRC));
	// Serial.println(Temp_High, HEX); //DEBUG!
	// Serial.println(Temp_Low, HEX); //DEBUG!
	// Serial.println(Temp_CRC, HEX); //DEBUG!
	bool CRC_State = TestCRC(RH, RH_CRC) | TestCRC(Temp, Temp_CRC);
	if(Error != 0 || CRC_State != true) { //Return error condition 
		Data[0] = -9999.0;
		Data[1] = -9999.0; 
	}


	// Serial.printlnf("Concat0 = %d, Concat1 = %d", (RH_High << 8 | RH_Low), (Temp_High << 8 | Temp_Low)); //DEBUG!

	//FIX! Add CRC calc!

	return Error; //Return I2C status
}


int SHTC3::WriteCMD(unsigned int CMD)
{
	Wire.beginTransmission(SHTC3_ADR);
	Wire.write(CMD >> 8); //Write MSB
	Wire.write(CMD & 0xFF); //Write LSB
	return Wire.endTransmission();
}

bool SHTC3::TestCRC(uint16_t Data, uint8_t CRC_Rec)
{
	const uint8_t POLYNOMIAL = 0x31;
	uint8_t crc = 0xFF;

	for (int j = 1; j >= 0; j--) {
		crc ^= (uint8_t)((Data >> 8*j) & 0xFF);

		for (int i = 8; i; --i) {
			crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
		}
	}
	// Serial.println(CRC_Rec, HEX); //DEBUG!
	// Serial.println(crc, HEX); //DEBUG!
	if(CRC_Rec == crc) return true;
	else return false;
}