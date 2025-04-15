// acts = ['dfPlay', 'dfPause', 'dfStop', 'dfResume', 'dfSetEq', 'dfVolume', 'dfPlayStop', 'dfPlayPause', 'dfPlayTouch']

void doAction(int act, int val) {

  // Serial.print("Act: ");
  // Serial.print(act);
  // Serial.print("    ");
  // Serial.print("Val: ");
  // Serial.println(val);

  switch (act) {
    case 0:
      if (val >= 1) {
        mp3.playFolder(1,val);
        trackDF = val;
        flagPlay = true;
      }
      break;
    case 1:
      if (val >= 1) {
        mp3.pause();
        flagPlay = false;
      }
      break;
    case 2:
      if (val >= 1) {
        mp3.stop();
        flagPlay = false;
      }
      break;
    case 3:
      if (val >= 1) {
        mp3.resume();
        flagPlay = true;
      }
      break;
    case 4:
      if (val >= 1) {
        mp3.setEQ(constrain(val-1,0,5));
      }
      break;
    case 5:
      if (val >= 1) {
        mp3.setVolume(constrain(val-1,0,30));
      }
      break;
    case 6:
      if (flagPlay==false && val>=1) {
        mp3.playFolder(1,val);
        trackDF = val;
        flagPlay = true;
      } else if (flagPlay==true && val>=1) {
        mp3.stop();
        flagPlay = false;
      }
      break;
    case 7:
      if (flagPlay==false && flagResume==false && val>=1) {
        mp3.playFolder(1,val);
        trackDF = val;
        flagPlay = true;
        flagResume = false;
      } else if (flagPlay==true && val >=1) {
        mp3.pause();
        flagPlay = false;
        flagResume = true;
      } else if (flagPlay==false && flagResume==true && val>=1) {
        mp3.resume();
        flagPlay = true;
        flagResume = false;
      }
      break;
    case 8:
      if (flagPlay==false && val>=1) {
        mp3.playFolder(1,val);
        trackDF = val;
        flagPlay = true;
      } else if (flagPlay==true && val<=0) {
        mp3.stop();
        flagPlay = false;
      }
      break;
  }

}