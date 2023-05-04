void printCarData(){
  oled->clearBuffer();
  //sprintf(displayData,"ypr:%.4f",blCar->ypr[2]);
  //printText(oled,2,displayData);
  sprintf(displayData,"stand:\np:%.1f, i:%.1f, d:%.1f\nvel:\np:%.2f,i:%.4f,d:%d\nRoll:%.4f\ntarget:%.4f\n",blCar->std->kP,blCar->std->kI,blCar->std->kD,blCar->vel->kP,blCar->vel->kI,blCar->vel->kD,blCar->ypr[2]*(180/M_PI),blCar->vel->t);
  printText(oled,displayData);
  //printText
  //oled->sendBuffer();

  draw(oled);
}

void keepBalance(Car *car){
  //Serial.print(car->ypr[2]);
  
  float turn = 0;
  float r;
  motorsReader(car->m);
  //mpu_setup();
  //mpu_loop(car->ypr);
  
  
  r = car->ypr[2] * (180/M_PI);
  //Serial.printf("%.6f\n",lastYpr);
  if(abs(r-car->lastR)>=20){
    r=car->lastR;
    //mpu_setup();
    Serial.print("reset1\n");
    return;
  }else{
    //我觉得颇具创新的“重启”操作，在角度值长时间不变时，为规避mpu问题，对mpu进行重启操作。这一操作雀食避免了之前经常出现的mpu卡死问题
    car->delat /= 10;
    car->delat += r-car->lastR;
    car->lastR = r;
    if(car->delat==0){
      mpu_setup();
      Serial.print("reset2\n");
      return;
    }
  }

  
  //直立环数据输入
  car->std->d = r;
  //速度环数据输入
  car->vel->d = (car->m->motors[0]->rev->rev+car->m->motors[1]->rev->rev)/2;
  //这行，锁定目标速度
  //car->vel->t = 0;

  //速度环的pid输出
  pid(car->vel,car->std->t);

  //这行，锁定角度
  //bcar->std->t = 0;

  
  car->std->t += 2.9;        //因为平衡点不一定在0度的位置，所以需要配平（其实不用也可以）
  pid(car->std,car->tarV);//直立环pid输出

  car->dir->d = car->ypr[0]*(180/M_PI);
  pid(car->dir,turn);

  motorControl(car->m->motors[0],car->tarV+turn/*+def*/);
  motorControl(car->m->motors[1],car->tarV-turn/*-def*/);
//
//  //Serial.begin(115200);
  Serial.printf("%.6f,%.6f,%.6f\n",car->delat,r,car->dir->d);//car->vel->d);//motors.motors[0]->rev->rev);
//  //Serial.end();
////  Serial.printf("%f\n",car->tarV);//car->m->motors[0]->rev->rev);
}


void calibrate(Car *car){
  float delat = 0;

  
  char show[32];
  for(int n = 4;n>0;n--){
    delat += abs(memory[n-1]-memory[n]);
    memory[n] = memory[n-1];
  }
  memory[0] = car->ypr[0]* (180/M_PI);
  if(abs(delat)<0.01&&delat != 0){
    oled->clearBuffer();
    oled->drawStr(0,10,"Caliibrate is over!");
    oled->sendBuffer();
    car->carControl->detach();
    car->dir->t = memory[0];
    delay(1000);
    car->carControl->attach_ms(25,keepBalance,car);
    carMoveStart = true;
    return;
  }
  
  oled->clearBuffer();
  for(int n = 4;n>=0;n--){
    sprintf(show,"%f",memory[n]);
    oled->drawStr(0,(5-n)*10,show);
  }
  oled->drawBox(5,60,max(0,(int)((5-abs(delat))*25)),8);
  oled->sendBuffer();
}
