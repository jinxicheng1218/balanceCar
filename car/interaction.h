#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

struct Interface{
  uint8_t n;
  int choice;
  void(*func[6])(void*);
  void *next[6];
  char text[128];
  int maxnum;
  int minnum;
  int *changeTarget;
  void *datas[6];
  byte type[6];
};

struct FrequencyRead{
  //bool startRead;
  //bool lastState;
  bool start;
  uint8_t pin;
  //int frequency;
  float dc;
  long frequency;
  long upTime;
  long tempTime;
  long cycle;
};


struct PWMout{
  bool start;
  uint8_t chan;
  uint8_t bit_num;
  uint8_t pin;
  int freq;
  int duty;
};

//float rev1=0,revAcc1=0,rev2=0,revAcc2=0;

Interface *first=new Interface;

Interface *freShow=new Interface;
Interface *pwmSet=new Interface;

Interface *pwmFreSet=new Interface;
Interface *pwmDutySet=new Interface;
////Interface *pwmOut=new Interface;



//Interface *=new Interface;
//free();
