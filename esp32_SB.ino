/**

Versions:
  Board:
    - esp32 by Espressif, 1.0.5
  Librarys:
    - OSC by Adrian Freed, 1.3.7
    - EspSoftwareSerial by Dirk Kaar, Peter Lerup, 7.0.0
    - DFPlayer Mini by enjoyneering
    - MPU6050 by Electronic Cats, 1.4.3

(C) 2024-2025

Laurence Bender
Germán Ito
Sabrina García

**/

#include "EEPROM.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <SoftwareSerial.h>
#include <DFPlayer.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"

#define ID 129

#define EEPROM_SIZE 512
#define SEND_SIZE 8
#define RECV_SIZE 5

#define NSEND_BYTE 510
#define NRECV_BYTE 511
#define RECV_START 360
#define IP_RANGES 3
#define NSENSORS 16

#define MP3_RX_PIN 16              //GPIO4/D2 to DFPlayer Mini TX
#define MP3_TX_PIN 17              //GPIO5/D1 to DFPlayer Mini RX
#define MP3_SERIAL_SPEED 9600      //DFPlayer Mini suport only 9600-baud
#define MP3_SERIAL_BUFFER_SIZE 32  //software serial buffer size in bytes, to send 8-bytes you need 11-bytes buffer (start byte+8-data bytes+parity-byte+stop-byte=11-bytes)
#define MP3_SERIAL_TIMEOUT 350     //average DFPlayer response timeout 200msec..300msec for YX5200/AAxxxx chip & 350msec..500msec for GD3200B/MH2024K chip

#define LED_OUTPUT_PIN 2
#define TOUCH_THRESHOLD 30
#define NUMREADS 10
#define READDELAY 500
#define CALIB_THRESHOLD 10

#define SENDINTERVAL 50
#define LOOPINTERVAL 250

byte data[EEPROM_SIZE];

char ssid[] = "soundblocks";  // Network SSID (name)
char pass[] = "soundblocks";  // Network password

WiFiUDP Udp;                           // A UDP instance to let us send and receive packets over UD
const unsigned int outPort = 12000;    // remote port (not needed for receive)
const unsigned int localPort = 12000;  // local port to listen for UDP packets (here's where we send the packets)

IPAddress ip(192, 168, 10, ID);  // IP de la ESP
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0);

OSCErrorCode error;

bool programMode = false;
int nsend = 0;
int nrecv = 0;

unsigned long sendReftime = millis();
unsigned long sendInterval = SENDINTERVAL;

unsigned long loopReftime = millis();
unsigned long loopInterval = LOOPINTERVAL;

SoftwareSerial mp3Serial;
DFPlayer mp3;

bool flagPlay = false;
bool flagResume = false;
int trackDF = 0;

MPU6050 accelgyro;

// MPU control/status vars
bool dmpReady = false;   // set true if DMP init was successful
uint8_t mpuIntStatus;    // holds actual interrupt status byte from MPU
uint8_t devStatus;       // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;     // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;      // count of all bytes currently in FIFO
uint8_t fifoBuffer[64];  // FIFO storage buffer

// Orientation/motion vars
Quaternion q;         // [w, x, y, z]         quaternion container
VectorInt16 aa;       // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;   // [x, y, z]            gravity-free accel sensor measurements
VectorFloat gravity;  // [x, y, z]            gravity vector
float ypr[3];         // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

float yaw = 0.0;
float pitch = 0.0;
float roll = 0.0;

int16_t ax = 0;
int16_t ay = 0;
int16_t az = 0;

int16_t gx, gy, gz;

long f_ax, f_ay, f_az, f_gx, f_gy, f_gz;
int p_ax, p_ay, p_az, p_gx, p_gy, p_gz;
int counter = 0;

int ax_o, ay_o, az_o, gx_o, gy_o, gz_o;

void setup() {

  Serial.begin(115200);
  delay(2000);

  pinMode(LED_OUTPUT_PIN, OUTPUT);
  digitalWrite(LED_OUTPUT_PIN, LOW);

  initEEPROM();
  initWIFI();
  initDFPlayer();
  initMPU6050();

  touchSetCycles(0x1000, 0x2000);
}

void loop() {

  // OSC messages

  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }

    if (!msg.hasError()) {
      msg.dispatch("/initprog", initprog);
      msg.dispatch("/send", send);
      msg.dispatch("/receive", receive);
      msg.dispatch("/commit", commit);
      msg.dispatch("/verify", verify);
      msg.dispatch("/endprog", endprog);
      msg.dispatch("/sensor", sensor);
    } else {
      error = msg.getError();
      // Serial.print("error: ");
      // Serial.println(error);
    }
  }

  // Accelerometer/Gyroscope MPU6050

  accelgyro.getRotation(&gx, &gy, &gz);

  if (dmpReady) {

    // Read a packet from FIFO
    if (accelgyro.dmpGetCurrentFIFOPacket(fifoBuffer)) {  // Get the Latest packet

      // Get YPR angles in degrees
      accelgyro.dmpGetQuaternion(&q, fifoBuffer);
      accelgyro.dmpGetGravity(&gravity, &q);
      accelgyro.dmpGetYawPitchRoll(ypr, &q, &gravity);
      yaw = ypr[0] * 180 / M_PI;
      if (yaw < 0.0) yaw *= -1.0;
      pitch = ypr[1] * 180 / M_PI;
      if (pitch < 0.0) pitch *= -1.0;
      roll = ypr[2] * 180 / M_PI;
      if (roll < 0.0) roll *= -1.0;

      // Get real acceleration, adjusted to remove gravity
      accelgyro.dmpGetQuaternion(&q, fifoBuffer);
      accelgyro.dmpGetAccel(&aa, fifoBuffer);
      accelgyro.dmpGetGravity(&gravity, &q);
      accelgyro.dmpGetLinearAccel(&aaReal, &aa, &gravity);
      ax = aaReal.x;
      ay = aaReal.y;
      az = aaReal.z;
    }
  }

  if (!programMode) {

    // Loop audio

    if (millis() - loopReftime >= loopInterval) {

      if (flagPlay && mp3.getStatus() == 0) {
        mp3.playFolder(1, trackDF);
      }

      loopReftime = millis();
    }

    if (millis() - sendReftime >= sendInterval) {

      for (int n = 0; n < data[NSEND_BYTE]; n++) {
        for (int i = 0; i < IP_RANGES; i++) {
          byte idini = data[n * SEND_SIZE + 2 + 2 * i];
          byte idend = data[n * SEND_SIZE + 2 + 2 * i + 1];

          if (idini == 0 && idend == 0) break;

          for (int k = idini; k <= idend; k++) {
            IPAddress outIp(192, 168, 10, k);

            int sensors = (data[n * SEND_SIZE] << 8) | data[n * SEND_SIZE + 1];
            for (int m = 0; m < NSENSORS; m++) {
              if (bitRead(sensors, m) == 1) sendSensorOSC(m, outIp);
            }
          }
        }
      }

      sendReftime = millis();

      // Serial.print("X accel: ");
      // Serial.print(ax);
      // Serial.print(" - Y accel: ");
      // Serial.print(ay);
      // Serial.print(" - Z accel: ");
      // Serial.println(az);
      // Serial.print("X ang vel: ");
      // Serial.print(gx);
      // Serial.print(" - Y ang vel: ");
      // Serial.print(gz);
      // Serial.print(" - Z ang vel: ");
      // Serial.println(gz);
      // Serial.print("Yaw: ");
      // Serial.print(yaw);
      // Serial.print("  -  Pitch: ");
      // Serial.print(pitch);
      // Serial.print("  -  Roll: ");
      // Serial.println(roll);
    }
  }
}
