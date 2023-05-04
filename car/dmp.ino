
//#include "I2Cdev.h"
void dmpDataReady() {
    mpuInterrupt = true;
}

void mpu_setup() {
//    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin(SDA,SCL);
    Wire.setClock(1000000);
        //TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
//    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
//        Fastwire::setup(400, true);
//    #endif
    mpu.initialize();
    devStatus = mpu.dmpInitialize();
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); 
    if (devStatus == 0) {
        mpu.setDMPEnabled(true);
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void mpu_loop(float *ypr_data) {
    if (!dmpReady)return;
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();
    //fifo关闭   测试
    fifoCount = mpu.getFIFOCount();
//    if ((mpuIntStatus & 0x10) || fifoCount == 32) {
//        mpu.resetFIFO();
//        Serial.println(F("FIFO overflow!"));
//    } else 
    if (mpuIntStatus & 0x02) {
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        fifoCount -= packetSize;
        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr_data, &q, &gravity);
            if(showDmpData){
              Serial.print("ypr_data\t");
              Serial.print(ypr_data[0] * 180/M_PI);
              Serial.print("\t");
              Serial.print(ypr_data[1] * 180/M_PI);
              Serial.print("\t");
              Serial.println(ypr_data[2] * 180/M_PI);
            }
            
            //测试用
            mpu.resetFIFO();
        #endif
    }
    //delay(1);
}
