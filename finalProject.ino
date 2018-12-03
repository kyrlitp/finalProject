int tempo = 600; //this is in ms
boolean lastChannelButtonState[2] = { LOW, LOW };
boolean channelButtonState[2] = { LOW, LOW };
int bigBlueButton = 11;
int randomSequence = 0;
int groove = 0;
int grooveAmount = 50;
int currentStep = 0;
int isEvenBeat = 0;
unsigned long lastStepTime = 0;
unsigned long nextStepTime = 0;
int ledPin = 16;
int ledPin2 = 15;
int ledPin3 = 14;
int ledPin4 = 13;
int tempoDetect = 0;
unsigned long beginTempoDetect = 0;
unsigned long endTempoDetect = 0;
int tempoDetected = 0;
byte midi_start = 250;
byte midi_stop = 252;
byte midi_clock = 248;
byte midi_continue = 251;
// int play_flag = 0;
byte data;
int clockAdd = 0;
int previousClockNum = 0;
int MIDICounter = 1;
int TEMPOCounter = 0;

//boolean mySequence[3][8] = {
//  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW }, //KICK
//  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW }, //HAT
//  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW }, //SNARE
//};
boolean hardCodedSequence[6][3][8] = {
  {
    { HIGH, LOW, LOW, LOW, LOW, HIGH, LOW, LOW }, //KICK
    { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH }, //HAT
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW }, //SNARE
  },
  {
    { HIGH, LOW, HIGH, LOW, LOW, LOW, LOW, HIGH },
    { LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH },
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, HIGH },
  },
  {
    { HIGH, LOW, LOW, LOW, HIGH, LOW, LOW, LOW },
    { HIGH, HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW },
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW },
  },
  {
    { HIGH, LOW, HIGH, HIGH, LOW, HIGH, LOW, LOW },
    { HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW },
    { LOW, HIGH, LOW, LOW, HIGH, LOW, HIGH, HIGH },
  },
  {
    { LOW, HIGH, LOW, LOW, HIGH, LOW, HIGH, HIGH },
    { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH },
    { HIGH, LOW, HIGH, HIGH, LOW, HIGH, LOW, LOW },
  },
  {
    { HIGH, LOW, HIGH, LOW, LOW, LOW, LOW, LOW },
    { LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH },
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW },
  },
};


int midiNotes[3] = { 36 /*kick = C1*/, 42 /*hat = F#1*/, 38 /*sneuh = D1*/ };


//
void setup() {
  // list of inputs and outputs
  Serial.begin(31250); //midi sync amnt
  pinMode(bigBlueButton, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  // usbMIDI setup
  //  usbMIDI.setHandleNoteOff(tempoCalculator);
  usbMIDI.setHandleNoteOn(onNoteOn);
  usbMIDI.setHandleNoteOff(onNoteOff);
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
}

void loop() {
  // put your main code here, to run repeatedly:
  LEDs();
  usbMIDI.read(); //allows onNoteOn and onNoteOff to work.
  if (tempoDetected == 1) { // we need to make sure we actually have a tempo before we do anything. (duh.)
    seqRandomizer();
    playSequenceMIDI();
  }
}

void RealTimeSystem(byte realtimebyte) {

  //EVERYTHING THAT HAS TO DO WITH MIDI CLOCK TICKS

  //BEGIN TEMPO DETECTION CODE//
  if (realtimebyte == midi_clock) {
    if (tempoDetected == 0) {
      if (clockAdd == 0) { //ON THE FIRST "TICK"...
        beginTempoDetect = millis(); //SET THE START POINT.
      }
      if (clockAdd == 24) { //ON THE LAST "TICK" (#24)...
        endTempoDetect = millis() - beginTempoDetect; //SET THE END POINT.
        tempo = endTempoDetect / 2; //THE TEMPO IS (1 & 2 & 3 & 4 &)
        groove = (tempo / 2);
        tempoDetected = 1;
      }
    }
    //END TEMPO DETECTION CODE//

    if (clockAdd == 1) {

      //MIDI COUNTER CODE

      if (MIDICounter > 7) {
        MIDICounter = 0;
      }
      MIDICounter++;
      Serial.print("MIDI Counter: ");
      Serial.println(MIDICounter);
    }

    if (clockAdd == 24) {
      clockAdd = 0;
    }

    clockAdd++;


  }
  if (realtimebyte == midi_start || realtimebyte == midi_continue) {
    clockAdd = 0; //IF THE MIDI IS (RE)STARTED, START THE CLOCK TICK AT 0.
    //play_flag = 1;
  }

  if (realtimebyte == midi_stop) { //WHEN MIDI IS STOPPED, RESET EVERYTHING.
    //play_flag = 0;
    //write nested for loop that goes through 8 3 times so that everything is set to low
    tempoDetected = 0;
    currentStep = 0;
    MIDICounter = 0;
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
  }
}

void onNoteOn(byte channel, byte note, byte velocity) {
  digitalWrite(ledPin, HIGH); //any note on turns on led
  //  Serial.println(note);
  //  Serial.println(velocity);
  //  Serial.println("ON");
  //  if (tempoDetected == 1) {
  //    if
  //  }
  if ((note == 36) || (note == 42) || (note == 38)) {
    if (note == 36) {
      note = 0;
    }
    if (note == 42) {
      note = 1;
    }
    if (note == 38) {
      note = 2;
    }

    hardCodedSequence[randomSequence][note][MIDICounter - 1] = HIGH;

    if (note == 0) {
      Serial.println("onNoteOn()");
      // Print to Serial Monitor what the value of the Sequence is.
      Serial.print("hardCodedSequence[randomSequence][");
      Serial.print(note);
      Serial.print("][");
      Serial.print(MIDICounter - 1);
      Serial.println("] = HIGH");
    }

  }
}
void onNoteOff(byte channel, byte note, byte velocity) {
  digitalWrite(ledPin, LOW); //any note on turns on led
  //  Serial.println(note);
  //  Serial.println(velocity);
  //  Serial.println("ON");
  //  if (tempoDetected == 1) {
  //    if
  //  }
  if ((note == 36) || (note == 42) || (note == 38)) {
    if (note == 36) {
      note = 0;
    }
    if (note == 42) {
      note = 1;
    }
    if (note == 38) {
      note = 2;
    }

    hardCodedSequence[randomSequence][note][MIDICounter - 1] = LOW;

    // Print to Serial Monitor what the value of the Sequence is.

    if (note == 0) {
      Serial.print("hardCodedSequence[randomSequence][");
      Serial.print(note);
      Serial.print(note);
      Serial.print("][");
      Serial.print(MIDICounter - 1);
      Serial.println("] = LOW");
    }


  }
}




void seqRandomizer() {
  lastChannelButtonState[0] = channelButtonState[0];
  channelButtonState[0] = digitalRead(bigBlueButton);

  //exact moment when button changes state
  if (channelButtonState[0] == HIGH && lastChannelButtonState[0] == LOW) {
    randomSequence = random(0, 6); //syntax: min (inclusive), max (exclusive)
    currentStep = 0;
  }
}

void LEDs() {
  // BIG BLUE BUTTON LED

  if (digitalRead(bigBlueButton) == HIGH) {
    digitalWrite(ledPin, HIGH);
  }
  else if (digitalRead(bigBlueButton) == LOW) {
    digitalWrite(ledPin, LOW);
  }

  // GROOVE LIGHT LEDS
  if (tempoDetected == 1) {

    if (hardCodedSequence[randomSequence][0][currentStep] == true) {
      digitalWrite(ledPin2, HIGH);
    }
    else if (hardCodedSequence[randomSequence][0][currentStep] == false) {
      digitalWrite(ledPin2, LOW);
    }

    if (hardCodedSequence[randomSequence][1][currentStep] == true) {
      digitalWrite(ledPin3, HIGH);
    }
    else if (hardCodedSequence[randomSequence][1][currentStep] == false) {
      digitalWrite(ledPin3, LOW);
    }

    if (hardCodedSequence[randomSequence][2][currentStep] == true) {
      digitalWrite(ledPin4, HIGH);
    }
    else if (hardCodedSequence[randomSequence][2][currentStep] == false) {
      digitalWrite(ledPin4, LOW);
    }
  }
}

void playSequenceMIDI() {
  grooveAmount = analogRead(A14); //NEEDS TO GET MAPPED
  grooveAmount = map(grooveAmount, 0, 1023, 0, groove);


  if (millis() > nextStepTime) { //THIS IS THE MOMENT time passes onto next step

    //turn things off
    for (int i = 0; i < 3; i++) {
      usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
    }

    //increment
    if (currentStep > 7) { //once step 8 is done...
      currentStep = 0; //it will reset back to step 1.
    }
    currentStep++;

    if ((currentStep % 2) == 0) {
      nextStepTime = millis() + grooveAmount + tempo;
    }
    if ((currentStep % 2) != 0) {
      nextStepTime = millis() - grooveAmount + tempo;
    }

    // lastStepTime = millis();
    Serial.print("Groovebox thinks the step is: ");
    Serial.println(currentStep);


    //calculate...
    //and set things on
    for (int i = 0; i < 3; i++) {
      if (hardCodedSequence[randomSequence][i][currentStep] == true) {
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}
