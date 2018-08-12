int ch1, ch3, ch2, pwm1, dig1, dig2, dig3, dig4, pwm2, fadeAmount, auton;
int brightness1 = 255;
int brightness2 = 255;
int val = 0;
int flag;


void setup() {
  pwm1 = 5;
  pwm2 = 6;
  dig1 = 7;
  dig2 = 8;
  dig3 = 9;                    // directional control of motor B
  dig4 = 10;
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(4, INPUT);

  pinMode(pwm1, OUTPUT);
  pinMode(dig1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(dig4, OUTPUT);
  auton = 0;
  val = 0;
}

void loop() {
  ch1 = pulseIn(2, HIGH, 25000);
  delay(10);
  ch3 = pulseIn(4  , HIGH, 25000);
  delay(10);
   Serial.print(ch1);
   Serial.print(",");
  Serial.println(ch3);
  
  
  if (ch3 > 1400)                                  //left
  {
      analogWrite(pwm1, 255);
    digitalWrite(dig1, HIGH);
      digitalWrite(dig2, LOW);
    analogWrite(pwm2, 255);
     digitalWrite(dig3, HIGH);
      digitalWrite(dig4, LOW);
    
    flag = 1;
  }
  if (ch3 < 1300) {                               //right
    analogWrite(pwm1, 255);
    digitalWrite(dig1, LOW);
      digitalWrite(dig2, HIGH);
    analogWrite(pwm2, 255);
     digitalWrite(dig3, LOW);
      digitalWrite(dig4, HIGH);
    
    flag = 1;
  }
  else {
    flag = 0;
  }
  if ((ch3 >= 1200) && (ch3 <= 1400 )) {           //Stop
    analogWrite(pwm1, 255);
    digitalWrite(dig1, HIGH);
     digitalWrite(dig2, HIGH);
    analogWrite(pwm2, 255);
     digitalWrite(dig3, HIGH);
      digitalWrite(dig4, HIGH);
   
    
  }

  if (ch1 < 1300 && flag == 0) {                            //back
    analogWrite(pwm1, 255);
    digitalWrite(dig1, LOW);
      digitalWrite(dig2, HIGH);
    analogWrite(pwm2, 255);
     digitalWrite(dig3, HIGH);
      digitalWrite(dig4, LOW);
  
  }
  if (ch1 > 1800 && flag == 0) {                   //forward
    analogWrite(pwm1, 255);
    digitalWrite(dig1, HIGH);
    digitalWrite(dig2, LOW);
    analogWrite(pwm2,  255);
    digitalWrite(dig3, LOW);
    digitalWrite(dig4, HIGH);
    
  }



}
