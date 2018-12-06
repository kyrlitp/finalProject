//TEENSY GROOVEBOX, by Jalen Sariñana Edington

//PIN MAPPINGS

int ledPin = 16;
int ledPin2 = 15;
int ledPin3 = 14;
int ledPin4 = 13;
int bigBlueButton = 11;

//TIME-BASED VARIABLES (tempo, groove, etc.)

////TEMPO DETECTION
int tempoDetect = 0;
unsigned long beginTempoDetect = 0;
unsigned long endTempoDetect = 0;
int tempoDetected = 0;

////TEMPO/GROOVE VALUES (variables here are placeholders)
int tempo = 600;
int groove = 0;
int grooveAmount = 50;

//COUNTERS, etc.
int currentStep = 0; //Variable that determines what step we're on.
unsigned long nextStepTime = 0; //We're going to need this to determine when our next step is.
boolean lastChannelButtonState[2] = { LOW, LOW };
boolean channelButtonState[2] = { LOW, LOW };
int randomSequence = 0; //This variable will change the selected drum pattern the user has selected.
int clockAdd = 0; //This shows us what step the MIDI Clock is on (hint: 24 steps in a quarter note!).


//MIDI MAPPINGS THAT realTimeSystem() NEEDS

byte midi_start = 250;
byte midi_stop = 252;
byte midi_clock = 248;
byte midi_continue = 251;

//MIDI VALUES (drum patterns, midi notes, etc.)

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


int midiNotes[3] = { 36 /*Kick = C1*/, 42 /*Hat = F#1*/, 38 /*Snare = D1*/ };



void setup() {
  // List of Inputs and Outputs
  Serial.begin(31250); //MIDI Sync Amount.
  pinMode(bigBlueButton, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);
  
  // usbMIDI setup
  usbMIDI.setHandleRealTimeSystem(RealTimeSystem);
}

void loop() {
  LEDs();
  usbMIDI.read(); //allows realTimeSystem to work.
  if (tempoDetected == 1) { // We need to make sure we actually have a tempo before we do anything. (duh.)
    seqRandomizer();
    playSequenceMIDI();
  }
}

void RealTimeSystem(byte realtimebyte) { //This function reads the MIDI Messages "Stop", "Start", "Continue" and "Clock" in REALTIME (hence the name...). We need these to map our tempo and turn the program on or off.

  //EVERYTHING THAT HAS TO DO WITH MIDI CLOCK TICKS

  //TEMPO DETECTION
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
    
    //COUNTERS

    if (clockAdd == 24) {
      clockAdd = 0;
    }
    
    clockAdd++;
  }
  
  if (realtimebyte == midi_start || realtimebyte == midi_continue) {
    clockAdd = 0; //IF THE MIDI IS (RE)STARTED, START THE CLOCK TICK AT 0.
  }

  if (realtimebyte == midi_stop) { //WHEN MIDI IS STOPPED, TURN EVERYTHING OFF!
    tempoDetected = 0; //We need to turn this off so we can re-detect the tempo in case the user wants to change it.
    currentStep = 0; //Same as above.
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, LOW);
    digitalWrite(ledPin4, LOW);
  }
}

void seqRandomizer() { //This function generates a random number when the big blue button is hit that maps to an existing drum pattern so it can be played.
  lastChannelButtonState[0] = channelButtonState[0];
  channelButtonState[0] = digitalRead(bigBlueButton);

  //This is the exact moment when button changes state
  if (channelButtonState[0] == HIGH && lastChannelButtonState[0] == LOW) {
    randomSequence = random(0, 6); //syntax: min (inclusive), max (exclusive)
    currentStep = 0;
  }
}

void LEDs() { //This function turns on or off our LEDs when sets of events happen.
  // BIG BLUE BUTTON LED

  if (digitalRead(bigBlueButton) == HIGH) {
    digitalWrite(ledPin, HIGH);
  }
  else if (digitalRead(bigBlueButton) == LOW) {
    digitalWrite(ledPin, LOW);
  }

  // GROOVE LIGHT LEDS
  
  if (tempoDetected == 1) { //Remember, we only want these Groove lights to go on if there's actually anything being played...

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

void playSequenceMIDI() { //This function plays our MIDI Sequence, and correctly moves onto the next step when it is appropriate. This also is where the "groove" is contained.
  grooveAmount = analogRead(A14); //NEEDS TO GET MAPPED
  grooveAmount = map(grooveAmount, 0, 1023, 0, groove);


  if (millis() > nextStepTime) { //This is only true the MOMENT the time passes onto the next step. Without this message, we would constantly be receiving and sending messages and crash the program.

    //Before anything is played, it's a good idea to send a Note Off just in case.
    for (int i = 0; i < 3; i++) {
      usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
    }

    //STEP SEQUENCING
    
    if (currentStep > 7) { //once step 8 is done...
      currentStep = 0; //it will reset back to step 1.
    }
    currentStep++;

    //GROOVE
    
    if ((currentStep % 2) == 0) {
      nextStepTime = millis() + grooveAmount + tempo;
    }
    if ((currentStep % 2) != 0) {
      nextStepTime = millis() - grooveAmount + tempo;
    }

    //On our current step, we want to play the value LOW or HIGH for each of our MIDI Notes.
    for (int i = 0; i < 3; i++) {
      if (hardCodedSequence[randomSequence][i][currentStep] == true) {
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}
