#include <stdlib.h>
#include <Arduino.h>
#include "dataLinkProtocolTest.h"

uint8_t readBuffer[100] = {0}; 

static uint8_t sht75_crc_table[] = {

	0,   49,  98,  83,  196, 245, 166, 151, 185, 136, 219, 234, 125, 76,  31,  46,
	67,  114, 33,  16,  135, 182, 229, 212, 250, 203, 152, 169, 62,  15,  92,  109,
	134, 183, 228, 213, 66,  115, 32,  17,  63,  14,  93,  108, 251, 202, 153, 168,
	197, 244, 167, 150, 1,   48,  99,  82,  124, 77,  30,  47,  184, 137, 218, 235,
	61,  12,  95,  110, 249, 200, 155, 170, 132, 181, 230, 215, 64,  113, 34,  19,
	126, 79,  28,  45,  186, 139, 216, 233, 199, 246, 165, 148, 3,   50,  97,  80,
	187, 138, 217, 232, 127, 78,  29,  44,  2,   51,  96,  81,  198, 247, 164, 149,
	248, 201, 154, 171, 60,  13,  94,  111, 65,  112, 35,  18,  133, 180, 231, 214,
	122, 75,  24,  41,  190, 143, 220, 237, 195, 242, 161, 144, 7,   54,  101, 84,
	57,  8,   91,  106, 253, 204, 159, 174, 128, 177, 226, 211, 68,  117, 38,  23,
	252, 205, 158, 175, 56,  9,   90,  107, 69,  116, 39,  22,  129, 176, 227, 210,
	191, 142, 221, 236, 123, 74,  25,  40,  6,   55,  100, 85,  194, 243, 160, 145,
	71,  118, 37,  20,  131, 178, 225, 208, 254, 207, 156, 173, 58,  11,  88,  105,
	4,   53,  102, 87,  192, 241, 162, 147, 189, 140, 223, 238, 121, 72,  27,  42,
	193, 240, 163, 146, 5,   52,  103, 86,  120, 73,  26,  43,  188, 141, 222, 239,
	130, 179, 224, 209, 70,  119, 36,  21,  59,  10,  89,  104, 255, 206, 157, 172
};

/*
 * uint8_t crc_8( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_8() calculates the 8 bit wide CRC of an input string of a
 * given length.
 */

uint8_t crc_8( const unsigned char *input_str, size_t num_bytes ) {

	size_t a;
	uint8_t crc;
	const unsigned char *ptr;

	crc = 0;
	ptr = input_str;

	if ( ptr != NULL ) for (a=0; a<num_bytes; a++) {

		crc = sht75_crc_table[(*ptr++) ^ crc];
	}

	return crc;

}  /* crc_8 */

/*
 * uint8_t update_crc_8( unsigned char crc, unsigned char val );
 *
 * Given a databyte and the previous value of the CRC value, the function
 * update_crc_8() calculates and returns the new actual CRC value of the data
 * comming in.
 */

uint8_t update_crc_8( unsigned char crc, unsigned char val ) {

	return sht75_crc_table[val ^ crc];

}  /* update_crc_8 */

// converts gps long lat to two packets
void gpsDataConverter(uint8_t (&gpsPacket) [10], float latit, float longit){
  double latAsPercent  = latit /90.0; //yeah ik its an intermediate step
  double lonAsPercent = longit/180.0;

  uint32_t latBytes = (uint32_t) (abs(latAsPercent) * 8388608.0); //2^23
  uint32_t lonBytes = (uint32_t) (abs(lonAsPercent) * 8388608.0);


  if(latAsPercent < 0){ //this is how u flip signs in twos complement
    latBytes = ~latBytes + 1;
  }
  if(lonAsPercent < 0){
    lonBytes = ~lonBytes + 1;
  }
  
  gpsPacket[0] = 1; //gps lat code
  gpsPacket[1] = (uint8_t) (latBytes >> 16);
  gpsPacket[2] = (uint8_t) (latBytes >> 8);
  gpsPacket[3] = (uint8_t) latBytes;

  gpsPacket[5] = 2; //gps long code
  gpsPacket[6] = (uint8_t) (lonBytes >> 16);
  gpsPacket[7] = (uint8_t) (lonBytes >> 8);
  gpsPacket[8] = (uint8_t) lonBytes;

  gpsPacket[4] = crc_8(gpsPacket, 4); //crc for lat

  // gpsPacket itself is only a pointer to the first item, the +5 tells it to point to the next item
  gpsPacket[9] = crc_8(gpsPacket + 5, 4);
};

//gps altitude, converts to two bytes as integer meters
//takes in double as input
void gpsAltConverter(uint8_t (&GPSAlt) [4], float alt){

  uint16_t bytesalt = (uint16_t) alt;

  //if <0 just set to zero
  if(alt < 0){
    uint16_t bytesalt = 0;
  }
  
  GPSAlt[0] = 3; //gps alt code, see document
  GPSAlt[1] = (uint8_t) (bytesalt >> 8);
  GPSAlt[2] = (uint8_t) bytesalt;

  GPSAlt[3] = crc_8(GPSAlt, 3); //comuting CRC, passes only first 3 bytes
}

void gpsSpeedConverter(uint8_t (&GPSSpeed) [4], float speed){

  uint16_t bytesspeed = (uint16_t) (speed * 463.0 / 9.0);  // convert from knots to cm/s

  if(speed < 0){
    uint16_t bytesspeed = 0;
  }

  GPSSpeed[0] = 4;
  GPSSpeed[1] = (uint8_t) (bytesspeed >> 8);
  GPSSpeed[2] = (uint8_t) bytesspeed;

  GPSSpeed[3] = crc_8(GPSSpeed, 3);
}

void gpsAngleConverter(uint8_t (&GPSAngle) [4], float angle){

  double angleAsPercent = angle / 360.0;

  uint16_t bytesangle = (uint16_t) (angleAsPercent * 65535.0);

  if(angle < 0){
    uint16_t bytesangle = 0;
  }

  GPSAngle[0] = 5;
  GPSAngle[1] = (uint8_t) (bytesangle >> 8);
  GPSAngle[2] = (uint8_t) bytesangle;

  GPSAngle[3] = crc_8(GPSAngle, 3);  
}

void tempConverter(uint8_t (&temperature) [3], float temp){

  uint8_t bytestemp = (uint8_t) (temp + 80.0);

  //if <0 just set to zero
  if(bytestemp < 0){
    uint8_t bytestemp = 0;
  }
  
  temperature[0] = 6; //gps alt code, see document
  temperature[1] = bytestemp;

  temperature[2] = crc_8(temperature, 2); //comuting CRC, passes only first 3 bytes
}

void presAltConverter(uint8_t (&presAlt) [4], float alt){

  uint16_t bytesalt = (uint16_t) alt;

  //if <0 just set to zero
  if(alt < 0){
    uint16_t bytesalt = 0;
  }
  
  presAlt[0] = 7; //gps alt code, see document
  presAlt[1] = (uint8_t) (bytesalt >> 8);
  presAlt[2] = (uint8_t) bytesalt;

  presAlt[3] = crc_8(presAlt, 3); //comuting CRC, passes only first 3 bytes
}

void baroPresConverter(uint8_t (&baroPres) [4], float pres){

  double presAsPercent = pres / 1200.0;

  uint16_t bytespres = (uint16_t) (presAsPercent * 65535.0);  // convert from HPa to 0-1 representing 0-120KPa

  //if <0 just set to zero
  if(pres < 0){
    uint16_t bytespres = 0;
  }
  
  baroPres[0] = 8; //gps alt code, see document
  baroPres[1] = (uint8_t) (bytespres >> 8);
  baroPres[2] = (uint8_t) bytespres;

  baroPres[3] = crc_8(baroPres, 3); //comuting CRC, passes only first 3 bytes
}

void pitotPresConverter(uint8_t (&pitotPres) [4], float pres){

  double presAsPercent = pres / 6894.757;

  uint16_t bytespres = (uint16_t) (abs(presAsPercent) * 32767.0);  // convert from HPa to 0-1 representing 0-120KPa

  //if <0 just set to zero
  if(presAsPercent < 0){
    bytespres = ~bytespres + 1;
  }
  
  pitotPres[0] = 9; //gps alt code, see document
  pitotPres[1] = (uint8_t) (bytespres >> 8);
  pitotPres[2] = (uint8_t) bytespres;

  pitotPres[3] = crc_8(pitotPres, 3); //comuting CRC, passes only first 3 bytes
}

void pitotSpeedConverter(uint8_t (&pitotSpeed) [4], float speed){

  uint16_t bytesspeed = (uint16_t) (speed * 100.0);  // convert from m/s to cm/s

  if(speed < 0){
    uint16_t bytesspeed = 0;
  }

  pitotSpeed[0] = 10;
  pitotSpeed[1] = (uint8_t) (bytesspeed >> 8);
  pitotSpeed[2] = (uint8_t) bytesspeed;

  pitotSpeed[3] = crc_8(pitotSpeed, 3);
}

void orientXConverter(uint8_t (&orientX) [4], float orient){ // 0 to 360

  double orientAsPercent = orient / 360.0;

  uint16_t bytesorient = (uint16_t) (orientAsPercent * 65535.0);

  if(orient < 0){
    uint16_t bytesorient = 0;
  }

  orientX[0] = 11;
  orientX[1] = (uint8_t) (bytesorient >> 8);
  orientX[2] = (uint8_t) bytesorient;

  orientX[3] = crc_8(orientX, 3);  
}

void orientYConverter(uint8_t (&orientY) [4], float orient){ // -90 to 90

  double orientAsPercent = orient / 90.0;

  uint16_t bytesorient = (uint16_t) (abs(orientAsPercent) * 32767.0);

  if(orientAsPercent < 0){
    bytesorient = ~bytesorient + 1;
  }

  orientY[0] = 12;
  orientY[1] = (uint8_t) (bytesorient >> 8);
  orientY[2] = (uint8_t) bytesorient;

  orientY[3] = crc_8(orientY, 3);  
}

void orientZConverter(uint8_t (&orientZ) [4], float orient){ // -90 to 90

  double orientAsPercent = orient / 180.0;

  uint16_t bytesorient = (uint16_t) (abs(orientAsPercent) * 32767.0);

  if(orientAsPercent < 0){
    bytesorient = ~bytesorient + 1;
  }

  orientZ[0] = 13;
  orientZ[1] = (uint8_t) (bytesorient >> 8);
  orientZ[2] = (uint8_t) bytesorient;

  orientZ[3] = crc_8(orientZ, 3);  
}

// These three will be the same for linear acceleration, combine into one?
void accelXConverter(uint8_t (&accelX) [4], float accel){ // -16g to 16g

  double accelAsPercent = accel / (16.0 * 9.80665); 

  uint16_t bytesaccel = (uint16_t) (abs(accelAsPercent) * 32767.0);

  if(accelAsPercent < 0){
    bytesaccel = ~bytesaccel + 1;
  }

  accelX[0] = 14;
  accelX[1] = (uint8_t) (bytesaccel >> 8);
  accelX[2] = (uint8_t) bytesaccel;

  accelX[3] = crc_8(accelX, 3);  
}

void accelYConverter(uint8_t (&accelY) [4], float accel){ // -16g to 16g

  double accelAsPercent = accel / (16.0 * 9.80665); 

  uint16_t bytesaccel = (uint16_t) (abs(accelAsPercent) * 32767.0);

  if(accelAsPercent < 0){
    bytesaccel = ~bytesaccel + 1;
  }

  accelY[0] = 15;
  accelY[1] = (uint8_t) (bytesaccel >> 8);
  accelY[2] = (uint8_t) bytesaccel;

  accelY[3] = crc_8(accelY, 3);  
}

void accelZConverter(uint8_t (&accelZ) [4], float accel){ // -16g to 16g

  double accelAsPercent = accel / (16.0 * 9.80665); 

  uint16_t bytesaccel = (uint16_t) (abs(accelAsPercent) * 32767.0);

  if(accelAsPercent < 0){
    bytesaccel = ~bytesaccel + 1;
  }

  accelZ[0] = 16;
  accelZ[1] = (uint8_t) (bytesaccel >> 8);
  accelZ[2] = (uint8_t) bytesaccel;

  accelZ[3] = crc_8(accelZ, 3);  
}

void rotXConverter(uint8_t (&rotX) [4], float rot){ // -2000 to 2000 deg/s

  double rotAsPercent = rot / 2000.0; 

  uint16_t bytesrot = (uint16_t) (abs(rotAsPercent) * 32767.0);

  if(rotAsPercent < 0){
    bytesrot = ~bytesrot + 1;
  }

  rotX[0] = 17;
  rotX[1] = (uint8_t) (bytesrot >> 8);
  rotX[2] = (uint8_t) bytesrot;

  rotX[3] = crc_8(rotX, 3);  
}

void rotYConverter(uint8_t (&rotY) [4], float rot){ // -2000 to 2000 deg/s

  double rotAsPercent = rot / 2000.0; 

  uint16_t bytesrot = (uint16_t) (abs(rotAsPercent) * 32767.0);

  if(rotAsPercent < 0){
    bytesrot = ~bytesrot + 1;
  }

  rotY[0] = 18;
  rotY[1] = (uint8_t) (bytesrot >> 8);
  rotY[2] = (uint8_t) bytesrot;

  rotY[3] = crc_8(rotY, 3);  
}

void rotZConverter(uint8_t (&rotZ) [4], float rot){ // -2000 to 2000 deg/s

  double rotAsPercent = rot / 2000.0; 

  uint16_t bytesrot = (uint16_t) (abs(rotAsPercent) * 32767.0);

  if(rotAsPercent < 0){
    bytesrot = ~bytesrot + 1;
  }

  rotZ[0] = 19;
  rotZ[1] = (uint8_t) (bytesrot >> 8);
  rotZ[2] = (uint8_t) bytesrot;

  rotZ[3] = crc_8(rotZ, 3);  
}

void magXConverter(uint8_t (&magX) [4], float mag){ // -1300 to 1300 uT

  double magAsPercent = mag / 1300.0; 

  uint16_t bytesmag = (uint16_t) (abs(magAsPercent) * 32767.0);

  if(magAsPercent < 0){
    bytesmag = ~bytesmag + 1;
  }

  magX[0] = 20;
  magX[1] = (uint8_t) (bytesmag >> 8);
  magX[2] = (uint8_t) bytesmag;

  magX[3] = crc_8(magX, 3);  
}

void magYConverter(uint8_t (&magY) [4], float mag){ // -1300 to 1300 uT

  double magAsPercent = mag / 1300.0; 

  uint16_t bytesmag = (uint16_t) (abs(magAsPercent) * 32767.0);

  if(magAsPercent < 0){
    bytesmag = ~bytesmag + 1;
  }

  magY[0] = 21;
  magY[1] = (uint8_t) (bytesmag >> 8);
  magY[2] = (uint8_t) bytesmag;

  magY[3] = crc_8(magY, 3);  
}

void magZConverter(uint8_t (&magZ) [4], float mag){ // -2500 to 2500 uT

  double magAsPercent = mag / 2500.0; 

  uint16_t bytesmag = (uint16_t) (abs(magAsPercent) * 32767.0);

  if(magAsPercent < 0){
    bytesmag = ~bytesmag + 1;
  }

  magZ[0] = 22;
  magZ[1] = (uint8_t) (bytesmag >> 8);
  magZ[2] = (uint8_t) bytesmag;

  magZ[3] = crc_8(magZ, 3);  
}

void fastServoConverter(uint8_t (&fastServo) [4], float fast){ // 0 to 1023

  double fastAsPercent = fast / 1023.0;

  uint16_t bytesfast = (uint16_t) (fastAsPercent * 65535.0);

  if(fastAsPercent < 0){
    uint16_t bytesfast = 0;
  }

  fastServo[0] = 23;
  fastServo[1] = (uint8_t) (bytesfast >> 8);
  fastServo[2] = (uint8_t) bytesfast;

  fastServo[3] = crc_8(fastServo, 3);  
}

void slowServoConverter(uint8_t (&slowServo) [4], float slow){ // 0 to 1023

  double slowAsPercent = slow / 1023.0;

  uint16_t bytesslow = (uint16_t) (slowAsPercent * 65535.0);

  if(slowAsPercent < 0){
    uint16_t bytesslow = 0;
  }

  slowServo[0] = 24;
  slowServo[1] = (uint8_t) (bytesslow >> 8);
  slowServo[2] = (uint8_t) bytesslow;

  slowServo[3] = crc_8(slowServo, 3);  
}

void elapsedTimeConverter(uint8_t (&elapsedTime) [5], uint32_t time){ // 0 to 1023

  uint32_t bytestime = time / 10;

  elapsedTime[0] = 25;
  elapsedTime[1] = (uint8_t) (bytestime >> 16);
  elapsedTime[2] = (uint8_t) (bytestime >> 8);
  elapsedTime[3] = (uint8_t) bytestime;

  elapsedTime[4] = crc_8(elapsedTime, 4);
}

void cpuTempConverter(uint8_t (&cpuTemp) [3], float temp){

  uint8_t bytestemp = (uint8_t) (temp + 80.0);

  //if <0 just set to zero
  if(bytestemp < 0){
    uint8_t bytestemp = 0;
  }
  
  cpuTemp[0] = 26; //gps alt code, see document
  cpuTemp[1] = bytestemp;

  cpuTemp[2] = crc_8(cpuTemp, 2); //comuting CRC, passes only first 3 bytes
}

void gpsTimeConverter(uint8_t (&GPSTime) [5], uint8_t hours, uint8_t minutes, uint8_t seconds){
  uint16_t secondsSinceMidnight = ((uint16_t) hours ) * 3600 + ((uint16_t) minutes ) * 60 + (uint16_t) seconds;

  GPSTime[0] = 27;
  GPSTime[1] = (uint8_t) (secondsSinceMidnight >> 16);
  GPSTime[2] = (uint8_t) (secondsSinceMidnight >> 8);
  GPSTime[3] = (uint8_t) secondsSinceMidnight;
  GPSTime[4] = crc_8(GPSTime, 4); //comuting CRC, passes only first 4 bytes
}

void gpsDateConverter(uint8_t (&GPSDate) [4], uint8_t day, uint8_t month, uint8_t year){
  uint16_t dateString =     ((uint16_t) day << 11);
  dateString = dateString + ((uint16_t) month << 7);
  dateString = dateString + (uint16_t) year + 30;


  GPSDate[0] = 28;
  GPSDate[1] = (uint8_t) (dateString >> 8);
  GPSDate[2] = (uint8_t) dateString;
  GPSDate[3] = crc_8(GPSDate, 3); //comuting CRC, passes only first 4 bytes
}

void linAccelXConverter(uint8_t (&linAccelX) [4], float accel){ // -16g to 16g

  double accelAsPercent = accel / (16.0 * 9.80665); 

  uint16_t bytesaccel = (uint16_t) (abs(accelAsPercent) * 32767.0);

  if(accelAsPercent < 0){
    bytesaccel = ~bytesaccel + 1;
  }

  linAccelX[0] = 29;
  linAccelX[1] = (uint8_t) (bytesaccel >> 8);
  linAccelX[2] = (uint8_t) bytesaccel;

  linAccelX[3] = crc_8(linAccelX, 3);  
}

void linAccelYConverter(uint8_t (&linAccelY) [4], float accel){ // -16g to 16g

  double accelAsPercent = accel / (16.0 * 9.80665); 

  uint16_t bytesaccel = (uint16_t) (abs(accelAsPercent) * 32767.0);

  if(accelAsPercent < 0){
    bytesaccel = ~bytesaccel + 1;
  }

  linAccelY[0] = 30;
  linAccelY[1] = (uint8_t) (bytesaccel >> 8);
  linAccelY[2] = (uint8_t) bytesaccel;

  linAccelY[3] = crc_8(linAccelY, 3);  
}

void linAccelZConverter(uint8_t (&linAccelZ) [4], float accel){ // -16g to 16g

  double accelAsPercent = accel / (16.0 * 9.80665); 

  uint16_t bytesaccel = (uint16_t) (abs(accelAsPercent) * 32767.0);

  if(accelAsPercent < 0){
    bytesaccel = ~bytesaccel + 1;
  }

  linAccelZ[0] = 31;
  linAccelZ[1] = (uint8_t) (bytesaccel >> 8);
  linAccelZ[2] = (uint8_t) bytesaccel;

  linAccelZ[3] = crc_8(linAccelZ, 3);  
}

void currentConverter(uint8_t (&current) [4], float currentmA){

  uint16_t bytesCurrent = (uint16_t) currentmA;
  
  current[0] = 33; //gps alt code, see document
  current[1] = (uint8_t) (bytesCurrent >> 8);
  current[2] = (uint8_t) bytesCurrent;

  current[3] = crc_8(current, 3); //comuting CRC, passes only first 3 bytes
}

void voltageConverter(uint8_t (&voltage) [4], float voltageV){

  uint16_t bytesVoltage = (uint16_t) (voltageV * 1000.0);
  
  voltage[0] = 34; //gps alt code, see document
  voltage[1] = (uint8_t) (bytesVoltage >> 8);
  voltage[2] = (uint8_t) bytesVoltage;

  voltage[3] = crc_8(voltage, 3); //comuting CRC, passes only first 3 bytes
}

void powerConverter(uint8_t (&power) [4], float powermW){

  uint16_t bytespower = (uint16_t) powermW;

  power[0] = 35;
  power[1] = (uint8_t) (bytespower >> 8);
  power[2] = (uint8_t) bytespower;

  power[3] = crc_8(power, 3);  
}

void batteryTempConverter(uint8_t (&batteryTemp) [3], float temp){

  uint8_t bytestemp = (uint8_t) (temp + 80.0);

  //if <0 just set to zero
  if(bytestemp < 0){
    uint8_t bytestemp = 0;
  }
  
  batteryTemp[0] = 36; //gps alt code, see document
  batteryTemp[1] = bytestemp;

  batteryTemp[2] = crc_8(batteryTemp, 2); //comuting CRC, passes only first 3 bytes
}

void gps2DataConverter(uint8_t (&gps2Packet) [10], float latit, float longit){
  double latAsPercent  = latit /90.0; //yeah ik its an intermediate step
  double lonAsPercent = longit/180.0;

  uint32_t latBytes = (uint32_t) (abs(latAsPercent) * 8388608.0); //2^23
  uint32_t lonBytes = (uint32_t) (abs(lonAsPercent) * 8388608.0);


  if(latAsPercent < 0){ //this is how u flip signs in twos complement
    latBytes = ~latBytes + 1;
  }
  if(lonAsPercent < 0){
    lonBytes = ~lonBytes + 1;
  }
  
  gps2Packet[0] = 37; //gps lat code
  gps2Packet[1] = (uint8_t) (latBytes >> 16);
  gps2Packet[2] = (uint8_t) (latBytes >> 8);
  gps2Packet[3] = (uint8_t) latBytes;

  gps2Packet[5] = 38; //gps long code
  gps2Packet[6] = (uint8_t) (lonBytes >> 16);
  gps2Packet[7] = (uint8_t) (lonBytes >> 8);
  gps2Packet[8] = (uint8_t) lonBytes;

  gps2Packet[4] = crc_8(gps2Packet, 4); //crc for lat

  // gpsPacket itself is only a pointer to the first item, the +5 tells it to point to the next item
  gps2Packet[9] = crc_8(gps2Packet + 5, 4);
};

void gps2SpeedConverter(uint8_t (&GPS2Speed) [4], float speed){

  uint16_t bytesspeed = (uint16_t) (speed * 463.0 / 9.0);  // convert from knots to cm/s

  if(speed < 0){
    uint16_t bytesspeed = 0;
  }

  GPS2Speed[0] = 39;
  GPS2Speed[1] = (uint8_t) (bytesspeed >> 8);
  GPS2Speed[2] = (uint8_t) bytesspeed;

  GPS2Speed[3] = crc_8(GPS2Speed, 3);
}

void gps2CourseConverter(uint8_t (&GPS2Course) [4], float course){

  double courseAsPercent = course / 360.0;

  uint16_t bytescourse = (uint16_t) (courseAsPercent * 65535.0);

  if(course < 0){
    uint16_t bytescourse = 0;
  }

  GPS2Course[0] = 40;
  GPS2Course[1] = (uint8_t) (bytescourse >> 8);
  GPS2Course[2] = (uint8_t) bytescourse;

  GPS2Course[3] = crc_8(GPS2Course, 3);  
}

//gps altitude, converts to two bytes as integer meters
//takes in double as input
void gps2AltConverter(uint8_t (&GPS2Alt) [4], float alt){

  uint16_t bytesalt = (uint16_t) alt;

  //if <0 just set to zero
  if(alt < 0){
    uint16_t bytesalt = 0;
  }
  
  GPS2Alt[0] = 41; //gps alt code, see document
  GPS2Alt[1] = (uint8_t) (bytesalt >> 8);
  GPS2Alt[2] = (uint8_t) bytesalt;

  GPS2Alt[3] = crc_8(GPS2Alt, 3); //comuting CRC, passes only first 3 bytes
}

void gps2TimeConverter(uint8_t (&GPS2Time) [5], uint8_t hours, uint8_t minutes, uint8_t seconds){
  uint16_t secondsSinceMidnight = ((uint16_t) hours ) * 3600 + ((uint16_t) minutes ) * 60 + (uint16_t) seconds;

  GPS2Time[0] = 42;
  GPS2Time[1] = (uint8_t) (secondsSinceMidnight >> 16);
  GPS2Time[2] = (uint8_t) (secondsSinceMidnight >> 8);
  GPS2Time[3] = (uint8_t) secondsSinceMidnight;
  GPS2Time[4] = crc_8(GPS2Time, 4); //comuting CRC, passes only first 4 bytes
}

void gps2DateConverter(uint8_t (&GPS2Date) [4], uint8_t day, uint8_t month, uint8_t year){
  uint16_t dateString =     ((uint16_t) day << 11);
  dateString = dateString + ((uint16_t) month << 7);
  dateString = dateString + (uint16_t) year + 30;


  GPS2Date[0] = 43;
  GPS2Date[1] = (uint8_t) (dateString >> 8);
  GPS2Date[2] = (uint8_t) dateString;
  GPS2Date[3] = crc_8(GPS2Date, 3); //comuting CRC, passes only first 4 bytes
}