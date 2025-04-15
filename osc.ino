void initprog(OSCMessage &msg) {
  if (programMode) return;

  // Serial.println("start...");
  for (int i = 0; i < EEPROM_SIZE; i++) {
    data[i] = 0;
  }
  nsend = 0;
  nrecv = 0;
  programMode = true;
  digitalWrite(LED_OUTPUT_PIN, HIGH);
}

void send(OSCMessage &msg) {
  if (!programMode) return;

  // Serial.println("send...");
  byte v[SEND_SIZE];
  for (int i = 0; i < SEND_SIZE; i++) {
    v[i] = byte(msg.getInt(i));
    // Serial.print(v[i]);
    // Serial.print(" ");
    data[nsend * SEND_SIZE + i] = v[i];
  }
  // Serial.println();
  nsend++;
}

void receive(OSCMessage &msg) {
  if (!programMode) return;

  //Serial.println("receive...");
  byte v[RECV_SIZE];
  for (int i = 0; i < RECV_SIZE; i++) {
    v[i] = byte(msg.getInt(i));
    // Serial.print(v[i]);
    // Serial.print(" ");
    data[RECV_START + nrecv * RECV_SIZE + i] = v[i];
  }
  Serial.println();
  nrecv++;
}

void commit(OSCMessage &msg) {
  if (!programMode) return;

  // Serial.println("commit...");
  data[NSEND_BYTE] = nsend;
  data[NRECV_BYTE] = nrecv;
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, data[i]);
  }
  EEPROM.commit();
}

void verify(OSCMessage &msg) {
  if (!programMode) return;

  // Serial.println("check...");
}

void endprog(OSCMessage &msg) {
  if (!programMode) return;

  // Serial.println("end...");
  programMode = false;
  digitalWrite(LED_OUTPUT_PIN, LOW);
  // Serial.println();
  // for (int i = 0; i < EEPROM_SIZE; i++) {
  //   Serial.print("N: ");
  //   Serial.print(i);
  //   Serial.print(" - ");
  //   Serial.println(data[i]);
  // }
  // Serial.println();
}

void sensor(OSCMessage &msg) {
  if (programMode) return;

  int id = msg.getInt(0);
  int m = msg.getInt(1);
  int val = msg.getInt(2);
  // Serial.print("ID: ");
  // Serial.print(id);
  // Serial.print("   ");
  // Serial.print("Sensor: ");
  // Serial.print(m);
  // Serial.print(" - ");
  // Serial.println(val);

  for (int n = 0; n < data[NRECV_BYTE]; n++) {
    byte id_sensor = data[RECV_START + n * RECV_SIZE];
    byte id_out = data[RECV_START + n * RECV_SIZE + 1];

    if (id == id_out && m == id_sensor) {
      byte id_act = data[RECV_START + n * RECV_SIZE + 2];
      byte map1 = data[RECV_START + n * RECV_SIZE + 3];
      byte map2 = data[RECV_START + n * RECV_SIZE + 4];

      if (map1==255 && map2==255) {
        doAction(id_act, val);
      } else if (map1==255 && map2!=255) {
        if (val>0) {
          doAction(id_act, map2);
        } else {
          doAction(id_act, val);
        }
      } else {
        doAction(id_act, map(val, 0, 127, map1, map2));
      }
    }
  }
}

void sendSensorOSC(int m, IPAddress ip) {
  int val = readSensor(m);

  if (val != -1) {
    OSCMessage msg("/sensor");
    msg.add(ID);
    msg.add(m);
    msg.add(val);
    Udp.beginPacket(ip, outPort);
    msg.send(Udp);
    Udp.endPacket();
    msg.empty();

    // Serial.print("IP: ");
    // Serial.print(ip);
    // Serial.print("    ");
    // Serial.print("Sensor: ");
    // Serial.print(m);
    // Serial.print(" - ");
    // Serial.println(val);
  }
}