


#include "cla.h"
#include "interaction.h"
//uint8_t rstate,ran;
//long int lastTime;
//MotorPin *ml = new MotorPin{IN1,IN2,ENA,RB1,RB2};
//void vReader(){
//  revx = 200*(float)ran/11;
//  ran = 0;
//}
//byte chancetime=0;
int targetV,def=0;
char displayData[256]; 

bool showDmpData=false;

float ypr[3];
float tarRev,lastYpr = 0;
float p,rev;

Revset *motorA;
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI *oled;

float turn(float r){
  return (r/180)*50;
}
Motors *motors = new Motors;
Motorset *motor1,*motor2;
Ticker *rotRead;//,*yprRead,*carControl;

//定时器中断
hw_timer_t * ctrlor = NULL;

void IRAM_ATTR onTimer(){
  keepBalance();
}


MotorMoverset *m1m;
Revset *revl;

Pidset *balance,*stand;

BluetoothSerial btControl;
//void setup(){};
//void loop(){};
Car *blCar;
PWMout *pwm = new PWMout;
FrequencyRead *freReader;
float memory[5] = {0,0,0,0,0};
bool carMoveStart = false;


void setup() {
//  Serial.begin(115200);
  systemSetup();
  btControl.begin("BalanceCar");
//  delete(freShow);
//  delete(pwmSet);
//  delete(pwmFreSet);
//  delete(pwmDutySet);
  pwm = pwmMakerSetup(FREQUENCYOUT,10);
  //attachInterrupt(freReader->pin,freReadUp,RISING);
  freReader = frequencyReaderSetup(FREQUENCYREAD);
  freReadStart();
  blCar = balanceCarSetup();
  initialInterfaces();
  selectInterface(first);


  //mpu_setup();
  
  
  //Serial.print(freShow->type[0]);
  
  //outPutInterface(freShow);
  //Serial.print(freShow->type[0]);
  //pinMode(14,INPUT_PULLUP);
  //attachInterrupt(14,freReadUp,FALLING);//RISING);
  
  //initialInterfaces();
  //selectInterface(first);
//
//  test->maxnum=1024;
//  test->minnum=0;
//  test->changeTarget=&targetV;
//  sprintf(test->text,"Hz\n");
//
//  
//  //balanceCarStart(blCar);
//  //inputInterface(test);
//  selectInterface(first);
}



char message[BTMSGLEN]="";
uint8_t msg;
uint8_t mesLen = 0;
bool fu=false,carStart=false;

void loop() {
  //delay(100);
  //mpu_loop(ypr);
  //Serial.print(blCar->ypr[2]);
  //Serial.print("\n");
  //Serial.printf("fre:%d Hz\ndc:%f\n",freReader->frequency,*(float*)&freReader->dc);
  
  
  
  delay(200);
  //Serial.printf("%s\n",displayData);
  //Serial.write(4949122);
  if(btControl.available()){ 
    msg=btControl.read();
    if(msg = 115 && !carStart){
      carStart = true;
      //deleteInterface();
      
      //delay(100);
      balanceCarStart(blCar);
    }
    if(msg>=48&&msg<=57){
      targetV*=10;
      targetV+=msg-48;
    }else{
      if(msg==45){
        fu = true;
      }else if(msg==35){
        if(fu)targetV*=-1;
        Serial.printf("\nget BTdata as:%d",targetV);
        blCar->vel->t = targetV;
        targetV = 0;
        mesLen = 0;
        fu = false;
      }else if(msg==36){
        if(fu)targetV*=-1;
        Serial.printf("\nget BTdata turn as:%d",targetV);
        blCar->dir->t += targetV;
        targetV = 0;
        mesLen = 0;
        fu = false;
      }
    }
  }

  if(carMoveStart){
    printCarData();
  }

//

}
