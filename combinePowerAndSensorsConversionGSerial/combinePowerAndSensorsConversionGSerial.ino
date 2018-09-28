#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

//variables
MPU6050 accelgyro; //address used is 0x68
int counter = 0;
int16_t ax, ay, az, gx, gy, gz;
unsigned long prevMillis = 0;
float energy = 0;

void setup() {
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  Serial3.begin(9600);

  // imu 1
  pinMode(50, OUTPUT);
  digitalWrite(50, LOW);
  Wire.begin();
  accelgyro.setXAccelOffset(-1750);
  accelgyro.setYAccelOffset(-1500);
  accelgyro.setZAccelOffset(1200);
  accelgyro.setXGyroOffset(-24); //check again?
  accelgyro.setYGyroOffset(-80); //check again?
  accelgyro.setZGyroOffset(63); 

  // initialize serial communication
  Serial.begin(9600);

  //first imu
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 first connection successful" : "MPU6050 first connection failed");
  Serial3.println("Testing device connections...");
  Serial3.println(accelgyro.testConnection() ? "MPU6050 first connection successful" : "MPU6050 first connection failed");
  
  //second imu
  pinMode(51, OUTPUT);
  digitalWrite(50, HIGH);
  digitalWrite(51, LOW);
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 second connection successful" : "MPU6050 second connection failed");
  Serial3.println("Testing device connections...");
  Serial3.println(accelgyro.testConnection() ? "MPU6050 second connection successful" : "MPU6050 second connection failed");
  accelgyro.setXAccelOffset(-3839);
  accelgyro.setYAccelOffset(-231);
  accelgyro.setZAccelOffset(1240);
  accelgyro.setXGyroOffset(44);
  accelgyro.setYGyroOffset(33);
  accelgyro.setZGyroOffset(-35); 

  //third imu
  pinMode(52, OUTPUT);
  digitalWrite(51, HIGH);
  digitalWrite(52, LOW);
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 third connection successful" : "MPU6050 third connection failed");
  Serial3.println("Testing device connections...");
  Serial3.println(accelgyro.testConnection() ? "MPU6050 third connection successful" : "MPU6050 third connection failed");
  accelgyro.setXAccelOffset(-600);
  accelgyro.setYAccelOffset(-2152);
  accelgyro.setZAccelOffset(550);
  accelgyro.setXGyroOffset(-31);
  accelgyro.setYGyroOffset(-32);
  accelgyro.setZGyroOffset(0); 

  //fourth imu
  pinMode(53, OUTPUT);
  digitalWrite(52, HIGH);
  digitalWrite(53, LOW);
  accelgyro.initialize();
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 fourth connection successful" : "MPU6050 fourth connection failed");
  Serial3.println("Testing device connections...");
  Serial3.println(accelgyro.testConnection() ? "MPU6050 fourth connection successful" : "MPU6050 fourth connection failed");
  accelgyro.setXAccelOffset(-1025);
  accelgyro.setYAccelOffset(1279);
  accelgyro.setZAccelOffset(2047);
  accelgyro.setXGyroOffset(-96);
  accelgyro.setYGyroOffset(37);
  accelgyro.setZGyroOffset(-35); 
}

void loop() {
  int voltage = analogRead(A0);
  //divide by 1023, multiply by 5V, multiply by 2 (because resistors same) 
  Serial3.print("Voltage: ");
  float realV = voltage / 1023.0 * 5 * 2;
  Serial3.print(realV);

  float sensorValue = analogRead(A1);
  float sensor = (sensorValue * 5) / 1023; //Vo
  float current = (sensor) / (0.1 * 10) * 1000 * (3.5-0.35) + 0.35; //changing the range of current to 0.35A to 3.5A
  //Serial3.print(" and vout: ");
  //Serial3.print(sensorValue);
  Serial3.print("V; Current: ");
  Serial3.print(current);
  Serial3.print("mA; Power: ");

  float power = current * realV;
  Serial3.print(power);
  Serial3.print("mW; Energy: ");

  unsigned long currentMillis = millis();
  energy += power / 1000 * ((currentMillis - prevMillis) / 1000.0);
  Serial3.print(energy);
  Serial3.println("J");
  prevMillis = currentMillis;

  if (counter == 0) {
        digitalWrite(50, LOW);
        digitalWrite(51, HIGH);
        digitalWrite(52, HIGH);
        digitalWrite(53, HIGH);
        Serial.print("first: ");
        Serial3.print("first: ");
  } else if (counter == 1) {
        digitalWrite(50, HIGH);
        digitalWrite(51, LOW); 
        digitalWrite(52, HIGH);
        digitalWrite(53, HIGH);
        Serial.print("second: ");
        Serial3.print("second: ");
  } else if (counter == 2) {
        digitalWrite(50, HIGH);
        digitalWrite(51, HIGH);
        digitalWrite(52, LOW);
        digitalWrite(53, HIGH);
        Serial.print("third: ");
        Serial3.print("third: ");
  } else {
        digitalWrite(50, HIGH);
        digitalWrite(51, HIGH);
        digitalWrite(52, HIGH);
        digitalWrite(53, LOW);
        Serial.print("fourth: ");
        Serial3.print("fourth: ");
  }

    accelgyro.getAcceleration(&ax, &ay, &az);
    accelgyro.getRotation(&gx, &gy, &gz);

    //accelerometer with +/- 2g
    float accConverter = 4.0 / 65545; //range is -2 to 2 over 16 bit range
    float convertedax = ax * accConverter;
    float converteday = ay * accConverter;
    float convertedaz = az * accConverter;
    //gyrometer with +/- 250 degrees/sec
    float gyroConverter = 500.0 / 65545; //range is -250 to 250 over 16 bit range
    int16_t convertedgx = gx * gyroConverter;
    int16_t convertedgy = gy * gyroConverter;
    int16_t convertedgz = gz * gyroConverter;
    
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    Serial.print("a/g (in g and degrees):\t");
    Serial.print(convertedax); Serial.print("g\t");
    Serial.print(converteday); Serial.print("g\t");
    Serial.print(convertedaz); Serial.print("g\t");
    Serial.print(convertedgx); Serial.print("deg\t");
    Serial.print(convertedgy); Serial.print("deg\t");
    Serial.print(convertedgz); Serial.println("deg");
  /*
    Serial3.print("a/g:\t");
    Serial3.print(ax); Serial3.print("\t");
    Serial3.print(ay); Serial3.print("\t");
    Serial3.print(az); Serial3.print("\t");
    Serial3.print(gx); Serial3.print("\t");
    Serial3.print(gy); Serial3.print("\t");
    Serial3.println(gz);
    
    Serial3.print("a/g (in g and degrees):\t");
    Serial3.print(convertedax); Serial3.print("\t");
    Serial3.print(converteday); Serial3.print("\t");
    Serial3.print(convertedaz); Serial3.print("\t");
    Serial3.print(convertedgx); Serial3.print("\t");
    Serial3.print(convertedgy); Serial3.print("\t");
    Serial3.println(convertedgz); */
  
  counter = (counter + 1) % 4;
  delay(333);
}
