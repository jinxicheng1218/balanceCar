FrequencyRead *frequencyReaderSetup(uint8_t pin){
  pinMode(pin,INPUT_PULLUP);
  FrequencyRead *freReader = new FrequencyRead;
  freReader->pin = FREQUENCYREAD;
  freReader->dc=0;
  freReader->frequency=0;
  freReader->upTime=0;
  freReader->cycle=0;
  freReader->start=false;

  return freReader;
}


void freReadChange(){
  freReader->tempTime = micros();
  if(digitalRead(freReader->pin)){
    freReader->cycle = (freReader->tempTime-freReader->upTime);
    freReader->frequency = 1000000/freReader->cycle;
    freReader->upTime = freReader->tempTime;
    //Serial.print("up\n");
  }else{
    freReader->dc = (float)((freReader->tempTime-freReader->upTime)/(double)freReader->cycle);
  }
}




void freReadStart(){
  attachInterrupt(freReader->pin,freReadChange,CHANGE);
  freReader->start = true;
}

void freReadEnd(FrequencyRead *freReader){
  detachInterrupt(freReader->pin);
  freReader->start = false;
}


PWMout *pwmMakerSetup(uint8_t pin,uint8_t chan){
  PWMout *pwm = new PWMout;
  pwm->pin = pin;
  pwm->chan = chan;
  pwm->start = false;
  pwm->duty = 0;
  pwm->freq = 0;
  return pwm;
}

void setPwm(PWMout *pwm,int freq,int duty){
  pwm->start = false;
  pwm->freq = freq;
  pwm->duty = duty;
  
}

void pwmDutyOut(PWMout *pwm){
  pwm->start = true;
  ledcSetup(pwm->chan,pwm->freq,10);
  ledcAttachPin(pwm->pin,pwm->chan);
  ledcWrite(pwm->chan,pwm->duty);
}

void pwmStop(PWMout *pwm){
  pwm->start = false;
  ledcDetachPin(pwm->pin);
}

void pwmOut(void *argument){
  PWMout *pwm = (PWMout*)argument;
  //pwm->freq = 1145;
  pwmStop(pwm);
  pwmDutyOut(pwm);
  oled->clearBuffer();
  oled->drawStr(0,10,"PWM set up");
  draw(oled);
  delay(1000);
  //if(pwm->start) pwmDutyOut(pwm);
  //else pwmStop(pwm);
}
