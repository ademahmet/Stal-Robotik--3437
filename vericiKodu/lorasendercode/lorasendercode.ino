#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
double t0, p0;
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
int RXPin = 3;
int TXPin = 4;

int GPSBaud = 9600;
// lora lib
#include <SPI.h>
#include <LoRa.h>
int buzzer = 5;
int statu = 0;



// Create a TinyGPS++ object
TinyGPSPlus gps;

// Create a software serial port called "gpsSerial"
SoftwareSerial gpsSerial(RXPin, TXPin);
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

#define OUTPUT_READABLE_YAWPITCHROLL
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
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
uint8_t teapotPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}


void setup() {
  Serial.begin(9600);

  //lora setup
  while (!Serial);
  pinMode(5, OUTPUT);
  Serial.println("LoRa Sender");

  if (!LoRa.begin(868E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  pinMode(A3, INPUT);
  }
  pinMode(5, OUTPUT);


  while (!pressure.begin());
  gpsSerial.begin(GPSBaud);

  //    Serial.println("basinc calisiyor");
  //    t0 = sicaklik(); //kalibrasyon sicaklik degeri
  p0 = 1001.14;

  //    Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();
  //    Serial.println(F("Testing device connections..."));
  //    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));
  //    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
  //    Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    //        Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);
    // enable Arduino interrupt detection
    //        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    //        Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;
    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    //        Serial.print(F("DMP Initialization failed (code "));
    //        Serial.print(devStatus);
    //        Serial.println(F(")"));
  }
}
unsigned long s1 = 0;
unsigned long s2;
double t1, p1, h1;
int durum = 0;
int saat, gun;

void loop() {
  t1 = sicaklik();
  p1 = basinc(t1);
  h1 = pressure.altitude(p1, p0);

  while (durum == 0) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.altitude.meters() > 0) {

          durum = 1;
        }
      }
    }
  }
  durum = 0;
  if (!dmpReady) return;
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  mpu.resetFIFO();
  fifoCount -= packetSize;
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);


  if (h1 > 7 && statu == 0) {
    statu = 1;
  }
  if (h1 < 5 && statu == 1) {
    digitalWrite(buzzer, HIGH);
    statu == 2;     
  }



  s2 = millis();
  // put your main code here, to run repeatedly:
  if (s2 - s1 > 900) {
    s1 = s2;
    saat = gps.time.hour();
    gun = gps.date.day();
    if (saat < 21) {
      saat += 3;
    }
    else {
      saat -= 21;
      gun += 1;
    }
    LoRa.beginPacket();
    LoRa.print(p1 * 100);
    LoRa.print(" ");
    LoRa.print(t1);
    LoRa.print(" ");
    LoRa.print(h1);
    LoRa.print(" ");
    LoRa.print(ypr[0] * 180 / M_PI);
    LoRa.print(" ");
    LoRa.print(ypr[1] * 180 / M_PI);
    LoRa.print(" ");
    LoRa.print(ypr[2] * 180 / M_PI);
    LoRa.print(" ");
    LoRa.print(gps.location.lat(), 6);
    LoRa.print(" ");
    LoRa.print(gps.location.lng(), 6);
    LoRa.print(" ");
    LoRa.print(gps.altitude.meters());
    LoRa.print(" ");
    LoRa.print(gun);
    LoRa.print("/");
    LoRa.print(gps.date.month());
    LoRa.print("/");
    LoRa.print(gps.date.year());
    LoRa.print(" ");
    if (gps.time.hour() < 10)
      LoRa.print(F("0"));
    LoRa.print(saat);
    LoRa.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    LoRa.print(gps.time.minute());
    LoRa.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    LoRa.print(gps.time.second());
    if (statu == 2) {
      LoRa.print(" inis ");
    }
    else {
      LoRa.print(" kalkis ");
    }
    LoRa.print(analogRead(A3));
    LoRa.println("");
    LoRa.endPacket();

    Serial.print(p1);
    Serial.print(" ");
    Serial.print(t1);
    Serial.print(" ");
    Serial.print(h1);
    Serial.print(" ");
    Serial.print(ypr[0] * 180 / M_PI);
    Serial.print(" ");
    Serial.print(ypr[1] * 180 / M_PI);
    Serial.print(" ");
    Serial.print(ypr[2] * 180 / M_PI);
    Serial.print(" ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" ");
    Serial.print(gps.location.lng(), 6);
    Serial.print(" ");
    Serial.print(gps.altitude.meters());
    Serial.print(" ");
    Serial.print(gun);
    Serial.print("/");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.year());
    Serial.print(" ");
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(saat);
    Serial.print(":");
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(":");
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    if (statu == 2) {
      Serial.print(" inis ");
    }
    else {
      Serial.print(" kalkis ");
    }
    Serial.print(analogRead(A3));
    Serial.println("");









  }
}

double sicaklik() {
  char status;
  double T;
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      return (T);
    }
    else Serial.print("Temnot1");
  }
  else Serial.print("Temnot0");
}

double basinc(double T) {
  char status;
  double P;
  status = pressure.startPressure(3);
  if (status != 0)
  {
    delay(status);
    status = pressure.getPressure(P, T);
    if (status != 0)
    {
      return (P);
    }
    else Serial.print("Prenot1");
  }
  else Serial.print("Prenot0");
}
