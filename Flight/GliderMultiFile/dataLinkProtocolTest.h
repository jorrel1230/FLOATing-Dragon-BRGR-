void gpsDataConverter(uint8_t (&gpsPacket) [10], float latit, float longit);

void gpsAltConverter(uint8_t (&GPSAlt) [4], float alt);

void gpsSpeedConverter(uint8_t (&GPSSpeed) [4], float speed);

void gpsAngleConverter(uint8_t (&GPSAngle) [4], float angle);

void tempConverter(uint8_t (&temperature) [3], float temp);

void presAltConverter(uint8_t (&presAlt) [4], float alt);

void baroPresConverter(uint8_t (&baroPres) [4], float pres);

void pitotPresConverter(uint8_t (&pitotPres) [4], float pres);

void pitotSpeedConverter(uint8_t (&pitotSpeed) [4], float speed);

void orientXConverter(uint8_t (&orientX) [4], float orient);

void orientYConverter(uint8_t (&orientY) [4], float orient);

void orientZConverter(uint8_t (&orientZ) [4], float orient);

void accelXConverter(uint8_t (&accelX) [4], float accel);

void accelYConverter(uint8_t (&accelY) [4], float accel);

void accelZConverter(uint8_t (&accelZ) [4], float accel);

void rotXConverter(uint8_t (&rotX) [4], float rot);

void rotYConverter(uint8_t (&rotY) [4], float rot);

void rotZConverter(uint8_t (&rotZ) [4], float rot);

void magXConverter(uint8_t (&magX) [4], float mag);

void magYConverter(uint8_t (&magY) [4], float mag);

void magZConverter(uint8_t (&magZ) [4], float mag);

void fastServoConverter(uint8_t (&fastServo) [4], float fast);

void slowServoConverter(uint8_t (&slowServo) [4], float slow);

void elapsedTimeConverter(uint8_t (&elapsedTime) [5], uint32_t time);

void cpuTempConverter(uint8_t (&cpuTemp) [3], float temp);

void gpsTimeConverter(uint8_t (&GPSTime) [5], uint8_t hours, uint8_t minutes, uint8_t seconds);

void gpsDateConverter(uint8_t (&GPSDate) [4], uint8_t day, uint8_t month, uint8_t year);

void linAccelXConverter(uint8_t (&linAccelX) [4], float accel);

void linAccelYConverter(uint8_t (&linAccelY) [4], float accel);

void linAccelZConverter(uint8_t (&linAccelZ) [4], float accel);

void currentConverter(uint8_t (&current) [4], float currentmA);

void voltageConverter(uint8_t (&voltage) [4], float voltageV);

void powerConverter(uint8_t (&power) [4], float powermW);

void batteryTempConverter(uint8_t (&batteryTemp) [3], float temp);

void gps2DataConverter(uint8_t (&gps2Packet) [10], float latit, float longit);

void gps2SpeedConverter(uint8_t (&GPS2Speed) [4], float speed);

void gps2CourseConverter(uint8_t (&GPS2Course) [4], float course);

void gps2AltConverter(uint8_t (&GPS2Alt) [4], float alt);

void gps2TimeConverter(uint8_t (&GPS2Time) [5], uint8_t hours, uint8_t minutes, uint8_t seconds);

void gps2DateConverter(uint8_t (&GPS2Date) [4], uint8_t day, uint8_t month, uint8_t year);