int readTouch(int pin) {
  int val = 0;
  for (int i = 0; i < NUMREADS; i++) {
    val += touchRead(pin);
    delayMicroseconds(READDELAY);
  }
  return (val / NUMREADS);
}

// sens = ['t1','t2','t3','t4','t5','t6','t7','t8','ax','ay','az','gx','gy','gz','azimuth','bearing','shake']

int readSensor(int m) {
  byte t;
  switch (m) {
    case 0:
      t = readTouch(4);
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 1:
      t = readTouch(15);
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 2:
      t = readTouch(13); 
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 3:
      t = readTouch(12); 
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 4:
      t = readTouch(14);
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 5:
      t = readTouch(27);
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 6:
      t = readTouch(33);
      if (t < TOUCH_THRESHOLD) {
        return(127);
      } else {
        return(0);
      }
      break;
    case 7:
      return(constrain(map(ax,-5000,5000,1,127),1,127));
      break;
    case 8:
      return(constrain(map(ay,-5000,5000,1,127),1,127));
      break;
    case 9:
      return(constrain(map(az,-5000,5000,1,127),1,127));
      break;
    case 10:
      return(constrain(map(gx,-5000,5000,1,127),1,127));
      break;
    case 11:
      return(constrain(map(gy,-5000,5000,1,127),1,127));
      break;
    case 12:
      return(constrain(map(gz,-5000,5000,1,127),1,127));
      break;
    case 13:
      return(map(yaw,0,180,1,127));
      break;
    case 14:
      return(map(pitch,0,180,1,127));
      break;
    case 15:
      return(map(roll,0,180,1,127));
      break;
  }
  return (-1);
}