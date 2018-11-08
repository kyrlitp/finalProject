int tempo = 200; //this is in ms
int grooveAmount = 50;
int currentStep = 0;
int isEvenBeat = 0;
unsigned long lastStepTime = 0;
unsigned long nextStepTime = 0;
// int acceptableMidiValues[] = {} // need to limit this to 36 to 51 (C1 to D#2)

boolean hardCodedSequence[3][8] = {
  { HIGH, LOW, LOW, LOW, LOW, HIGH, LOW, LOW }, //KICK
  { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH }, //HAT
  { LOW, LOW, HIGH, LOW, LOW, LOW, HIGH, LOW }, //SNARE
};

int midiNotes[3] = { 36 /*kick = C1*/, 42 /*hat = F#1*/, 38 /*sneuh = D1*/ };


//
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  playSequenceMIDI();

}

void playSequenceMIDI() {
  grooveAmount = analogRead(A14)/10; //fix this later, it's kinda janky
  
  if (millis() > nextStepTime) { //THIS IS THE MOMENT time passes onto next step
    //turn things off
    for (int i = 0; i < 3; i++) {
      usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
    }

    //increment
    currentStep = currentStep + 1;
    if ((currentStep % 2) == 0) {
      nextStepTime = millis()+grooveAmount+tempo;
    }
    if ((currentStep % 2) != 0) {
      nextStepTime = millis()-grooveAmount+tempo;
    }
    if (currentStep > 7) { //once step 8 is done...
      currentStep = 0; //it will reset back to step 1.
    }
     // lastStepTime = millis();


    //calculate...
    //and set things on
    for (int i = 0; i < 3; i++) {
      if (hardCodedSequence[i][currentStep] == true) {
        usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
      }
    }
  }
}
