// Подключаем библиотеку FastLED.
#include "FastLED.h"
 
// Указываем, какое количество пикселей у нашей ленты.
#define LED_COUNT 96
 
// Указываем, к какому порту подключен вход ленты DIN.
#define LED_PIN 5
#define BUTTON_PIN 4
#define LED_ON_PIN 10

#define A0_PURPURE 0
#define A1_GREEN 1
#define A2_YELLOW 2
#define A3_ORANGE 3

#define MODE_WHITE 0
#define MODE_RGB 1
#define MODE_HSV 2
#define MODE_RAINBOW 3
#define MODE_RAINBOW2 4
#define MODE_RANDOM 5

#define MODES_COUNT 5

#define POT_COUNT 4
#define HISTORY_SIZE 25

#include <FastLED.h>
CRGB strip[LED_COUNT];
int offset = 0;

int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

int p[POT_COUNT];
//int p0 = 0; //Purpure
//int p1 = 0; //Green
//int p2 = 0; //Yellow
//int p3 = 0; //Orange
int history[POT_COUNT][HISTORY_SIZE];

boolean needReset = false;

int currentMode = 0;
int modes[] = {MODE_WHITE, MODE_RAINBOW, MODE_RANDOM};

unsigned long rainbowChangeTime = 0;
int rainbow2Offeset = 0;

void setup()
{ 
  FastLED.addLeds<WS2812B, LED_PIN, RGB>(strip, LED_COUNT);
  pinMode(BUTTON_PIN, INPUT);   
  pinMode(LED_ON_PIN, OUTPUT);
  Serial.begin(9600);
  digitalWrite(LED_ON_PIN, HIGH);
}
 
void loop()
{
  readPotentiometers();
  handleClick();
  handleReset();
  if(currentMode == MODE_WHITE){
    modeWhite();
  } else if(currentMode == MODE_RAINBOW){
    modeRainbow();
  } else if(currentMode == MODE_RANDOM){
    modeRandomLed();
  } else if(currentMode == MODE_RGB){
    modeRGB();
  } else if(currentMode == MODE_HSV){
    modeHSV();
  } else if(currentMode == MODE_RAINBOW2){
    modeRainbow2();
  }
}

void handleClick(){
  int buttonState = digitalRead(BUTTON_PIN);
  if(lastButtonState != buttonState){
    lastDebounceTime = millis();
  }

  if(lastDebounceTime > 0 && (millis() - lastDebounceTime) >= debounceDelay){
    lastDebounceTime = 0;
    if(buttonState == LOW){
      lastButtonState = LOW;
      changeState();
    } else{
      lastButtonState = HIGH;
    }
  }
}

void changeState(){
  int nextMode = currentMode + 1;
  if(nextMode >= MODES_COUNT){
    nextMode = 0;
  }
  currentMode = nextMode;
  needReset = true;
}

void handleReset(){
  if(needReset){
    for (int i = 0; i < LED_COUNT; i++){
      strip[i] = CRGB::Black;
    }
    FastLED.show();
    needReset = false;
  }
}

void modeWhite(){
  for (int i = 0; i < LED_COUNT; i++){
      strip[i] = CRGB::White;
      strip[i].fadeLightBy(p[0]);
  }
  FastLED.show();
}

void modeRandomLed(){
  int i = random(0, LED_COUNT - 1);
  int hue = random(0, 255);
  strip[i].setHSV(hue, 255, 200);
  strip[i].fadeLightBy(p[0]);
    FastLED.show();
}

void modeRainbow(){
  for (int i = 0; i < LED_COUNT; i++){
      int hue = i + offset;
      strip[i].setHSV(hue, 255, 200);
      strip[i].fadeLightBy(p[0]);
  }

  offset++;
  if(offset == 10000){
    offset = 0;
  }

  FastLED.show();
}

void modeRainbow2(){
  unsigned long t = millis();
  int delay = p[1] * 10;
  if((t - rainbowChangeTime > delay)){
    for (int i = 0; i < LED_COUNT; i++){
      strip[i].setHSV(rainbow2Offeset, 255, 255);
      strip[i].fadeLightBy(p[0]);
    }
    rainbow2Offeset++;
    if(rainbow2Offeset == 10000){
      rainbow2Offeset = 0;
    }
    rainbowChangeTime = t;
    FastLED.show();
  }
}

void modeRGB(){
   for (int i = 0; i < LED_COUNT; i++){
      int hue = i + offset;
      strip[i]= CRGB(p[2], p[1], p[3]);
      strip[i].fadeLightBy(p[0]);
  }
  FastLED.show();
}

void modeHSV(){
   for (int i = 0; i < LED_COUNT; i++){
      int hue = i + offset;
      strip[i].setHSV(p[1], p[2], p[3]);
  }
  FastLED.show();
}

void readPotentiometers(){
    readPInput(A0_PURPURE);
    readPInput(A1_GREEN);
    readPInput(A2_YELLOW);
    readPInput(A3_ORANGE);
}

void readPInput(int pos){
  int input = analogRead(pos);
    p[pos] = stackArray(history[pos], input / 4);
}

int stackArray(int arr[], int value){
  int sum = 0;
  for(int i =0; i < HISTORY_SIZE; i++){
    if(i + 1 < HISTORY_SIZE){
      arr[i] = arr[i+1];
      sum += arr[i];
    } else {
      arr[i] = value;
      sum += value;
    }
  }
  sum = sum / HISTORY_SIZE;
  return sum;
}

