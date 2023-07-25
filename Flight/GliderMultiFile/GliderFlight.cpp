#include <Arduino.h>
#include "dataLinkProtocolTest.h"

// GPS
#include <Adafruit_GPS.h>

//bmp388 stuff
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

//bno055
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

//take a guess
#include <Servo.h>
#include <SD.h>
#include <SPI.h>

// C++ program to convert string
// to char array using c_str()
#include <cstring>
#include <string>
#include <iostream>

#include <InternalTemperature.h>

// INA219
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

const int chipSelect = BUILTIN_SDCARD;
char* filename;

//bmp
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP3XX bmp;

// GPS
// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

//bno055
/* Set the delay between fresh samples */
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;
// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);



//diff pressure sensor stuff
/*
 Reads HSCDRRN010MDAA3 diff pressure sensor
*/

int sensorPin = A16;  // select the input pin for the potentiometer
int vrefPin = A9;
int sensorValue = 0;  // variable to store the value coming from the sensor
int selfVoltage = 0;
double VSupply = 3.3;
double Pmax = 10;
double Pmin = -10;
double density = 1.225;
double pressureOffset = 23;
double avgPressue = 0;

int sampleCount = 10;
double last10P[10];
double last10V[10];

//servos
int fastServoPin = 28;
int slowServoPin = 25;
int fastServoADC = A12;
int slowServoADC = A13;

Servo fastServo;
Servo slowServo;

// thermistor
const float vin = 3.3;
const float r1KO = 10;
float voltageDiv = 0;
float resKO = 0;

void setup() {
  Serial2.begin(115200);
  Wire.begin();

  //bmp
  if (!bmp.begin_I2C(0x77)) {  // hardware I2C mode, can pass in address & alt Wire
    //if (! bmp.begin_SPI(BMP_CS)) {  // hardware SPI mode
    //if (! bmp.begin_SPI(BMP_CS, BMP_SCK, BMP_MISO, BMP_MOSI)) {  // software SPI mode
    //Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    //while (1);
  }
  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);



  //bno055
  //Serial.println("Orientation Sensor Test"); Serial.println("");
  /* Initialise the sensor */
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    // Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    //while (1);
  }

  // GPS
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);  // 1 Hz update rate
  GPS.sendCommand(PMTK_API_SET_FIX_CTL_5HZ);  // 1 Hz reccommended
  GPS.sendCommand(PGCMD_ANTENNA);
  GPSSerial.println(PMTK_Q_RELEASE);

  //servo
  fastServo.attach(fastServoPin);
  slowServo.attach(slowServoPin);

  // heater
  pinMode(30, OUTPUT);
  digitalWrite(30, LOW);


  if (!ina219.begin()) {
    // Serial.println("Failed to find INA219 chip");
  }

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    // Serial.println("Card failed, or not present");
  }
  int i = 0;
  bool exists = true;
  String name;
  while (exists) {
    name = "datalog" + (String)i + ".txt";
    const int length = name.length();

    // declaring character array (+1 for null terminator)
    filename = new char[length + 1];

    // copying the contents of the
    // string to char array
    strcpy(filename, name.c_str());

    if (!SD.exists(filename)) exists = false;
    i++;
  }
}

float currentmA;
float loadVoltage;
float powermW;
float batteryTemp;
bool heater = false;
double bmpTempC;
double bmpPressureHPA;
double bmpBaroAltM;
double orientX;
double orientY;
double orientZ;
double gyroX;
double gyroY;
double gyroZ;
double linAccelX;
double linAccelY;
double linAccelZ;
double magneticX;
double magneticY;
double magneticZ;
double accelX;
double accelY;
double accelZ;
double dynamicPA;
double airspeedMS;
int hours;
int minutes;
int seconds;
int milliseconds;
int day;
int month;
int year;
int fix;
int quality;
bool hasFix;
float latitude;
float longitude;
float speed;
float angle;
float altitude;
int satellites;
int antenna;
int fastServoPos;
int slowServoPos;
int fastSetting = 5;
int slowSetting = 5;
float cpuTemp;

float lookup[241] = { 277.2, 263.6, 250.1, 236.8, 224, 211.5, 199.6, 188.1, 177.3, 167, 157.2, 148.1, 139.4, 131.3, 123.7, 116.6, 110, 103.7, 97.9, 92.5, 87.43, 82.79, 78.44, 74.36, 70.53,
                      66.92, 63.54, 60.34, 57.33, 54.5, 51.82, 49.28, 46.89, 44.62, 42.48, 40.45, 38.53, 36.7, 34.97, 33.33, 31.77, 30.25, 28.82, 27.45, 26.16, 24.94, 23.77, 22.67, 21.62, 20.63, 19.68, 18.78, 17.93, 17.12,
                      16.35, 15.62, 14.93, 14.26, 13.63, 13.04, 12.47, 11.92, 11.41, 10.91, 10.45, 10, 9.575, 9.17, 8.784, 8.416, 8.064, 7.73, 7.41, 7.106, 6.815, 6.538, 6.273, 6.02, 5.778, 5.548, 5.327, 5.117, 4.915,
                      4.723, 4.539, 4.363, 4.195, 4.034, 3.88, 3.733, 3.592, 3.457, 3.328, 3.204, 3.086, 2.972, 2.863, 2.759, 2.659, 2.564, 2.472, 2.384, 2.299, 2.218, 2.141, 2.066, 1.994, 1.926, 1.86, 1.796, 1.735, 1.677,
                      1.621, 1.567, 1.515, 1.465, 1.417, 1.371, 1.326, 1.284, 1.243, 1.203, 1.165, 1.128, 1.093, 1.059, 1.027, 0.9955, 0.9654, 0.9363, 0.9083, 0.8812, 0.855, 0.8297, 0.8052, 0.7816, 0.7587, 0.7366, 0.7152,
                      0.6945, 0.6744, 0.6558, 0.6376, 0.6199, 0.6026, 0.5858, 0.5694, 0.5535, 0.538, 0.5229, 0.5083, 0.4941, 0.4803, 0.4669, 0.4539, 0.4412, 0.429, 0.4171, 0.4055, 0.3944, 0.3835, 0.373, 0.3628, 0.353, 0.3434,
                      0.3341, 0.3253, 0.3167, 0.3083, 0.3002, 0.2924, 0.2848, 0.2774, 0.2702, 0.2633, 0.2565, 0.25, 0.2437, 0.2375, 0.2316, 0.2258, 0.2202, 0.2148, 0.2095, 0.2044, 0.1994, 0.1946, 0.19, 0.1855, 0.1811, 0.1769,
                      0.1728, 0.1688, 0.165, 0.1612, 0.1576, 0.1541, 0.1507, 0.1474, 0.1441, 0.141, 0.1379, 0.135, 0.1321, 0.1293, 0.1265, 0.1239, 0.1213, 0.1187, 0.1163, 0.1139, 0.1115, 0.1092, 0.107, 0.1048, 0.1027, 0.1006,
                      0.0986, 0.0966, 0.0947, 0.0928, 0.0909, 0.0891, 0.0873, 0.0856, 0.0839, 0.0822, 0.0806, 0.079, 0.0774, 0.0759, 0.0743, 0.0729, 0.0714, 0.07, 0.0686, 0.0672, 0.0658, 0.0645, 0.0631, 0.0619 };

float interpolate(float resistance) {
  int l = 0;
  int r = 240;
  int i = 0;
  while (l < r) {
    int m = l + (r - l) / 2;
    if (lookup[m] == resistance) {
      i = m;
      break;
    }
    if (lookup[m] > resistance) l = m + 1;
    else r = m - 1;
    i = l - 1;
  }
  return (i - 40.0) + (resistance - lookup[i]) / (lookup[i + 1] - lookup[i]);
}

//function included with bno055
void printEvent(sensors_event_t* event) {
  if (event->type == SENSOR_TYPE_ACCELEROMETER) {
    // Serial.print("\"accel\":\"");
    accelX = event->acceleration.x;
    accelY = event->acceleration.y;
    accelZ = event->acceleration.z;
  } else if (event->type == SENSOR_TYPE_ORIENTATION) {
    orientX = event->orientation.x;
    orientY = event->orientation.y;
    orientZ = event->orientation.z;
  } else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD) {
    // Serial.print("\"magnetic\":\"");
    magneticX = event->magnetic.x;
    magneticY = event->magnetic.y;
    magneticZ = event->magnetic.z;
  } else if (event->type == SENSOR_TYPE_GYROSCOPE) {
    // Serial.print("\"gyro\":\"");
    gyroX = event->gyro.x;
    gyroY = event->gyro.y;
    gyroZ = event->gyro.z;
  } else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION) {
    // Serial.print("\"accel\":\"");
    linAccelX = event->acceleration.x;
    linAccelY = event->acceleration.y;
    linAccelZ = event->acceleration.z;
  }
}

//averages array, for diff pressure
double avg(double dataArray[10]) {
  double avg = 0;
  for (int i = 0; i < 10; i++) {
    avg += dataArray[i];
  }
  return avg / 10;
}

void SFE_UBLOX_GNSS::processNMEA(char incoming) {
  nmea.process(incoming);
}

String toString() {
  String output = millis();

  output += " | \"currentmA\":\"";
  output += currentmA;
  output += "\", \"loadVoltage\":\"";
  output += loadVoltage;
  output += "\", \"powermW\":\"";
  output += powermW;
  output += "\", \"batteryTemp\":\"";
  output += batteryTemp;
  // output += "\", \"heaterOn\":\"";
  // if (isOn) { output += "on"; }
  // else { output += "off"; }
  output += "\", \"temperatureC\":\"";
  output += bmpTempC;
  output += "\", \"pressureHPA\":\"";
  output += bmpPressureHPA;
  output += "\", \"baroAltM\":\"";
  output += bmpBaroAltM;
  output += "\", \"orient\":\"x=";
  output += orientX;
  output += " y=";
  output += orientY;
  output += " z=";
  output += orientZ;
  output += "\", \"gyro\":\"x=";
  output += gyroX;
  output += " y=";
  output += gyroY;
  output += " z=";
  output += gyroZ;
  output += "\", \"linAccel\":\"x=";
  output += linAccelX;
  output += " y=";
  output += linAccelY;
  output += " z=";
  output += linAccelZ;
  output += "\", \"magnetic\":\"x=";
  output += magneticX;
  output += " y=";
  output += magneticY;
  output += " z=";
  output += magneticZ;
  output += "\", \"accel\":\"x=";
  output += accelX;
  output += " y=";
  output += accelY;
  output += " z=";
  output += accelZ;
  output += "\", \"dynamicPA\":\"";
  output += dynamicPA;
  output += "\", \"airspeedMS\":\"";
  output += airspeedMS;
  output += "\", \"Time\":\"";
  if (hours < 10) { output += '0'; }
  output += hours;
  output += ':';
  if (minutes < 10) { output += '0'; }
  output += minutes;
  output += ':';
  if (seconds < 10) { output += '0'; }
  output += seconds;
  output += '.';
  if (milliseconds < 10) {
    output += "00";
  } else if (milliseconds > 9 && milliseconds < 100) {
    output += "0";
  }
  output += milliseconds;
  output += "\", \"Date\":\"";
  output += day;
  output += "/";
  output += month;
  output += "/20";
  output += year;
  output += "\", \"Fix\":\"";
  output += fix;
  output += "\", \"quality\":\"";
  output += quality;
  if (hasFix) {
    output += "\", \"Location\":\"";
    output += String(latitude, 5);
    output += ", ";
    output += String(longitude, 5);
    output += "\", \"Speed(knots)\":\"";
    output += speed;
    output += "\", \"Angle\":\"";
    output += angle;
    output += "\", \"Altitude\":\"";
    output += altitude;
    output += "\", \"Satellites\":\"";
    output += satellites;
    output += "\", \"Antenna status\":\"";
    output += antenna;
  }
  output += "\", \"fastservopos\":\"";
  output += fastServoPos;
  output += "\", \"slowservopos\":\"";
  output += slowServoPos;
  output += "\", \"fastsetting\":";
  output += fastSetting;
  output += "\", \"slowsetting\":";
  output += slowSetting;
  output += "\", \"CPUTemp\":";
  output += cpuTemp;
  output += "\", \"Raw NMEA\":\"";
  output += GPS.lastNMEA();
  output.remove(output.length() - 2);
  output += "\"";
  return output;
}

void loop()  // run over and over again
{

  currentmA = ina219.getCurrent_mA();
  loadVoltage = ina219.getBusVoltage_V() + (ina219.getShuntVoltage_mV() / 1000);
  powermW = ina219.getPower_mW();

  voltageDiv = vin * analogRead(A14) / 1023;
  resKO = (r1KO * vin / voltageDiv) - r1KO;
  batteryTemp = interpolate(resKO);

  if (batteryTemp < 0.0 && !heater) {
    digitalWrite(30, LOW);
    heater = true;
  } else if (batteryTemp > 5.0 && heater) {
    digitalWrite(30, HIGH);
    heater = false;
  }

  if (bmp.performReading()) {
    bmpTempC = bmp.temperature;
    bmpPressureHPA = bmp.pressure / 100.0;
    bmpBaroAltM = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  }

  //bno055 starts here
  sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData;
  if (bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER)) printEvent(&orientationData);
  if (bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE)) printEvent(&angVelocityData);
  if (bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL)) printEvent(&linearAccelData);
  if (bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER)) printEvent(&magnetometerData);
  if (bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER)) printEvent(&accelerometerData);

  //diff pressure sensor
  for (int i = 0; i < sampleCount; i++) {
    // read the value from the sensor:
    sensorValue = analogRead(sensorPin);
    selfVoltage = analogRead(vrefPin);
    double Vout = (double)sensorValue / (double)selfVoltage;
    double pressuremBar = Pmin + (Vout - 0.1) * (Pmax - Pmin) / 0.80;
    double pressurePa = pressuremBar * 100;
    pressurePa += pressureOffset;
    last10P[i] = pressurePa;
    last10V[i] = Vout;
    delay(10);
  }

  avgPressue = avg(last10P);
  double speedms = sqrt(2.0 * abs(avgPressue) / density);
  double avgV = avg(last10V);

  //Serial.print("Vout: ");
  //Serial.println(avgV);
  dynamicPA = avgPressue;
  airspeedMS = speedms;

  char c = GPS.read();
  if (GPS.newNMEAreceived()) {
    if (GPS.parse(GPS.lastNMEA())) {
      hours = GPS.hour;
      minutes = GPS.minute;
      seconds = GPS.seconds;
      milliseconds = GPS.milliseconds;
      day = GPS.day;
      month = GPS.month;
      year = GPS.year;
      fix = GPS.fix;
      quality = GPS.fixquality;
      if (GPS.fix) {
        hasFix = true;
        latitude = (int)GPS.latitude / 100 + (GPS.latitude - 100 * ((int)GPS.latitude / 100)) * 1.0 / 60;
        longitude = (int)GPS.longitude / 100 + (GPS.longitude - 100 * ((int)GPS.longitude / 100)) * 1.0 / 60;
        if (GPS.lat == 'S') latitude = -latitude;
        if (GPS.lon == 'W') longitude = -longitude;
        speed = GPS.speed;
        angle = GPS.angle;
        altitude = GPS.altitude;
        satellites = GPS.satellites;
        antenna = GPS.antenna;
      } 
      else {
        hasFix = false;
      }
    }
  }

  //servo
  fastServoPos = analogRead(fastServoADC);
  slowServoPos = analogRead(slowServoADC);

  if ((millis() / 1000) % 2 == 1 && fastSetting == 175) {
    fastSetting = 5;
    fastServo.write(5);
  } else if ((millis() / 1000) % 2 == 0 && fastSetting == 5) {
    fastSetting = 175;
    fastServo.write(175);
  }
  if ((millis() / 300000) % 2 == 1 && slowSetting == 150) {
    slowServo.write(5);
    slowSetting = 5;
  } else if ((millis() / 300000) % 2 == 0 && slowSetting == 5) {
    slowServo.write(150);
    slowSetting = 150;
  }

  cpuTemp = InternalTemperature.readTemperatureC();

  uint8_t gpsPacket[10];
  gpsDataConverter(gpsPacket, latitude, longitude);

  uint8_t gpsAltPacket[4];
  gpsAltConverter(gpsAltPacket, altitude);

  uint8_t gpsSpeedPacket[4];
  gpsSpeedConverter(gpsSpeedPacket, speed);

  uint8_t gpsAnglePacket[4];
  gpsAngleConverter(gpsAnglePacket, angle);

  uint8_t tempPacket[3];
  tempConverter(tempPacket, bmpTempC);

  uint8_t presAltPacket[4];
  presAltConverter(presAltPacket, bmpBaroAltM);

  uint8_t baroPresPacket[4];
  baroPresConverter(baroPresPacket, bmpPressureHPA);

  uint8_t pitotPresPacket[4];
  pitotPresConverter(pitotPresPacket, dynamicPA);

  uint8_t pitotSpeedPacket[4];
  pitotSpeedConverter(pitotSpeedPacket, airspeedMS);

  uint8_t orientXPacket[4];
  orientXConverter(orientXPacket, orientX);

  uint8_t orientYPacket[4];
  orientYConverter(orientYPacket, orientY);

  uint8_t orientZPacket[4];
  orientZConverter(orientZPacket, orientZ);

  uint8_t accelXPacket[4];
  accelXConverter(accelXPacket, accelX);

  uint8_t accelYPacket[4];
  accelYConverter(accelYPacket, accelY);

  uint8_t accelZPacket[4];
  accelZConverter(accelZPacket, accelZ);

  uint8_t rotXPacket[4];
  rotXConverter(rotXPacket, gyroX);

  uint8_t rotYPacket[4];
  rotYConverter(rotYPacket, gyroY);

  uint8_t rotZPacket[4];
  rotZConverter(rotZPacket, gyroZ);

  uint8_t magXPacket[4];
  magXConverter(magXPacket, magneticX);

  uint8_t magYPacket[4];
  magYConverter(magYPacket, magneticY);

  uint8_t magZPacket[4];
  magZConverter(magZPacket, magneticZ);

  uint8_t fastServoPacket[4];
  fastServoConverter(fastServoPacket, fastServoPos);

  uint8_t slowServoPacket[4];
  slowServoConverter(slowServoPacket, slowServoPos);

  uint8_t elapsedTimePacket[5];
  elapsedTimeConverter(elapsedTimePacket, millis());

  uint8_t cpuTempPacket[3];
  cpuTempConverter(cpuTempPacket, cpuTemp);

  uint8_t gpsTimePacket[5];
  gpsTimeConverter(gpsTimePacket, hours, minutes, seconds);

  uint8_t gpsDatePacket[4];
  gpsDateConverter(gpsDatePacket, day, month, year);

  uint8_t linAccelXPacket[4];
  linAccelXConverter(linAccelXPacket, linAccelX);

  uint8_t linAccelYPacket[4];
  linAccelYConverter(linAccelYPacket, linAccelY);

  uint8_t linAccelZPacket[4];
  linAccelZConverter(linAccelZPacket, linAccelZ);

  uint8_t currentPacket[4];
  currentConverter(currentPacket, currentmA);

  uint8_t voltagePacket[4];
  voltageConverter(voltagePacket, loadVoltage);

  uint8_t powerPacket[4];
  powerConverter(powerPacket, powermW);

  uint8_t batteryTempPacket[3];
  batteryTempConverter(batteryTempPacket, batteryTemp);

  uint8_t gps2Packet[10];
  gps2DataConverter(gps2Packet, latitude2, longitude2);

  uint8_t gps2AltPacket[4];
  gps2AltConverter(gps2AltPacket, altitude2);

  uint8_t gps2SpeedPacket[4];
  gps2SpeedConverter(gps2SpeedPacket, speed2);

  uint8_t gps2CoursePacket[4];
  gps2CourseConverter(gps2CoursePacket, course2);

  uint8_t gps2TimePacket[5];
  gps2TimeConverter(gps2TimePacket, hours2, minutes2, (uint8_t)seconds2);

  uint8_t gps2DatePacket[4];
  gps2DateConverter(gps2DatePacket, days2, months2, (uint8_t)(years2 - 2000));

  Serial2.write(gpsPacket, 10);
  Serial2.write(gpsAltPacket, 4);
  Serial2.write(gpsSpeedPacket, 4);
  Serial2.write(gpsAnglePacket, 4);
  Serial2.write(tempPacket, 3);
  Serial2.write(presAltPacket, 4);
  Serial2.write(baroPresPacket, 4);
  Serial2.write(pitotPresPacket, 4);
  Serial2.write(pitotSpeedPacket, 4);
  Serial2.write(orientXPacket, 4);
  Serial2.write(orientYPacket, 4);
  Serial2.write(orientZPacket, 4);
  Serial2.write(accelXPacket, 4);
  Serial2.write(accelYPacket, 4);
  Serial2.write(accelZPacket, 4);
  Serial2.write(rotXPacket, 4);
  Serial2.write(rotYPacket, 4);
  Serial2.write(rotZPacket, 4);
  Serial2.write(magXPacket, 4);
  Serial2.write(magYPacket, 4);
  Serial2.write(magZPacket, 4);
  Serial2.write(fastServoPacket, 4);
  Serial2.write(slowServoPacket, 4);
  Serial2.write(elapsedTimePacket, 5);
  Serial2.write(cpuTempPacket, 3);
  Serial2.write(gpsTimePacket, 5);
  Serial2.write(gpsDatePacket, 4);
  Serial2.write(linAccelXPacket, 4);
  Serial2.write(linAccelYPacket, 4);
  Serial2.write(linAccelZPacket, 4);
  Serial2.write(currentPacket, 4);
  Serial2.write(voltagePacket, 4);
  Serial2.write(powerPacket, 4);
  Serial2.write(batteryTempPacket, 3);
  Serial2.write(gps2Packet, 10);
  Serial2.write(gps2AltPacket, 4);
  Serial2.write(gps2SpeedPacket, 4);
  Serial2.write(gps2CoursePacket, 4);
  Serial2.write(gps2TimePacket, 5);
  Serial2.write(gps2DatePacket, 4);

  // open the file.
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    String output = toString();
    dataFile.println(output);
    dataFile.close();
    // print to the serial port too:
    // Serial.println(output);
  } else {
    // if the file isn't open, pop up an error:
    // Serial.println("error opening datalog.txt");
  }
}