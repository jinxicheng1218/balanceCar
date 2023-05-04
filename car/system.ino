void systemSetup(){
  //元件设置
  //mpu_setup();
  delay(1000);
  motor1 = motorSetup(IN1,IN2,ENA,RA1,RA2);
  motor2 = motorSetup(IN3,IN4,ENB,RB1,RB2);
  motors->motors[0] = motor1;
  motors->motors[1] = motor2;
  motors->n=2;
  
  motorMoverSetup(motor1,8);
  motorMoverSetup(motor2,9);

  rotRead = startCounter(motors,0.05);
  oled=oledSetup(OLEDD0,OLEDD1,OLEDCS,OLEDDC,OLEDRES);


  

  //平衡车初始化
  //blCar = balanceCarSetup();
  
}


Car* balanceCarSetup(){
  Car *car = new Car;
  car->std = pidSetup/*(-1,-1,37,0,0);//-2.0版本*/(-1,-1,42*0.7,0,-5*0.7);//1.0版本：(-1,-1,35*0.7,0,-4*0.7);;
  car->vel = pidSetup(-1,-1,-0.38,-0.38/100,0 );//1.0版本：(5,20,-0.68,-0.68/150,0);;
  car->dir = pidSetup(20,10,10,0.1,-0.01);
  car->dir->t = 0;
  
  car->m = motors;
  //car->ypr[3] = {0,0,0};
  car->tarV = 0;
  car->tarD = 0;
  car->lastR = 0;
  car->delat = 10;
  mpu_setup();
  return car;
}

void balanceCarStart(Car *car){
  car->yprRead = new Ticker;
  car->carControl = new Ticker;

  oled->clearBuffer();
  oled->drawStr(0,10,"Please wait to calibrate");
  oled->drawStr(0,20,"keep static!");
  oled->sendBuffer();
  delay(1000);
  
  car->yprRead->attach_ms(12,mpu_loop,car->ypr);
  car->carControl->attach_ms(100,calibrate,car);
  //car->carControl->attach_ms(25,keepBalance,car);
}
