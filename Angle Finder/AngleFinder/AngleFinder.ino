
#include <math.h>
#include <M5StickCPlus.h>

#ifdef __cplusplus
extern "C" {
#include "IMU_6886.h"
}
#endif

IMU_6886 imu6886;

float accX = 0;
float accY = 0;
float accZ = 0;

float gyroX = 0;
float gyroY = 0;
float gyroZ = 0;

float pitch = 0.0F;
float roll  = 0.0F;
float yaw   = 0.0F;

//  Use the following global variables and access functions
//  to calibrate the acceleration sensor
float    base_x_accel;
float    base_y_accel;
float    base_z_accel;

float    base_x_gyro;
float    base_y_gyro;
float    base_z_gyro;

// Use the following global variables and access functions to help store the overall
// rotation angle of the sensor
unsigned long last_read_time;
float         last_x_angle;  // These are the filtered angles
float         last_y_angle;
float         last_z_angle;  
float         last_gyro_x_angle;  // Store the gyro angles to compare drift
float         last_gyro_y_angle;
float         last_gyro_z_angle;

double theta = 0, last_theta = 0;
double phi = 0, last_phi = 0;
double alpha = 0.2;

float temp = 0;

void set_last_read_angle_data(unsigned long time, float x, float y, float z, float x_gyro, float y_gyro, float z_gyro) {
  last_read_time = time;
  last_x_angle = x;
  last_y_angle = y;
  last_z_angle = z;
  last_gyro_x_angle = x_gyro;
  last_gyro_y_angle = y_gyro;
  last_gyro_z_angle = z_gyro;
}

inline unsigned long get_last_time() {return last_read_time;}
inline float get_last_x_angle() {return last_x_angle;}
inline float get_last_y_angle() {return last_y_angle;}
inline float get_last_z_angle() {return last_z_angle;}
inline float get_last_gyro_x_angle() {return last_gyro_x_angle;}
inline float get_last_gyro_y_angle() {return last_gyro_y_angle;}
inline float get_last_gyro_z_angle() {return last_gyro_z_angle;}

void calibrate_sensors() {
  int                   num_readings = 500;
  float                 x_accel = 0;
  float                 y_accel = 0;
  float                 z_accel = 0;
  float                 x_gyro = 0;
  float                 y_gyro = 0;
  float                 z_gyro = 0;
    set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 25);
    M5.Lcd.println("Starting Calibration");
    delay(2000);
  //Serial.println("Starting Calibration");
  // Discard the first set of values read from the IMU
    M5.Imu.getAccelData(&accX, &accY, &accZ);
    M5.Imu.getGyroData(&gyroY, &gyroX, &gyroZ);

  // Read and average the raw values from the IMU
  for (int i = 0; i < num_readings; i++) {
    M5.Imu.getAccelData(&accX, &accY, &accZ);
    M5.Imu.getGyroData(&gyroY, &gyroX, &gyroZ);

    x_accel += accX;
    y_accel += accY;
    z_accel += accZ;
    x_gyro += gyroX;
    y_gyro += gyroY;
    z_gyro += gyroZ;
    delay(10);
  }
  x_accel /= num_readings;
  y_accel /= num_readings;
  z_accel /= num_readings;
  x_gyro /= num_readings;
  y_gyro /= num_readings;
  z_gyro /= num_readings;
  
  // Store the raw calibration values globally
  base_x_accel = x_accel;
  base_y_accel = y_accel;
  base_z_accel = z_accel;
  base_x_gyro = x_gyro;
  base_y_gyro = y_gyro;

  base_z_gyro = z_gyro;

  M5.Lcd.printf(" %3.1f %3.1f %3.1f %3.1f", base_x_accel, base_y_accel, base_x_gyro, base_y_gyro);
  //imu6886.Init(32, 33);
  delay(1000);    
  //Serial.println("Finishing Calibration");
}
void draw_battery() {
  int width = 70;
  int height = 15;



  M5.Lcd.drawRect(15, M5.Lcd.height() - height -2, width, height, WHITE);
  M5.Lcd.fillRect(15 + width, M5.Lcd.height() - height + 2, 4, height - 6,  WHITE);


  float VBat = M5.Axp.GetBatVoltage();
  M5.Lcd.fillRect(15 + 2, M5.Lcd.height() - height, width -4, height - 4, BLACK);
  VBat = (VBat - 3.4)/0.8;
  if (VBat > 0.5) {
  M5.Lcd.fillRect(15+2, M5.Lcd.height() - height, (width - 4)*VBat, height -4, GREEN);
} else if (VBat > 0.3) {
  M5.Lcd.fillRect(15+2, M5.Lcd.height() - height, (width - 4)*VBat, height -4, ORANGE);
}
else {
  M5.Lcd.fillRect(15+2, M5.Lcd.height() - height, (width - 4)*VBat, height -4, RED);
}
}

void draw_green_triangle() {
    M5.Lcd.fillTriangle(0,17,
                        M5.Lcd.width(), 17,
                        M5.Lcd.width()/2, 2,
                        GREEN);
M5.Lcd.fillRect(0, 17, M5.Lcd.width(),6,  GREEN);
}

void setup() {
    
    M5.begin();
    // put your setup code here, to run once:
    //M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(40, 5);
    M5.Lcd.println("ANGLE FINDER");
    delay(500);
    /*M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(50, 30);
    M5.Lcd.println("  X      Y       Z");
    M5.Lcd.setCursor(45, 90);
    M5.Lcd.println("  Pitch   Roll    Yaw");*/

    M5.Imu.Init();
    M5.Imu.SetGyroFsr(M5.Imu.GFS_250DPS);
    M5.Imu.SetAccelFsr(M5.Imu.AFS_2G);
    calibrate_sensors(); 
    set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);

    M5.Lcd.setCursor(2, 65);
    M5.Lcd.setTextSize(2);

    M5.Lcd.fillScreen(BLACK);
    draw_green_triangle();
    draw_battery();
//M5.Lcd.width() - 1), random(M5.Lcd.height()


  // Battery


  //M5.Lcd.fillRect(17, M5.Lcd.height() - 14, 0.7 * (M5.Lcd.width() - 13 - ) , M5.Lcd.height()-1, WHITE);

     //checkAXP192();    //M5.Lcd.setTextSize(2);
    //M5.Lcd.setCursor(40, 5);
    //M5.Lcd.println("MPU6886 TEST");
        
}

bool set_offset = false;
float angle_offset = 0;
unsigned long t_offset = 0;

void loop() {
    // put your main code here, to run repeatedly:
    //imu6886.getGyroData(&gyroX, &gyroY, &gyroZ);
    //imu6886.getAccelData(&accX, &accY, &accZ);
    //imu6886.getTempData(&temp);
    //imu6886.getAhrsData(&pitch, &roll, &yaw);
  
  


  unsigned long t_now = millis();
  
//  checkAXP192()
    M5.update();  // Read the press state of the key.  读取按键 A, B, C 的状态
    if (M5.BtnB.wasReleasefor(
                   700)) {  // The button B is pressed for 700ms. 按键 B 按下
                            // 700ms,屏幕清空
        M5.Lcd.fillScreen(
            BLACK);  // Set BLACK to the background color.  将黑色设置为底色
        M5.Lcd.setCursor(40, 5);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println("RESET CALIB.");
        delay(1000);
        calibrate_sensors(); 
        set_last_read_angle_data(millis(), 0, 0, 0, 0, 0, 0);
        M5.Lcd.fillScreen(
            BLACK);  // Set BLACK to the background color.  将黑色设置为底色
        draw_green_triangle();
        draw_battery();
    } else if (M5.BtnB.wasReleased()) {  // If the button B is pressed. 如果按键
                                         // B 被按下，
        set_offset = true;
        t_offset = t_now;
    }
    M5.update();
    //if (t_now % 1000 < 50) { draw_battery();}

    M5.Imu.getGyroData(&gyroX, &gyroY, &gyroZ);
    M5.Imu.getAccelData(&accX, &accY, &accZ);
    M5.Imu.getTempData(&temp);
    //M5.Imu.getAhrsData(&pitch, &roll, &yaw);

    

        
    float gyro_x = (gyroX - base_x_gyro) ;
    float gyro_y = (gyroY - base_y_gyro) ;
    float gyro_z = (gyroZ - base_z_gyro) ;

    float RADIANS_TO_DEGREES = 180/3.14159;

    float accel_angle_x = atan(
                                -1*accX/sqrt(
                                              pow(accY,2) + pow(accZ,2)
                                              )
                                              )*RADIANS_TO_DEGREES;
    float accel_angle_y = atan(accY/sqrt(pow(accX,2) + pow(accZ,2)))*RADIANS_TO_DEGREES;
  
    float dt =(t_now - get_last_time())/1000.0;
    float gyro_angle_x = gyro_x*dt + get_last_x_angle();
    float gyro_angle_y = gyro_y*dt + get_last_y_angle();
    float gyro_angle_z = gyro_z*dt + get_last_z_angle();
    
    // Compute the drifting gyro angles
    float unfiltered_gyro_angle_x = gyro_x*dt + get_last_gyro_x_angle();
    float unfiltered_gyro_angle_y = gyro_y*dt + get_last_gyro_y_angle();
    float unfiltered_gyro_angle_z = gyro_z*dt + get_last_gyro_z_angle();

    // Apply the complementary filter to figure out the change in angle - choice of alpha is
    // estimated now.  Alpha depends on the sampling rate...
    float alpha = 0.9;
    float angle_x = alpha*gyro_angle_x + (1.0 - alpha)*accel_angle_x;
    float angle_y = alpha*gyro_angle_y + (1.0 - alpha)*accel_angle_y;
    float angle_z = gyro_angle_z;
    
    char displayBuffer[25];
    /*//dtostrd((2000 - (t_now - t_offset)), 6, 0, displayBuffer);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(0, 67);
    sprintf( displayBuffer, "%lu %lu", t_now,  t_offset);
    M5.Lcd.printf(displayBuffer);
    
    M5.Lcd.setCursor(0, 82);
    //dtostrf(t_now, 6, 0, displayBuffer);
    sprintf( displayBuffer, "%lu", t_now - t_offset );
    M5.Lcd.printf(displayBuffer);
    
    M5.Lcd.setCursor(0, 97);
    //dtostrf(t_offset, 6, 0, displayBuffer);
    sprintf( displayBuffer, "%lu", t_offset );
    M5.Lcd.printf(displayBuffer);
*/
    if (not set_offset) {    
      M5.Lcd.setCursor(0, 50);
      M5.Lcd.setTextSize(4);
      //dtostrf(angle_x - angle_offset, 5, 1, displayBuffer);

      //M5.Lcd.printf(displayBuffer);
      if ((angle_x - angle_offset) >= 0) {
        int fraction = int(floor(((angle_x - angle_offset) - floor(angle_x - angle_offset)) * 10 ));      
        /*if (fraction > 5) {
          fraction = 5;
        } else {
          fraction = 0;
        }*/
      M5.Lcd.printf("%5d.%d", int(floor(angle_x - angle_offset)), fraction);

      } else
      {
        int fraction = int(floor(((angle_x - angle_offset) - ceil(angle_x - angle_offset)) * -10 ));      
        /*if (fraction > 5) {
          fraction = 5;
        } else {
          fraction = 0;
        }*/
      //Put negative sign in the string direction and convert to positive number to show -0.5 correctly  
      
      M5.Lcd.printf("  -%2d.%d", int((-1 * ceil(angle_x - angle_offset))), fraction);
      


      }
      
      /*M5.Lcd.setCursor(10, 83);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("%3.2f %3.1f", accX, accel_angle_x);

      M5.Lcd.setCursor(10, 98);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("%3.2f %3.1f", accY, accel_angle_y);


      M5.Lcd.setCursor(10, 113);
      M5.Lcd.setTextSize(2);
      M5.Lcd.printf("%3.2f", accZ);*/


    }    else {
      //M5.Lcd.setTextSize(2);
      //M5.Lcd.setCursor(0, 0);
      //dtostrf((t_now - t_offset), 6, 1, displayBuffer);
      //M5.Lcd.printf(displayBuffer);
      if ((t_now - t_offset) > 3000) {
        set_offset = false;
        angle_offset = angle_x;
        //M5.Lcd.printf("done");
        M5.Lcd.setCursor(0, 35);
        M5.Lcd.setTextSize(4);
        M5.Lcd.printf("        ");
      } else {
        M5.Lcd.setCursor(0, 50);
        M5.Lcd.setTextSize(4);
        M5.Lcd.printf("  ------  ");
      }
      
    }

    //M5.Lcd.printf("%.2f   %.2f   %.2f   ", gyroX, gyroY, gyroZ);
    /*M5.Lcd.setCursor(185, 45);
    M5.Lcd.print("o/s");
    M5.Lcd.setCursor(45, 60);
    M5.Lcd.printf("%.2f   %.2f   %.2f   ", accX, accY, accZ);
    M5.Lcd.setCursor(185, 60);
    M5.Lcd.print("G");
    M5.Lcd.setCursor(45, 75);
    M5.Lcd.printf("Temperature : %.2f  D", temp);
    M5.Lcd.setCursor(45, 105);
    M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", pitch, roll, yaw);*/
    set_last_read_angle_data(t_now, angle_x, angle_y, angle_z, unfiltered_gyro_angle_x, unfiltered_gyro_angle_y, unfiltered_gyro_angle_z);
    delay(10);
        
}
