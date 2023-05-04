#include <Ticker.h>
#include "BluetoothSerial.h"
#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

//#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
//#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
//#endif 

//mpu6050-i2c通信引脚
#define SDA      23
#define SCL      22
#define LED_PIN  13//mpu中断引脚

//电机控制引脚
#define IN1      15
#define IN2      2
#define IN3      0
#define IN4      4
#define ENA      16
#define ENB      17
#define RA1      39
#define RA2      36
#define RB1      34
#define RB2      35       

#define M        12
#define BTMSGLEN 32

#define OLEDD0   18
#define OLEDD1   19
#define OLEDRES  21
#define OLEDDC   3
#define OLEDCS   1

#define FREQUENCYREAD 14
#define FREQUENCYOUT  27

//马达引脚参数
struct MotorPin{
  uint8_t In1;
  uint8_t In2;
  uint8_t En;
  uint8_t R1;
  uint8_t R2;
};

//马达PWM参数
struct MotorMoverset{
  uint8_t pin;
  uint8_t chan;
  uint16_t duty;
};


//pid参数
struct Pidset {
  float t;
  float d;
  float Esum;
  float last;
  float Imax;
  float Emax;
  float kP;
  float kI;
  float kD;
  long long int lastTime;
};

//电机转速读取参数
struct Revset {
  uint8_t input1;
  uint8_t input2;
  byte state;
  int frequency;
  unsigned long lastTime;
  float rev;
  //Ticker *ticker1;
  //Ticker *ticker2;
};

//马达全部参数
struct Motorset{
  float *targetRev;
  MotorMoverset *mover;
  MotorPin *pin;
  Pidset *revCon;
  Revset *rev;
  
};

//马达组
struct Motors{
  Motorset *motors[4];
  uint8_t n;
};


struct Car{
  Pidset *std;
  Pidset *vel;
  Pidset *dir;
  Motors *m;
  float ypr[3];
  float tarV;
  float tarD;
  float lastR;
  float delat;
  Ticker *yprRead;
  Ticker *carControl;
};

//void motorPid(Motorset *motor,float p,float i,float d,float Imax,float Emax);
void keepBalance();
//printText(U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled/*,uint8_t line*/,char *text)



//i2c
#include "MPU6050_6Axis_MotionApps20.h"
MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL

 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
//float ypr_data[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
