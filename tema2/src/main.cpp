#include <EEPROM.h>

const int buttonStartStopPin = 3;
const int buttonDifficultyPin = 2;

const int ledRGB_RedPin = 6;
const int ledRGB_GreenPin = 5;
const int ledRGB_BluePin = 4;

unsigned long startTime = 0;
int difficulty = 0;
int wordInterval[] = {3000, 2000, 1000};

bool gameRunning = false;
bool wordDisplayed = false;
int correctWords = 0;
String currentWord;

unsigned long lastButtonPressTime = 0;
const unsigned long debounceDelay = 200;

String words[] = {"apple", "banana", "orange", "grape", "peach", "plum", "melon", "lemon"};
int numWords = sizeof(words) / sizeof(words[0]);

void setup() {
  Serial.begin(9600);
  pinMode(buttonStartStopPin, INPUT_PULLUP);
  pinMode(buttonDifficultyPin, INPUT_PULLUP);
  pinMode(ledRGB_RedPin, OUTPUT);
  pinMode(ledRGB_GreenPin, OUTPUT);
  pinMode(ledRGB_BluePin, OUTPUT);
  setRGBColor(255, 255, 255);
}

void loop() {
  handleButtons();

  if (gameRunning) {
    if (millis() - startTime < 30000) {
      if (!wordDisplayed || (millis() - startTime >= wordInterval[difficulty] * (correctWords + 1))) {
        displayWord();
      }
      checkUserInput();
    } else {
      endRound();
    }
  }
}

void handleButtons() {
  unsigned long currentMillis = millis();

  if (digitalRead(buttonStartStopPin) == LOW && currentMillis - lastButtonPressTime > debounceDelay) {
    lastButtonPressTime = currentMillis;
    if (!gameRunning) startRound();
    else endRound();
  }

  if (digitalRead(buttonDifficultyPin) == LOW && !gameRunning && currentMillis - lastButtonPressTime > debounceDelay) {
    lastButtonPressTime = currentMillis;
    changeDifficulty();
  }
}

void startRound() {
  gameRunning = true;
  correctWords = 0;
  startTime = millis();

  for (int i = 0; i < 3; i++) {
    setRGBColor(255, 255, 0);
    delay(500);
    setRGBColor(0, 0, 0);
    delay(500);
  }
  setRGBColor(0, 255, 0);
  Serial.println("Game started!");
}

void endRound() {
  gameRunning = false;
  wordDisplayed = false;
  setRGBColor(255, 0, 0);
  Serial.print("Game ended! Correct words: ");
  Serial.println(correctWords);
  delay(1000);
  setRGBColor(255, 255, 255);
}

void changeDifficulty() {
  difficulty = (difficulty + 1) % 3;
  const char* difficultyText[] = {"Easy", "Medium", "Hard"};
  Serial.print(difficultyText[difficulty]);
  Serial.println(" mode on!");
}

void setRGBColor(int red, int green, int blue) {
  analogWrite(ledRGB_RedPin, red);
  analogWrite(ledRGB_GreenPin, green);
  analogWrite(ledRGB_BluePin, blue);
}

void displayWord() {
  if (!wordDisplayed) {
    int randomIndex = random(numWords);
    currentWord = words[randomIndex];
    Serial.print("New Word: ");
    Serial.println(currentWord);
    wordDisplayed = true;
    setRGBColor(0, 255, 0);
  }
}

void checkUserInput() {
  if (Serial.available() > 0) {
    String userInput = Serial.readStringUntil('\n');
    userInput.trim();

    if (userInput.equalsIgnoreCase(currentWord)) {
      Serial.println("Correct!");
      correctWords++;
      wordDisplayed = false;
      setRGBColor(0, 255, 0);
    } else {
      Serial.println("Incorrect!");
      setRGBColor(255, 0, 0);
      delay(500);
      setRGBColor(0, 255, 0);
    }
  }
}
