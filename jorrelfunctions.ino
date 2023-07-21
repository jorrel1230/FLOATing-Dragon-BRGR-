#include "InterpolationLib.h"


float buffer[3];

const int len = 94;
double alt_arr[94] = {5311, 5479, 6000, 6719, 7001, 7536, 7998, 8999, 10003, 10390, 12001, 12316, 13700, 13999, 15052, 15141, 15367, 
                15823, 16000, 18402, 19258, 19311, 20000, 21551, 24868, 25000, 29019, 29580, 30000, 31627, 32637, 32998, 35728, 
                37001, 38671, 40000, 40485, 41000, 42447, 43969, 44829, 45000, 46391, 46525, 47998, 49878, 50000, 50360, 51000, 
                51886, 52601, 54494, 56000, 56200, 57467, 57998, 59786, 60000, 60367, 61463, 62066, 63192, 63999, 66000, 66112, 
                66965, 67998, 69209, 70000, 71000, 72001, 72998, 76000, 76358, 77001, 77998, 78805, 80000, 80383, 85000, 86000, 
                87237, 89553, 91138, 92001, 93861, 95000, 97998, 98566, 100000, 103287, 106286, 107001, 110000};

double wind_dir[94] = {315, 330, 15, 350, 340, 343, 345, 350, 321, 310, 225, 223, 212, 210, 231, 233, 237, 246, 250, 257, 260, 261, 
                  270, 262, 245, 245, 253, 254, 255, 250, 246, 245, 235, 225, 233, 240, 240, 230, 230, 230, 230, 230, 240, 240, 
                  240, 221, 220, 225, 235, 246, 256, 280, 220, 220, 220, 220, 229, 230, 224, 206, 174, 108, 60, 110, 111, 115, 
                  290, 334, 55, 90, 85, 115, 85, 74, 55, 30, 40, 65, 71, 70, 90, 90, 86, 88, 90, 88, 120, 95, 86, 65, 83, 86, 85, 100};

double wind_spd[94] = {2, 2, 3, 8, 10, 12, 14, 7, 5, 4, 7, 9, 15, 17, 18, 18, 18, 19, 19, 17, 16, 16, 20, 20, 19, 20, 29, 30, 31, 34, 
                  38, 40, 40, 43, 41, 40, 38, 38, 47, 57, 63, 63, 55, 55, 53, 50, 50, 50, 51, 44, 38, 22, 14, 15, 18, 20, 12, 11, 
                  11, 10, 9, 5, 3, 12, 12, 16, 1, 10, 17, 14, 11, 20, 5, 7, 12, 14, 21, 31, 29, 27, 22, 27, 29, 26, 24, 38, 33, 
                  22, 24, 30, 29, 30, 30, 30};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  /*
  float air_dens = 1.225;

  get_bank_info(buffer, 20.0, air_dens);
  Serial.println("Radius, Angle, Load Factor: ");
  Serial.println(buffer[0]);
  Serial.println(buffer[1]);
  Serial.println(buffer[2]);

  lift_drag(buffer, air_dens, 40, 0);
  Serial.println("Lift, Drag: ");
  Serial.println(buffer[0]);
  Serial.println(buffer[1]);
  */

  // Serial.println(travel_dir(25, -57, 65, -17));

  while (!Serial) { ; }
  Serial.begin(9600);

  Serial.println(windaccounted_bearing(9000, 5, 50));

}

void loop() {
  // put your main code here, to run repeatedly:
}

// Calculate Bank Angle and Turning Radius; return in array [radius, angle]
// Note: Angle (Radians) Radius (Meters)
//       Airspeed (m/s)  Air Dens (kg/m^3)
void get_bank_info(float buffer[], float horizontal_airspeed, float air_density) {
  // Plane Specs
  float weight = 20;           // newtons
  float mass = weight / 9.81;  // kg
  float coef_lift = 0.75;      // Assumed Coef Of Lift used as base case
  float wing_area = 0.13575;   // meters squared

  // Calcs
  float lift = 1.0 / 2 * wing_area * air_density * (horizontal_airspeed * horizontal_airspeed) * coef_lift;


  float temp = sqrt(pow((lift), 2) - weight * weight);
  float radius = (mass * horizontal_airspeed * horizontal_airspeed) / temp;
  float bank_angle = atan(temp / weight);
  float load_factor = lift / weight;


  buffer[0] = radius;
  buffer[1] = bank_angle;
  buffer[2] = load_factor;
}

// Returns to forces of lift and drag depending on speed, angle and air dens
void lift_drag(float buffer[], float air_dens, float air_speed, float angle_of_attack) {
  // Plane Specs
  float wing_area = 0.13575;  // meters squared
  float epsilon = 0.083766;   // Drag from Lift coef

  // Aero Coefficients (@ angles of attack)
  float lift_coef_0 = 0.171802;
  float drag_coef_0 = 0.05499;
  float lift_coef_a = 3.78299;  // rad^-1

  float lift_coef = lift_coef_0 + angle_of_attack * lift_coef_a;
  float drag_coef = drag_coef_0 + epsilon * pow(lift_coef, 2);

  float dynamic_pressure = (1.0 / 2.0) * air_dens * pow(air_speed, 2);

  float lift_force = lift_coef * dynamic_pressure * wing_area;
  float drag_force = drag_coef * dynamic_pressure * wing_area;

  buffer[0] = lift_force;
  buffer[1] = drag_force;
}

// Returns the direction the glider must TRAVEL in to reach target
// Takes gps coords of glider and target location
float travel_dir(float lat1, float long1, float lat2, float long2) {
  // Convert latitude and longitude to radians
  lat1 = radians(lat1);
  long1 = radians(long1);
  lat2 = radians(lat2);
  long2 = radians(long2);

  // Calculate the bearing
  float bearing = atan2(sin(long2 - long1) * cos(lat2), cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(long2 - long1));

  // Convert the bearing to degrees
  bearing = degrees(bearing);

  // Make sure the bearing is positive
  if (bearing < 0) {
    bearing += 360;
  };

  return bearing;
}

// Calculates bearing Based on Wind Dir, and "Goal" Bearing
// Goal Bearing - The direction that the glider must travel in
// returns direction the glider must "face" to travel in direction of goal
// Takes in data from a sounding file, uses it to calculate for a direction for the glider to maintain to account for windspeed
float windaccounted_bearing(float altitude, float air_speed, float goal_bearing) {
    float predicted_wind_speed = Interpolation::ConstrainedSpline(alt_arr, wind_spd, len, altitude * 3.281);
    float predicted_wind_dir = Interpolation::ConstrainedSpline(alt_arr, wind_dir, len, altitude * 3.281);

    float bearing = (180.0 / PI * (asin((-predicted_wind_speed / air_speed) * sin((predicted_wind_dir - goal_bearing) * PI / 180.0)))) + goal_bearing;
    if (bearing != bearing) {
      Serial.println("Windspeed too high to move towards target");
    }

    return bearing;
}

