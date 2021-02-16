#define redLightPin 5
#define yellowLightPin 6
#define greenlightPin 7

#define mode1Pin A2
#define mode2Pin A3
#define mode3Pin A4
#define buttonPin A5

#define changeDelay 600
// stay yellow for 3 seconds always

#define MANUAL_MODE 0
#define SUPER_MANUAL 1
#define AUTO_FAST 2
#define RED_LIGHT_GREEN_LIGHT 3

int state;
int countSinceChange;
int stayOnGreenCount; // auto mode: time between changing to yellow; game mode: minimum time to stay green; auto_mode: minimum time before next button press checked
int stayOnRedCount; // auto mode: time between changing to Green; game mode: minimum time to stay red; auto_mode: minimum time before next button press checked
int mode;
bool gameStarted;
int blinkCounter;
bool buttonLastState;
int nextTimeToChange;

void updateGameState(){
  if (gameStarted)
  {
    if (state == 0){ // display green
      digitalWrite(redLightPin, LOW);
      digitalWrite(yellowLightPin, LOW);
      digitalWrite(greenlightPin, HIGH);
    }
    else if (state == 1){ // display yellow
      digitalWrite(redLightPin, LOW);
      digitalWrite(greenlightPin, LOW);
      digitalWrite(yellowLightPin, HIGH);
    }
    else{ // display red
      digitalWrite(yellowLightPin, LOW);
      digitalWrite(greenlightPin, LOW);
      digitalWrite(redLightPin, HIGH);
    }
  }
  else{
    if (blinkCounter < 100){
      digitalWrite(redLightPin, LOW);
      digitalWrite(greenlightPin, LOW);
      digitalWrite(yellowLightPin, LOW);
    }
    else if (blinkCounter > 200) {
      digitalWrite(redLightPin, HIGH);
      digitalWrite(greenlightPin, HIGH);
      digitalWrite(yellowLightPin, HIGH);
    }

    blinkCounter++;

    if (blinkCounter > 300){
      blinkCounter = 0;
      // Serial.println("Resetting blink");
    }
  }
}

void updateState(){
  if (state == 0){ // display green
    digitalWrite(redLightPin, LOW);
    digitalWrite(yellowLightPin, LOW);
    digitalWrite(greenlightPin, HIGH);
  }
  else if (state == 1){ // display yellow
    digitalWrite(redLightPin, LOW);
    digitalWrite(greenlightPin, LOW);
    digitalWrite(yellowLightPin, HIGH);
  }
  else{ // display red
    digitalWrite(yellowLightPin, LOW);
    digitalWrite(greenlightPin, LOW);
    digitalWrite(redLightPin, HIGH);
  }
}

void checkGameButton() {
  bool buttonPressed = (digitalRead(buttonPin) == HIGH);
  if (buttonPressed && !buttonLastState)
  {
    buttonLastState = true;
    // Serial.print("Button pressed");
  }
  if (!buttonPressed && buttonLastState){
    // button was presseed and released, actually do the toggle of the game starting or stopping
    // Serial.print("Button Released");
    buttonLastState = false;
    gameStarted = !gameStarted;
    countSinceChange = 0;
    state = 0;
    nextTimeToChange = stayOnGreenCount;
  }
}

void checkGameTimer(){
  if (gameStarted){
    if (nextTimeToChange < countSinceChange){
      // Serial.print("Time: ");
      // Serial.print(nextTimeToChange);
      // Serial.println(" Expired.");
      int randomMultiplier = random(1,4); // number from 1 - 4 (works out to be 3-12 seconds green, 1-4 seconds red)
      // time to change!
      if (state == 0){
        // we are on green, change to red, reset counter, and set next time to random multiple of red count
        state = 2;
        countSinceChange = 0;
        nextTimeToChange = randomMultiplier * stayOnRedCount;
        // Serial.print("Changing to red for:");
        // Serial.println(nextTimeToChange);
      }
      else
      {
        // we are on red, change to green, reset counter, and set next time to random multiple of green count
        state = 0;
        countSinceChange = 0;
        nextTimeToChange = randomMultiplier * stayOnGreenCount;
        // Serial.print("Changing to Green for:");
        // Serial.println(nextTimeToChange);
      }
    }
  }
}

void iterateGameCounter(){
  if (countSinceChange <= nextTimeToChange)
  {
    countSinceChange++;
  }
}


void checkSuperManualButton() {
  bool buttonPressed = (digitalRead(buttonPin) == HIGH);
  if (buttonPressed && !buttonLastState)
  {
    buttonLastState = true;
    // Serial.print("Button pressed");
  }
  if (!buttonPressed && buttonLastState){
    // button was presseed and released, actually do the toggle of the light
    // Serial.print("Button Released");
    buttonLastState = false;
    countSinceChange = 0;
    state++;
    if (state > 2){
      state = 0;
    }
    nextTimeToChange = stayOnGreenCount;
  }
}

void checkButton(){
  if (digitalRead(buttonPin) == HIGH)
  {
    if (state == 0 && countSinceChange > stayOnGreenCount) {
      // currently on green and time has expired for green counter
      state = 1;
      countSinceChange = 0;
      // Serial.println("Check Button Button Pressed on green");
    }
    else if (state == 2 && countSinceChange > stayOnRedCount) {
      state = 0;
      countSinceChange = 0;
      // Serial.println("Check Button Button Pressed on red");
    }
  }
  
  if (state == 1 && countSinceChange > changeDelay){
    state = 2;
    countSinceChange = 0;
    // Serial.println("Check Button Yellow Expired");
  }
}

void checkTimer() {
  if (state == 0 && countSinceChange > stayOnGreenCount) {
    // currently on green and time has expired for green counter
    state = 1;
    countSinceChange = 0;
  }
  else if (state == 1 && countSinceChange > changeDelay) {
    state = 2;
    countSinceChange = 0;
  }
  else if (state == 2 && countSinceChange > stayOnRedCount) {
    state = 0;
    countSinceChange = 0;
  }
}

void iterateCounter(){
  if (state == 1 && countSinceChange <= changeDelay){
    countSinceChange++;
  }
  else if (state == 0 && countSinceChange <= stayOnGreenCount) {
    countSinceChange++;
  }
  else if (state == 2 && countSinceChange <= stayOnRedCount) {
    countSinceChange++;
  }
}

int determineMode(){
  if (digitalRead(mode1Pin) == HIGH){
    stayOnGreenCount = 100; // 0.5 seconds
    stayOnRedCount = 100; // 0.5 seconds
    return SUPER_MANUAL;
  }
  else if (digitalRead(mode2Pin) == HIGH){
    stayOnGreenCount = 1000; // 5 seconds
    stayOnRedCount = 1000; // 5 seconds
    return AUTO_FAST;
  }
  else if (digitalRead(mode3Pin) == HIGH){
    stayOnGreenCount = 600; // 3 seconds
    stayOnRedCount = 200; // 1 seconds
    return RED_LIGHT_GREEN_LIGHT;
  }
  stayOnGreenCount = 200; // 1 second
  stayOnRedCount = 200; // 1 second
  return MANUAL_MODE;
}

void setup() {
  // Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(redLightPin, OUTPUT);
  digitalWrite(redLightPin, LOW);
  pinMode(yellowLightPin, OUTPUT);
  digitalWrite(yellowLightPin, LOW);
  pinMode(greenlightPin, OUTPUT);
  digitalWrite(greenlightPin, LOW);
  pinMode(mode1Pin, INPUT);
  pinMode(mode2Pin, INPUT);
  pinMode(mode3Pin, INPUT);
  pinMode(buttonPin, INPUT);
  countSinceChange = 0;
  state = 0;
  gameStarted = false;
  blinkCounter = 0;
  buttonLastState = false;
  nextTimeToChange = 0;
  mode = determineMode();
  // Serial.print("Mode: ");
  // Serial.println(mode);
  // Serial.print("Stay On Green: ");
  // Serial.print(stayOnGreenCount);
  // Serial.print("; Stay On Red: ");
  // Serial.println(stayOnRedCount);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mode == RED_LIGHT_GREEN_LIGHT) {
    // game mode
    checkGameButton();
    checkGameTimer();
    updateGameState();
    iterateGameCounter();
  }
  else if (mode == AUTO_FAST) {
    // automatic changing
    checkTimer();
    updateState();
    iterateCounter();
  }
  else if (mode == SUPER_MANUAL) {
    checkSuperManualButton();
    updateState();
    iterateGameCounter();
  }
  else {
    // manual
    checkButton();
    updateState();
    iterateCounter();
  }

  delay(5);
}
