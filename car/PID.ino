Pidset* pidSetup(float Imax,float Emax,float p,float i,float d){
  return new Pidset{0,0,0,0,Imax,Emax,p,i,d,millis()};
}
void deletePidset(Pidset *st){
  delete st;
}

void pid(Pidset *PIDarg,float &c){
  float e = PIDarg->t-PIDarg->d;
  PIDarg->Esum += e*PIDarg->kI;
  if(PIDarg->Imax > 0){
    if(abs(PIDarg->Esum)>PIDarg->Imax)PIDarg->Esum=(PIDarg->Esum>0?PIDarg->Imax:-1*PIDarg->Imax);
  }
  if(PIDarg->Emax > 0){
    if(abs(e)>PIDarg->Emax)PIDarg->Esum = 0;
  }
  c = e*PIDarg->kP + PIDarg->Esum + 1000*((PIDarg->d-PIDarg->last)*PIDarg->kD)/(millis()-PIDarg->lastTime);
  PIDarg->last = PIDarg->d;
  PIDarg->lastTime = millis();
}
