//马达(引脚)初始化
Motorset *motorSetup(uint8_t In1,uint8_t In2,uint8_t En,uint8_t R1,uint8_t R2){
  Motorset *m = new Motorset;
  m->pin=new MotorPin{In1,In2,En,R1,R2};
  m->rev=motorReaderSetup(R1,R2);
  pinMode(In1,OUTPUT);
  pinMode(In2,OUTPUT);
  pinMode(R1,INPUT);
  pinMode(R2,INPUT);
  return m;
}
//马达pid初始化
void motorPid(Motorset *motor,float p,float i,float d,float Imax,float Emax){
  motor->revCon=pidSetup(Imax,Emax,p,i,d);//new Pidset{Imax,Emax,p,i,d};
  motor->targetRev = &motor->revCon->t;
}
//启动全部计数器
Ticker *startCounter(Motors *motors,float ms){
  Ticker *ticker = new Ticker;
  ticker->attach_ms(ms,motorsFrequency,motors);
  return ticker;
}
void motorsFrequency(Motors *motors){
  for(uint8_t i=0;i<motors->n;i++){
    frequencyCounter(motors->motors[i]->rev);
  }
}
//启动全部转速计算器
Ticker *startRev(Motors *motors,float ms){
  Ticker *ticker = new Ticker;
  ticker->attach_ms(ms,motorsReader,motors);
  return ticker;
}
void motorsReader(Motors *motors){
  for(uint8_t i=0;i<motors->n;i++){
    timeReader(motors->motors[i]->rev);
  }
}


void motorControl(Motorset *motor,float targetRev){
  float duty;
  //Revset *revReader = motor->rev;
  //*motor->targetRev = targetRev;
  //motor->revCon->d = revReader->rev;
  //pid(motor->revCon,duty);
  //下面这个是测试用的
  duty = targetRev;
  if(duty>=0){
    digitalWrite(motor->pin->In1,0);
    digitalWrite(motor->pin->In2,1);
    motorMover(motor->mover,min((int)(duty),1022));
  }else{
    digitalWrite(motor->pin->In1,1);
    digitalWrite(motor->pin->In2,0);
    motorMover(motor->mover,min((int)(0-duty),1022));
  }
}

//Motorset *motorPins(uint8_t In1,uint8_t In2,uint8_t EN,uint8_t R1,uint8_t R2){
//  
//}
//马达pwm输出初始化
void motorMoverSetup(Motorset *motor,uint8_t chan){
  ledcSetup(chan,100,10);
  ledcAttachPin(motor->pin->En,chan);
  motor->mover = new MotorMoverset{motor->pin->En,chan,0};
}
//马达输出设置
//void motorMover(Motorset *motor,uint32_t duty){
//  ledcWrite(motor->mover->chan,duty);
//  motor->mover->duty = duty;
//}
//马达pwm初始化
MotorMoverset *motorMoverSetup(uint8_t chan,MotorPin *pins){
  ledcSetup(chan,100,10);
  ledcAttachPin(pins->En,chan);
  return new MotorMoverset{pins->En,chan,0};
}
//马达pwm输出
void motorMover(MotorMoverset *motor,uint32_t duty){
  ledcWrite(motor->chan,duty);
  motor->duty = duty;
}

//转速计初始化
Revset* motorReaderSetup(uint8_t pin1,uint8_t pin2){
  return new Revset{pin1,pin2,0,0,0,0};
}

void deleteReader(Revset *revArg){
  delete revArg;
}
//转速
float readRev(Revset *revArg){
  return revArg->rev;
}
//转速计算
void timeReader(Revset *revArg){
  int ms = abs(millis()-revArg->lastTime);
  revArg->lastTime = millis();
  revArg->rev = 0.5*revArg->frequency*1000/(11.0*ms);
  revArg->frequency = 0;
}

//旋转计数器
void frequencyCounter(Revset *revArg){
  byte newState = digitalRead(revArg->input1);//<<1|digitalRead(revArg->input2);
  if(revArg->state ==0&&newState==1){
    revArg->frequency+=(digitalRead(revArg->input2)?-1:1);
  }
  revArg->state = newState;
  //Serial.printf("\n%d,%d",digitalRead(revArg->input1),digitalRead(revArg->input2));
}
