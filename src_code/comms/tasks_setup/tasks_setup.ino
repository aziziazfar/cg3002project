#include "Arduino_FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define STACK_SIZE 200

/* Constants and other Global Variables */
// dummy i2c addresses of the IMUs
//const int MPU_left_arm_addr = 0x68;
//const int MPU_right_arm_addr = 0x42;
//const int MPU_left_leg_addr = 0x61;
//const int MPU_right_leg_addr = 0x48;

const byte startHandshake = 'H';
const byte ack = 'A';
const byte nak = 'N';

uint8_t current;
uint8_t voltage;
const TickType_t xTaskFreq = 10;
SemaphoreHandle_t xSemaphoreSensors = NULL;
SemaphoreHandle_t xSemaphoreBuffer = NULL;

/****** STRUCT DECLARATIONS ******/
struct sensorReadings {
  uint8_t acc_x;
  uint8_t acc_y;
  uint8_t acc_z;
  uint8_t gyro_x;
  uint8_t gyro_y;
  uint8_t gyro_z;
};

struct sensorReadings arm_left;
struct sensorReadings arm_right;
struct sensorReadings leg_left;
struct sensorReadings leg_right;
/*********************************/

/****** DATA SERIALIZATION *******/
const int packetLength = 24;
const int bufferSize = 16;
int sendId = 0;  // data yet to be ack by rpi (head of ringbuffer)
int insertId = 0; // data to save to 
unsigned char ringBuffer[bufferSize][packetLength];

// Copies structure data into an array of char
unsigned int serialize(unsigned char *buf, int8_t *p, size_t size) {
  char checksum = 0;
  buf[0] = size+2;
  memcpy(buf+1, p, size);
  for(int i=1; i<=size; i++) {
    checksum ^= buf[i];
  }
  buf[size+1] = checksum;
  
  // insert buf into ring buffer
  if(xSemaphoreTake(xSemaphoreBuffer, 5) == pdTRUE) {
    ringBuffer[insertId][packetLength] = buf[1];
    insertId = (insertId + 1) % bufferSize;
    xSemaphoreGive(xSemaphoreBuffer);
  }
  Serial.print("size is ");
  Serial.println(size+2);
  return size+2;
}

void sendDataPacket() {
  int8_t cfg[packetLength];
  unsigned char buffer[64];

  // Copy sensor data to data packet
  cfg[0] = arm_left.acc_x;
  cfg[1] = arm_left.acc_y;
  cfg[2] = arm_left.acc_z;
  cfg[3] = arm_left.gyro_x;
  cfg[4] = arm_left.gyro_y;
  cfg[5] = arm_left.gyro_z;
    
  cfg[6] = arm_right.acc_x;
  cfg[7] = arm_right.acc_y;
  cfg[8] = arm_right.acc_z;
  cfg[9] = arm_right.gyro_x;
  cfg[10] = arm_right.gyro_y;
  cfg[11] = arm_right.gyro_z;
    
  cfg[12] = leg_left.acc_x;
  cfg[13] = leg_left.acc_y;
  cfg[14] = leg_left.acc_z;
  cfg[15] = leg_left.gyro_x;
  cfg[16] = leg_left.gyro_y;
  cfg[17] = leg_left.gyro_z;
    
  cfg[18] = leg_right.acc_x;
  cfg[19] = leg_right.acc_y;
  cfg[20] = leg_right.acc_z;
  cfg[21] = leg_right.gyro_x;
  cfg[22] = leg_right.gyro_y;
  cfg[23] = leg_right.gyro_z;

  unsigned len = serialize(buffer, cfg, sizeof(cfg));
  sendSerialData(buffer, len);
}

void sendSerialData(unsigned char *buffer, int len) {
  Serial.print("len is ");
  Serial.println(len);
  for(int i=0; i<len; i++) {
    Serial.print("Test send: ");
    Serial.println(buffer[i]);
    Serial2.println(buffer[i]);
  }
}
/*********************************/

/********** TASKS ***************/
void uartSend(void * pvParameters) {
  TickType_t xPreviousWakeTimeSend = xTaskGetTickCount();
  while(1) {
    Serial.println("uartSend");
    // organise data, serialize, send
    sendDataPacket();
    xSemaphoreGive(xSemaphoreSensors);
    vTaskDelayUntil(&xPreviousWakeTimeSend, xTaskFreq);
  }
}

void uartReceive(void * pvParameters){
  char ackByte;
  TickType_t xPreviousWakeTimeReceive = xTaskGetTickCount();
  
  while(1) {
    Serial.println("uartReceive");
    if(Serial2.available() == 1) {
      ackByte = Serial2.read();
      if(ackByte == 'A') {
        // ack received packet
        Serial.print("A received");
        // update ring buffer
        sendId = (sendId + 1) % bufferSize;
      } else if(ackByte == 'N') {
        Serial.print("N received");
        if(xSemaphoreTake(xSemaphoreBuffer, 2) == pdTRUE) {
          sendSerialData(ringBuffer[sendId], packetLength);
          xSemaphoreGive(xSemaphoreBuffer);
        }
      }
    }
    vTaskDelayUntil(&xPreviousWakeTimeReceive, xTaskFreq);
  }
}

void getSensorData(void * pvParameters) {
  TickType_t xPreviousWakeTimeSensor = xTaskGetTickCount();
  while(1) {
    if(xSemaphoreTake(xSemaphoreSensors, 0) == pdTRUE) {
      Serial.println("getSensorData");
      // Dummy values
      arm_left.acc_x = 1;
      arm_left.acc_y = 2;
      arm_left.acc_z = 3;
      arm_left.gyro_x = 10;
      arm_left.gyro_y = 20;
      arm_left.gyro_z = 35;
    
      arm_right.acc_x = 10;
      arm_right.acc_y = 60;
      arm_right.acc_z = 30;
      arm_right.gyro_x = 5;
      arm_right.gyro_y = 15;
      arm_right.gyro_z = 25;
    
      leg_left.acc_x = 40;
      leg_left.acc_y = 20;
      leg_left.acc_z = 50;
      leg_left.gyro_x = 35;
      leg_left.gyro_y = 55;
      leg_left.gyro_z = 65;
    
      leg_right.acc_x = 20;
      leg_right.acc_y = 30;
      leg_right.acc_z = 40;
      leg_right.gyro_x = 25;
      leg_right.gyro_y = 35;
      leg_right.gyro_z = 45;
    }
    vTaskDelayUntil(&xPreviousWakeTimeSensor, xTaskFreq);
  }
}


void getPowerData(void * pvParameters) {
  TickType_t xPreviousWakeTime = xTaskGetTickCount();
  while(1) {
    voltage = 1.2;
    current = 0.34;
    Serial.println("getPowerData");
    vTaskDelayUntil(&xPreviousWakeTime, xTaskFreq);
  }
}

/********************************/

void setup() {
  // put your setup code here, to run once:
  /* Set up IMUs */

  /* Set up current sensor */

  /* Set up semaphores */
  
  /* Set up serial comm */
  Serial.begin(115200); // for debugging purposes
  Serial2.begin(115200);
  
// Bootup handshake process (Arduino side)
// Keep sending to RPi "hello\n" and wait for ACK ("ACK")
// when received ACK, send ACK to RPi

  // Wait for serial port to be available before sending handshake message
  while(!Serial2) {
//  while(!Serial) {
    delay(100);
  }
  
  // Keep waiting for 'H' from RPi
  while(!Serial2.available() || Serial2.read() != 'H') {      // no ACK
//  while(!Serial.available() || Serial.read() != 'H') {
    delay(100);
  }

  // Respond with 'A' from RPi
  Serial2.write('A');
//  Serial.write('A');
  // Wait for 'A' from RPi
  while(!Serial2.available() || Serial2.read() != 'A') {
//  while(!Serial.available() || Serial.read() != 'A') {
    delay(100);
  }
  Serial.println("Connection established");

  xSemaphoreSensors = xSemaphoreCreateBinary();
  xSemaphoreBuffer = xSemaphoreCreateMutex();
  xSemaphoreGive(xSemaphoreSensors);
  xSemaphoreGive(xSemaphoreBuffer);
  
  xTaskCreate(getPowerData, "getPowerData", STACK_SIZE, NULL, 3, NULL);
  xTaskCreate(getSensorData, "getSensorData", STACK_SIZE, NULL, 2, NULL);
  xTaskCreate(uartSend, "uartSend", STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(uartReceive, "uartReceive", STACK_SIZE, NULL, 1, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
}
