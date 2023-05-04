


//标准的选择界面
void selectInterface(void *argument){
  Interface *interface = (Interface *)argument;
  float rev1=0,rev2=0,revAcc1=0,revAcc2=0;
  //uint8_t 
  while(!carStart){
    delay(100);
    motorsReader(blCar->m);
    rev1 = blCar->m->motors[0]->rev->rev;
    rev2 = blCar->m->motors[1]->rev->rev;
    
    //B电机转动调整选项
    interface->choice-=increment(rev2,revAcc2,50);
    interface->choice%=interface->n;
    if(interface->choice<0)interface->choice=interface->n-1;
    
    //A电机转动进行选定或取消
    if(increment(rev1,revAcc1,100)<0)return;
    else if(increment(rev1,revAcc1,100)>0){
      revAcc1=0;
      interface->func[interface->choice](interface->next[interface->choice]);
    }
    
    oled->clearBuffer();
    printText(oled,interface->text);
    oled->drawBox(120,10*interface->choice+5,5,5);
    draw(oled);
    //Serial.printf("%d  %f\n",interface->choice,revAcc2);
  }
}

//数值选择界面
void inputInterface(void *argument){
  Interface *interface = (Interface *)argument;
  delay(200);
  char text[32];
  float rev1=0,rev2=0,revAcc1=0,revAcc2=0;
  //interface->choice = interface->minnum;
  //uint8_t 
  while(!carStart){
    delay(100);
    motorsReader(blCar->m);
    rev1 = blCar->m->motors[0]->rev->rev;
    rev2 = blCar->m->motors[1]->rev->rev;
    
    //B电机转动调整选项
    interface->choice+=increment(rev2,revAcc2,max(2000/(interface->maxnum-interface->minnum),1))*min(max(interface->choice/*(interface->maxnum-interface->minnum)*//100,1),500);
    interface->choice%=interface->maxnum;
    if(interface->choice<interface->minnum)interface->choice=interface->maxnum-1;
    
    //A电机转动进行选定或取消
    if(increment(rev1,revAcc1,100)<0){
      interface->choice = *(interface->changeTarget);
      return;
    }
    else if(increment(rev1,revAcc1,100)>0){
      oled->clearBuffer();
      sprintf(text,"data saved\n");
      printText(oled,text);
      draw(oled); 
      *(interface->changeTarget) = interface->choice;
      delay(500);
      return;
    }
    

    sprintf(text,"%d%s\n",interface->choice,interface->text);
    oled->clearBuffer();
    printText(oled,text);
    //oled->drawBox(120,10*interface->choice+5,5,5);
    draw(oled); 
    //Serial.printf("%d  %f\n",interface->choice,revAcc2);
  }
}

//数值显示界面
//void outPutInterface(void *a){}
void outPutInterface(void *argument){
  Interface *interface = (Interface *)argument;
  delay(200);
  float rev1=0,revAcc1=0;
  //interface->choice = interface->minnum;
  //uint8_t 
  while(!carStart){
    //Serial.print(freShow->type[0]);
    Serial.printf("fre:%d Hz\ndc:%f\n",freReader->frequency,freReader->dc);
    delay(100);
    motorsReader(blCar->m);
    rev1 = blCar->m->motors[0]->rev->rev;

    if(increment(rev1,revAcc1,100)!=0){
      return;
    }

    //sprintf(text,"%s\n%s",interface->choice,interface->text);
    oled->clearBuffer();
    printData(oled,interface->text,interface->datas,interface->type);
    //Serial.print(interface->type[0]);
    //oled->drawBox(120,10*interface->choice+5,5,5);
    draw(oled); 
    //Serial.printf("%d  %f\n",interface->choice,revAcc2);
  }
}


int increment(float rev,float &acc,float thr){
  int re;
  if(abs(rev)>0)acc+=rev;
  else acc = 0;
  if(abs(acc)>thr){
    re = (int)(acc/thr);
    acc=0;
    return re;
  }
}



void initialInterfaces(){
  //第一层：主界面
  first->n = 4;
  first->choice = 0;
  sprintf(first->text,"PWM read\nPWM out\nFollow\nBalance Car!\n");
  first->func[0] = outPutInterface;
  first->func[1] = selectInterface;
  first->func[3] = deleteInterface;
  first->next[0] = freShow;
  first->next[1] = pwmSet;
  first->next[3] = &carStart;

  //第二层：频率读取
  sprintf(freShow->text,"frequency:\nHz\nduty cycle:\n \n");
  freShow->datas[0] = &freReader->frequency;
  freShow->datas[1] = &freReader->dc;
  freShow->type[0] = 0;
  freShow->type[1] = 2;

  //第二层：PWM输出设置
  pwmSet->n=3;
  pwmSet->choice = 0;
  sprintf(pwmSet->text,"PWM Frequency Set\nPWM Duty Set\nPWM OUT\n");
  pwmSet->func[0] = inputInterface;
  pwmSet->func[1] = inputInterface;
  pwmSet->func[2] = pwmOut;
  pwmSet->next[0] = pwmFreSet;
  pwmSet->next[1] = pwmDutySet;
  pwmSet->next[2] = pwm;

  //第二层：跟随输出设置

  //第三层：PWM频率设置
  sprintf(pwmFreSet->text,"Hz\n");
  pwmFreSet->maxnum = 1000001;
  pwmFreSet->minnum = 0;
  pwmFreSet->changeTarget = &pwm->freq;
  pwmFreSet->choice = 0;
  
 //第三层：PWM占空比设置
  sprintf(pwmDutySet->text,"/1023\n");
  pwmDutySet->maxnum = 1024;
  pwmDutySet->minnum = 0;
  pwmDutySet->changeTarget = &pwm->duty;
  pwmDutySet->choice = 0;

  
}

void deleteInterface(void *a){
  carStart = true;
  ledcDetachPin(pwm->pin);
  detachInterrupt(freReader->pin);
  delete(pwm);
  delete(freReader);
  delete(freShow);
  delete(pwmSet);
  delete(pwmFreSet);
  delete(pwmDutySet);
  delay(1000);
  balanceCarStart(blCar);
  
}
