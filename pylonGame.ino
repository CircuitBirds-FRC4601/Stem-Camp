#include <Time.h>
#include <TimeLib.h>

#include "Adafruit_NeoPixel.h"




#define PIN            6
#define NUMPIXELS      16
#define PULL_PY 1           //these numbers are the ordering for the Neopixel LEDs on each pylon in serial order along the DI/DO daisy chain. 
#define PUSH_PY 0
#define SHORT_PY 4
#define RECESSED 3 //might be 2
#define ROT 2
#define IRLEDb 40
#define IRLEDr 42
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(9, 6, NEO_GRB + NEO_KHZ800);

byte inc;
// VARIABLES DEFINED!
// ouputPeriod sets the number of loops through before outputing state via serial
//            (kept track of with timesThrough
// scoreR and scoreB are the running red and blue score
// ownersPins for each numbered pin, tells who owns it. 0= no one owns, >0 Blue, <0 Red
// analogPin is the A# pin for the IR sensor's value
// bump_owner_score = what to add to the team with the most pylons currently owned at the end.
// duration_of_game = how many seconds total per game
int num_pylons, outputPeriod = 200, scoreB, scoreR, ownersPins[8], valueofPins[8], timesThrough, outi, scoreRlast, scoreBlast, bump_owner_score, pylons_owned;
int timedifference, sec_old = 0, seconds, output, startTime_seconds, lightPin = 11, analogPin = 5, output2, seconds2, temp_time, total_elapsed_time, initialization_full_time;
int duration_of_game = 121, flag = 0;
float val = 0.0;

void setup() {                                        // variable initialization
  num_pylons = 5;                                     // number of indepedent pylons
  scoreR = 0;
  scoreB = 0;
  scoreRlast = scoreR;
  scoreBlast = scoreB;
  timesThrough = 0;                                   // keeps track of times through the loop
  // temporary line for testing
  digitalWrite(2, LOW);
  for (int i = 1; i < 8; i++) {
    ownersPins[i] = 0;                                // 0= no one owns, >0 Blue, <0 Red
    valueofPins[i] = 0;
  }
  valueofPins[1] = 0;                                 // here is the value map for the pins...see below for the relation to idexing and the pin values.
  valueofPins[2] = 1;
  valueofPins[3] = 4;
  valueofPins[4] = 2;
  valueofPins[5] = 3;
  bump_owner_score = 2;                               // bump up score of the team with most ownership at end by this amount
  pixels.begin();                                     // This initializes the NeoPixel library.
  for (int i = 0; i <= 9; i++) {
    pixels.setPixelColor(i, 255, 0, 0);
    pixels.show();
  }
  delay(300);
  for (int i = 0; i <= 9; i++) {
    pixels.setPixelColor(i, 0, 255, 0);
    pixels.show();
  }
  delay(300);
  for (int i = 0; i <= 9; i++) {
    pixels.setPixelColor(i, 0, 0, 255);
    pixels.show();
  }

  delay(300);
  for (int i = 0; i < 10; i++) {
    pixels.setPixelColor(i, 0, 200, 0);
    pixels.show();
    delay(10);
  }

  pinMode(2, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(51, OUTPUT);
  pinMode(53, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);
  pinMode(30, INPUT_PULLUP);
  pinMode(31, INPUT_PULLUP);
  pinMode(24, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP);
  pinMode(IRLEDb, OUTPUT);
  pinMode(IRLEDr, OUTPUT);
  Serial.begin(9600);
  time_t startTime = now();
  startTime_seconds = second(startTime);
  temp_time = startTime_seconds;
  initialization_full_time = minute(startTime) * 60 + temp_time;
  digitalWrite(13, HIGH);
  boolean waiting = true;
  digitalWrite(IRLEDb, HIGH);
  digitalWrite(IRLEDr, HIGH);
  digitalWrite(2, LOW);                              // our signal ground return
  while (waiting)                                    // for generating the start sequence from the main computer. Not in use now.
  {
    //if (Serial.available()) {

    if (digitalRead(53) == LOW) {                 //sending this line low starts the run sequence.
      waiting = false;
      digitalWrite(IRLEDb, LOW);
      digitalWrite(IRLEDr, LOW);
    }
    else {
      pixels.setPixelColor(6, pixels.Color(255, 0, 0));
      pixels.setPixelColor(7, pixels.Color(255, 0, 0));
      pixels.setPixelColor(8, pixels.Color(255, 0, 0));
      pixels.setPixelColor(5, pixels.Color(255, 0, 0));
      pixels.show();
      //      Serial.write(Serial.read());
    }

    //   }
  }
  pixels.setPixelColor(5, pixels.Color(255, 0, 255));
  pixels.setPixelColor(6, pixels.Color(255, 0, 255));
  pixels.setPixelColor(7, pixels.Color(255, 0, 255));
  pixels.setPixelColor(8, pixels.Color(255, 0, 255));
  pixels.show();
  delay(600);
  pixels.setPixelColor(5, pixels.Color(0, 0, 0));
  pixels.setPixelColor(6, pixels.Color(0, 0, 0));
  pixels.setPixelColor(7, pixels.Color(0, 0, 0));
  pixels.setPixelColor(8, pixels.Color(0, 0, 0));
  pixels.show();
  delay(600);
  pixels.setPixelColor(5, pixels.Color(255, 0, 255));
  pixels.setPixelColor(6, pixels.Color(255, 0, 255));
  pixels.setPixelColor(7, pixels.Color(255, 0, 255));
  pixels.setPixelColor(8, pixels.Color(255, 0, 255));
  pixels.show();
}
// The main game section
//
//
//  once every "outperiod" times through the loop polling the pylons the code outputs the
//     score into the serial line.
//
//   here's the hardware mapping:
//
// index            RED   BLUE    value of pin
// 2      pullpy     3        4          1
// 1      pushpy     10      22          0
// 5      shortpy    30      31          3
// 4      recessed   24      25          2
// 3      rotation   26      28          4
// light        A5       (but you light it up with line 51)
//
// indicator light for light pylon
//
void loop() {
  // Update the Pylon Colors to show current ownership of pylon
  for (int i = 1; i <= num_pylons; i++) { // ownersPins[i]= 0 no one owns, >0 Blue, <0 Red
    outi = i - 1;              // have to map the ownersPins order with the pylon neopixel address
    if (ownersPins[i] < 0) {
      pixels.setPixelColor(outi, pixels.Color(255, 0, 0));
      pixels.show();
    }
    if (ownersPins[i] > 0) {
      pixels.setPixelColor(outi, pixels.Color(0, 0, 255));
      pixels.show();
    }
    if (ownersPins[i] == 0) {
      pixels.setPixelColor(outi, pixels.Color(0, 255, 0));
      pixels.show();
    }
  }
  // Output Score, episodically, only when the score actually changes (so the python doesn't choke on the buffer fillings!)
  // TIME section...to send updates to the score board each second we need to keep track of time
  time_t t = now();
  seconds = second(t);
  output = seconds - sec_old;
  if (output == -59) {                          //correct for the minute boundary
    output = 1;
  }
  if (output == 1) {
    //if((scoreR!=scoreRlast)||(scoreB!=scoreBlast)){
    Serial.print(scoreB);
    Serial.print(' ');
    Serial.println(scoreR);
    //    }
    scoreRlast = scoreR;
    scoreBlast = scoreB;
    timesThrough = 0;
    output = 0;
  }
  timesThrough++;
  sec_old = seconds;
  // pylon polling sections
  if (digitalRead(3) == HIGH)   //PULLPY pair
  {
    if (ownersPins[2] >= 0) { // only increment score if the ownership changes.
      ownersPins[2] = -1;
      scoreR = scoreR + valueofPins[2];
    }
  }
  if (digitalRead(4) == HIGH)
  {
    if (ownersPins[2] <= 0) {
      ownersPins[2] = +1;
      scoreB = scoreB + valueofPins[2];
    }
  }
  if (digitalRead(10) == LOW)  //PUSHPY pair
  {
    if (ownersPins[1] >= 0) {  // only increment score if the ownership changes.
      ownersPins[1] = -1;
      scoreR = scoreR + valueofPins[1];
      delay(200);              // to avoid "crashes" when both sides hit at same time
    }
  }
  if (digitalRead(22) == LOW)
  {
    if (ownersPins[1] <= 0) {
      ownersPins[1] = +1;
      scoreB = scoreB + valueofPins[1];
      delay(200);              // to avoid "crashes" when both sides hit at same time
    }
  }
  //Serial.println(analogRead(A0));
  //delay(10);
  if (digitalRead(30) == LOW) // SHORTPY
  {
    if (ownersPins[5] >= 0) { // only increment score if the ownership changes.
      ownersPins[5] = -1;
      scoreR = scoreR + valueofPins[5];
    }
  }
  if (digitalRead(31) == LOW)
  {
    if (ownersPins[5] <= 0) {
      ownersPins[5] = +1;
      scoreB = scoreB + valueofPins[5];
    }
  }
  if (digitalRead(24) == LOW) // RECESSED
  {
    if (ownersPins[4] >= 0) { // only increment score if the ownership changes.
      ownersPins[4] = -1;
      scoreR = scoreR + valueofPins[4];
    }
  }
  if (digitalRead(25) == LOW)
  {
    if (ownersPins[4] <= 0) {
      ownersPins[4] = +1;
      scoreB = scoreB + valueofPins[4];
    }
  }
  if (digitalRead(26) == LOW) // ROTATION
  {
    if (ownersPins[3] >= 0) { // only increment score if the ownership changes.
      ownersPins[3] = -1;
      scoreR = scoreR + valueofPins[3];
    }
  }
  if (digitalRead(28) == LOW)
  {
    if (ownersPins[3] <= 0) {
      ownersPins[3] = +1;
      scoreB = scoreB + valueofPins[3];
    }
  }
  // The light pylon, not quite working correctly yet! Its a common pylon, they have to drive under it when its light matches them!
  digitalWrite(51, HIGH);              // turnon the IR LED
  val = analogRead(analogPin);         // read the input pin
  //     Serial.println(val);               // debug value
  //digitalWrite(11,HIGH);             // color LED indicator light RED  lines for testing....
  //digitalWrite(12,HIGH);             // color LED : BLUE
  //digitalWrite(13,HIGH);             // color LED :  GREEN
  time_t t2 = now();
  seconds2 = second(t2);
  output2 = seconds2 - temp_time;
  if (output2 < 0) {
    output2 = output2 + 60;
  }
  if (output2 > 20) {
    output2 = 0;
    temp_time = seconds2; // then toggle the output lines!
    digitalWrite(lightPin, LOW);
    if (lightPin == 11) {
      lightPin = 12;
    }
    else if (lightPin == 12) {
      lightPin = 11;
    }
  }
  digitalWrite(lightPin, HIGH);
  // section to test for the end of the game!
  total_elapsed_time = minute(t2) * 60 + second(t2) - initialization_full_time;
  if ((total_elapsed_time == duration_of_game) && (flag == 0)) {            //check time to stop game
    for (int i = 1; i <= num_pylons; i++) {     // check who owned the most pylons
      pylons_owned = pylons_owned + ownersPins[i];
    }
    if (pylons_owned < 0) {                      // add "bump_owner_score" to score of team owning most pylons
      scoreR = scoreR + bump_owner_score;
    }
    if (pylons_owned > 0) {
      scoreB = scoreB + bump_owner_score;
    }
    flag = 1;
  }
  if (total_elapsed_time > duration_of_game) {
    scoreR = scoreRlast;
    scoreB = scoreBlast;
    if (scoreR > scoreB) {                      // put the minarets color to match the winning team's color!
      pixels.setPixelColor(6, pixels.Color(255, 0, 0));
      pixels.setPixelColor(7, pixels.Color(255, 0, 0));
      pixels.setPixelColor(8, pixels.Color(255, 0, 0));
      pixels.setPixelColor(5, pixels.Color(255, 0, 0));
      pixels.show();
    }
    if (scoreB > scoreR) {
      pixels.setPixelColor(6, pixels.Color(0, 0, 255));
      pixels.setPixelColor(7, pixels.Color(0, 0, 255));
      pixels.setPixelColor(8, pixels.Color(0, 0, 255));
      pixels.setPixelColor(5, pixels.Color(0, 0, 255));
      pixels.show();
    }
    if (scoreR == scoreB) {                      // no winnner, just make the minarets green!
      pixels.setPixelColor(6, pixels.Color(0, 255, 0));
      pixels.setPixelColor(7, pixels.Color(0, 255, 0));
      pixels.setPixelColor(8, pixels.Color(0, 255, 0));
      pixels.setPixelColor(5, pixels.Color(0, 255, 0));
      pixels.show();
    }
  }

  /*if (analogRead(A1) > 500)q
    {
    Serial.write((int)'B');
    Serial.write(12);
    Serial.println();
    }
    if (analogRead(A0) > 500)
    {
    Serial.write((int)'B');
    Serial.write(12);
    Serial.println();
    }

    if (Serial.available()) {
    inc = Serial.read();
    switch (inc) {
      case '3':
        digitalWrite(50, HIGH);
        digitalWrite(52, LOW);
        digitalWrite(51, HIGH);
        digitalWrite(53, LOW);
        Serial.println('3');
        break;
      case '4':
        digitalWrite(50, LOW);
        digitalWrite(52, HIGH);
        digitalWrite(53, HIGH);
        digitalWrite(51, LOW);
        Serial.println('4');
        break;
      case '5':
        digitalWrite(51, LOW);
        digitalWrite(53, LOW);
        digitalWrite(52, LOW);
        digitalWrite(54, LOW);
        Serial.println('5');
        break;
      case 80:
        pixels.setPixelColor(6, pixels.Color(0, 0, 255));
        pixels.setPixelColor(7, pixels.Color(0, 0, 255));
        pixels.setPixelColor(8, pixels.Color(0, 0, 255));
        pixels.setPixelColor(5, pixels.Color(0, 0, 255));
        pixels.show();
        break;
      case 90:
        pixels.setPixelColor(6, pixels.Color(255, 0, 0));
        pixels.setPixelColor(7, pixels.Color(255, 0, 0));
        pixels.setPixelColor(8, pixels.Color(255, 0, 0));
        pixels.setPixelColor(5, pixels.Color(255, 0, 0));
        break;
      default:
        break;
    }*/
  delay(1);
}
