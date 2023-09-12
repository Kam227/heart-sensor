// Heart rate monitor variables
volatile int BPM;
volatile int Signal;
volatile int Pulse;
volatile int IBI;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("Please enter your age:");
  while (Serial.available() == 0) {
    // wait for user input
  }
  int age = Serial.parseInt(); // read the user's age from the Serial input
  Serial.print("You entered: ");
  Serial.println(age);
  // do something with the age, such as store it in a variable or use it in a calculation
}
// Lie detector variables
int potentiometerPin = A0;
int potValue;
int mappedValue;
int buttonPin = 2;
volatile bool displayEnabled = true;

void setup() {
  Serial.begin(115200);
  interruptSetup();
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), disableDisplay, FALLING);
}

void loop() {
  // Heart rate monitor code
  int a = digitalRead(2);
  int before = BPM;

  if (a == 1) {
    delay(3000);
    int after = BPM;

    if (before == after) {
      digitalWrite(4, HIGH);
      delay(100);
      digitalWrite(4, LOW);
      delay(100);
    } else {
      digitalWrite(8, HIGH);
      delay(100);
      digitalWrite(8, LOW);
      delay(100);
    }
  }

  // Lie detector code
  if (displayEnabled) {
    potValue = analogRead(potentiometerPin);
    mappedValue = map(potValue, 0, 1023, 1, 100);
    Serial.println(mappedValue);

    // Thresholds for different age groups
    int upperThreshold = 170;
    int lowerThreshold = 100;

    if (mappedValue <= 20) {
      upperThreshold = 170;
      lowerThreshold = 100;
    } else if (mappedValue < 30) {
      upperThreshold = 162;
      lowerThreshold = 95;
    } else if (mappedValue < 35) {
      upperThreshold = 157;
      lowerThreshold = 93;
    } else if (mappedValue < 40) {
      upperThreshold = 153;
      lowerThreshold = 90;
    } else if (mappedValue < 45) {
      upperThreshold = 149;
      lowerThreshold = 88;
    } else if (mappedValue < 50) {
      upperThreshold = 145;
      lowerThreshold = 85;
    } else if (mappedValue < 55) {
      upperThreshold = 140;
      lowerThreshold = 83;
    } else if (mappedValue < 60) {
      upperThreshold = 136;
      lowerThreshold = 80;
    }

    // Heart rate monitor readings
    if (Pulse == true) {
      digitalWrite(4, HIGH);
      delay(100);
      digitalWrite(4, LOW);
      delay(100);
      BPM = 60000 / IBI;
      if (BPM < upperThreshold && BPM > lowerThreshold) {
        Serial.print("Truth, ");
        Serial.println(BPM);
      } else {
        Serial.print("Lie, ");
        Serial.println(BPM);
      }
      Pulse = false;
    }
  }
}

void interruptSetup() {
  cli();
  TCCR2A = 0x02;
  TCCR2B = 0x06;
  OCR2A = 0x7C;
  TIMSK2 = 0x02;
  sei();
}

ISR(TIMER2_COMPA_vect) {
  cli();  // disable interrupts during ISR

  // read the signal from the sensor
  Signal = analogRead(0);
  
  // check if a pulse is detected
  if (Signal < 512) {
    IBI = sampleCounter - lastBeatTime;
    lastBeatTime = sampleCounter;
    if (secondBeat) {
      secondBeat = false;
      for (int i = 0; i <= 9; i++) {
        rate[i] = IBI;
      }
    }
    if (firstBeat) {
      firstBeat = false;
      secondBeat = true;
      sei(); // enable interrupts
      return;
    }
    int runningTotal = 0;
    for (int i = 0; i <= 8; i++) {
      rate[i] = rate[i + 1];
      runningTotal += rate[i];
    }
    rate[9] = IBI;
    runningTotal += rate[9];
    runningTotal /= 10;
    BPM = 60000 / runningTotal;
    Pulse = true;
  }
  sei(); // enable interrupts at the end of ISR
}