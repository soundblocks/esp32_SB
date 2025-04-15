void initEEPROM() {

  EEPROM.begin(EEPROM_SIZE);

  for (int i = 0; i < EEPROM_SIZE; i++) {
    data[i] = byte(EEPROM.read(i));
  }

  for (int i = 0; i < EEPROM_SIZE; i++) {
    // Serial.print("N: ");
    // Serial.print(i);
    // Serial.print(" - ");
    // Serial.println(data[i]);
  }
  
}

void initWIFI() {

  // Serial.println();
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.config(ip, gateway, subnet);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // Serial.print(".");
  }
  // Serial.println("");

  // Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

  // Serial.println("Starting UDP");
  Udp.begin(localPort);
  // Serial.print("Local port: ");

#ifdef ESP32
  // Serial.println(localPort);
#else
  // Serial.println(Udp.localPort());
#endif

  // Serial.println("");

  blinkLED(5);
}

void initDFPlayer() {

  mp3Serial.begin(MP3_SERIAL_SPEED, SWSERIAL_8N1, MP3_RX_PIN, MP3_TX_PIN, false, MP3_SERIAL_BUFFER_SIZE, 0);  //false=signal not inverted, 0=ISR/RX buffer size (shared with serial TX buffer)
  mp3.begin(mp3Serial, MP3_SERIAL_TIMEOUT, DFPLAYER_HW_247A, false);                                          //"DFPLAYER_HW_247A" see NOTE, false=no feedback from module after the command

  mp3.stop();   //if player was runing during ESP8266 reboot
  mp3.reset();  //reset all setting to default

  mp3.setSource(2);   //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash
  mp3.setEQ(0);       //0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  mp3.setVolume(30);  //0..30, module persists volume on power failure

  //mp3.sleep();                            //inter sleep mode, 24mA
  //mp3.wakeup(2);                          //exit sleep mode & initialize source 1=USB-Disk, 2=TF-Card, 3=Aux, 5=NOR Flash
  //mp3Serial.enableRx(true);               //enable interrupts on RX-pin for better response detection, less overhead than mp3Serial.listen()

  //Serial.print("DFPlayer Status: ");
  //Serial.print(mp3.getStatus());  //0=stop, 1=playing, 2=pause, 3=sleep or standby, 4=communication error, 5=unknown state
  //Serial.print(" - Volume: ");
  //Serial.print(mp3.getVolume());  //0..30
  //Serial.print(" - Command: ");
  //Serial.println(mp3.getCommandStatus());  //1=module busy, 2=module sleep, 3=request not fully received, 4=checksum not match, 5=requested folder/track out of range,
  //6=requested folder/track not found, 7=advert available while track is playing, 8=SD card not found, 9=???, 10=module sleep
  //11=OK command accepted, 12=OK playback completed, 13=OK module ready after reboot
  //mp3Serial.enableRx(false);              //disable interrupts on RX-pin, less overhead than mp3Serial.listen()
  //mp3.playMP3Folder(1); //1=track, folder name must be "mp3" or "MP3" & files in folder must start with 4 decimal digits with leading zeros
  //mp3.playFolder(1, 1); //1=folder/2=track, folder name must be 01..99 & files in folder must start with 3 decimal digits with leading zeros
  //mp3.pause();

}

void initMPU6050() {

  digitalWrite(LED_OUTPUT_PIN, HIGH);

  Wire.begin();
  accelgyro.initialize();
  
  devStatus = accelgyro.dmpInitialize();

  // Previously offset values
  // ax_o = accelgyro.getXAccelOffset();
  // ay_o = accelgyro.getYAccelOffset();
  // az_o = accelgyro.getZAccelOffset();
  // gx_o = accelgyro.getXGyroOffset();
  // gy_o = accelgyro.getYGyroOffset();
  // gz_o = accelgyro.getZGyroOffset();

  // while(true) {

  //   accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  //   // Reading filter
  //   f_ax = f_ax - (f_ax >> 5) + ax;
  //   p_ax = f_ax >> 5;

  //   f_ay = f_ay - (f_ay >> 5) + ay;
  //   p_ay = f_ay >> 5;

  //   f_az = f_az - (f_az >> 5) + az;
  //   p_az = f_az >> 5;

  //   f_gx = f_gx - (f_gx >> 3) + gx;
  //   p_gx = f_gx >> 3;

  //   f_gy = f_gy - (f_gy >> 3) + gy;
  //   p_gy = f_gy >> 3;

  //   f_gz = f_gz - (f_gz >> 3) + gz;
  //   p_gz = f_gz >> 3;

  //   //Each 100 read, fix the offset
  //   if (counter == 100) {

  //     Serial.print("Averages:");
  //     Serial.print("\t");
  //     Serial.print(p_ax);
  //     Serial.print("\t");
  //     Serial.print(p_ay);
  //     Serial.print("\t");
  //     Serial.print(p_az);
  //     Serial.print("\t");
  //     Serial.print(p_gx);
  //     Serial.print("\t");
  //     Serial.print(p_gy);
  //     Serial.print("\t");
  //     Serial.println(p_gz);
      
  //     //Calibrar el acelerometro a 1g en el eje z (ajustar el offset)
  //     if (p_ax > 0) ax_o--;
  //     else { ax_o++; }
  //     if (p_ay > 0) ay_o--;
  //     else { ay_o++; }
  //     if (p_az - 16384 > 0) az_o--;
  //     else { az_o++; }

  //     accelgyro.setXAccelOffset(ax_o);
  //     accelgyro.setYAccelOffset(ay_o);
  //     accelgyro.setZAccelOffset(az_o);

  //     //Calibrar el giroscopio a 0º/s en todos los ejes (ajustar el offset)
  //     if (p_gx > 0) gx_o--;
  //     else { gx_o++; }
  //     if (p_gy > 0) gy_o--;
  //     else { gy_o++; }
  //     if (p_gz > 0) gz_o--;
  //     else { gz_o++; }

  //     accelgyro.setXGyroOffset(gx_o);
  //     accelgyro.setYGyroOffset(gy_o);
  //     accelgyro.setZGyroOffset(gz_o);

  //     if (abs(p_ax)<=CALIB_THRESHOLD && abs(p_ay)<=CALIB_THRESHOLD && abs(p_az-16384)<=CALIB_THRESHOLD) {
  //       break;
  //     }

  //     counter = 0;

  //   }

  //   counter++;

  // }

  // Supply your own gyro offsets here, scaled for min sensitivity
  accelgyro.setXGyroOffset(220);
  accelgyro.setYGyroOffset(76);
  accelgyro.setZGyroOffset(-85);
  accelgyro.setZAccelOffset(1788);  // 1688 factory default for my test chip

  if (devStatus == 0) {

    // Calibration Time: generate offsets and calibrate our MPU6050
    accelgyro.CalibrateAccel(6);
    accelgyro.CalibrateGyro(6);
    // Serial.println();
    // accelgyro.PrintActiveOffsets();

    // Turn on the DMP, now that it's ready
    accelgyro.setDMPEnabled(true);
    mpuIntStatus = accelgyro.getIntStatus();

    dmpReady = true;

    // Get expected DMP packet size for later comparison
    packetSize = accelgyro.dmpGetFIFOPacketSize();

    digitalWrite(LED_OUTPUT_PIN, LOW);

  } else {

    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    // Serial.print(F("DMP Initialization failed (code "));
    // Serial.print(devStatus);
    // Serial.println(F(")"));

  }

} 

void blinkLED(int n) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_OUTPUT_PIN, HIGH);
    delay(500);
    digitalWrite(LED_OUTPUT_PIN, LOW);
    delay(500);
  }
}
