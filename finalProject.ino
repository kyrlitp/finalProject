int tempo = 600; //this is in ms
boolean lastChannelButtonState[2] = { LOW, LOW };
boolean channelButtonState[2] = { LOW, LOW };
int randomButton1 = 11;
int randomSequence = 0;
int groove = 0;
int grooveAmount = 50;
int currentStep = 0;
int isEvenBeat = 0;
unsigned long lastStepTime = 0;
unsigned long nextStepTime = 0;
int timeAcross4Hats = 0;
int ledPin = 13;
int tempoDetect = 0;
unsigned long beginTempoDetect = 0;
unsigned long endTempoDetect = 0;
int tempoDetected = 0;

// int acceptableMidiValues[] = {} // need to limit this to 36 to 51 (C1 to D#2)

boolean hardCodedSequence[3][3][8] = {
  {
    { HIGH, LOW, LOW, LOW, LOW, HIGH, LOW, LOW }, //KICK
    { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH }, //HAT
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW }, //SNARE
  },
  {
    { HIGH, LOW, HIGH, LOW, LOW, LOW, LOW, HIGH }, //KICK
    { LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, HIGH}, //HAT
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW }, //SNARE
  },
  {
    { LOW, HIGH, LOW, LOW, HIGH, LOW, LOW, HIGH }, //KICK
    { HIGH, HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW }, //HAT
    { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW }, //SNARE
  }
};

int midiNotes[3] = { 36 /*kick = C1*/, 42 /*hat = F#1*/, 38 /*sneuh = D1*/ };


//
void setup() {
  // list of inputs and outputs
  Serial.begin(9600);
  pinMode(randomButton1, INPUT);
  pinMode(ledPin, OUTPUT);

  // usbMIDI setup
  usbMIDI.setHandleNoteOff(tempoCalculator);
  usbMIDI.setHandleNoteOn(onNoteOn);

}

void loop() {
  // put your main code here, to run repeatedly:
  usbMIDI.read(); //allows onNoteOn and onNoteOff to work.
  if (tempoDetected == 1) { // we need to make sure we actually have a tempo before we do anything. (duh.)
    seqRandomizer();
    playSequenceMIDI();
  }
}



void tempoCalculator(byte channel, byte note, byte velocity) {
  digitalWrite(ledPin, LOW); //any note off turns off led
  //  Serial.println(note);
  //  Serial.println(velocity);
  //  Serial.println("OFF");
  if (tempoDetected == 0) {
    if (note == 42) {
      Serial.println("THAT'S THE HAT!!!");
      tempoDetect++;
      if (tempoDetect == 1) {
        beginTempoDetect = millis();
        Serial.print(beginTempoDetect);
      }
      if (tempoDetect == 4) {
        endTempoDetect = (millis() - beginTempoDetect);
        Serial.print(endTempoDetect);
        Serial.print(" milliseconds across 4 hat hits.");
        tempoDetected = 1;
      }
      tempo = (endTempoDetect / 4);
      groove = (tempo / 2);
    }
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

}

void arrayStorePattern() {

}


void seqRandomizer() {
  lastChannelButtonState[0] = channelButtonState[0];
  channelButtonState[0] = digitalRead(randomButton1);

  //exact moment when button changes state
  if (channelButtonState[0] == HIGH && lastChannelButtonState[0] == LOW) {
    randomSequence = random(0, 3); //syntax: min (inclusive), max (exclusive)
    Serial.println(randomSequence);
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
    currentStep = currentStep + 1;
    if ((currentStep % 2) == 0) {
      nextStepTime = millis() + grooveAmount + tempo;
    }
    if ((currentStep % 2) != 0) {
      nextStepTime = millis() - grooveAmount + tempo;
    }
    if (currentStep > 7) { //once step 8 is done...
      currentStep = 0; //it will reset back to step 1.
    }
    // lastStepTime = millis();


    //calculate...
    //and set things on
    for (int i = 0; i < 3; i++) {
      if (hardCodedSequence[randomSequence][i][currentStep] == true) {
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}
