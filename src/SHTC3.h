#ifndef SHTC3_h
#define SHTC3_h

//Use conditional include between particle and arduino enviroment 
#if defined(ARDUINO) && ARDUINO >= 100 
	#include <Arduino.h>
	#include <Wire.h>
#elif defined(PARTICLE)
	#include <Particle.h>
#endif

// #define ADR 0x70 //Define hardcoded address
// #define SLEEP 0xB098
// #define WAKE 0x3517
// #define READ_RH_LP 0x44DE
// #define READ_RH 0x5C24
// #define READ_TEMP_LP 0x6458
// #define READ_TEMP 0x7CA2

namespace Commands {
			constexpr uint16_t SLEEP = 0xB098;
			constexpr uint16_t WAKE = 0x3517;
			constexpr uint16_t READ_RH_LP = 0x44DE;
			constexpr uint16_t READ_RH = 0x5C24;
			constexpr uint16_t READ_TEMP_LP = 0x6458;
			constexpr uint16_t READ_TEMP = 0x7CA2;
		};

class SHTC3
{
	public:
		SHTC3();
		int begin();
		int Sleep();
		int Wake(bool ClearData = true); //Clear out data by default 
		int GetData(float *Data);

	private:
		int WriteCMD(unsigned int CMD);
		bool TestCRC(uint16_t Data, uint8_t CRC_Rec);
		const int ADR = 0x70; //Default address
};

#endif