/**/#include "ada88.h"void setup() {  pinMode(0,OUTPUT);  pinMode(1,OUTPUT);  pinMode(2,OUTPUT);  ada88_init();  ada88_write(19);}void loop() {  int brightness;  digitalWrite(0,LOW);  digitalWrite(1,LOW);  digitalWrite(2,LOW);  delay(2);  brightness= analogRead(0);  ada88_writeNumber(10-brightness/100);  digitalWrite(0,HIGH);  digitalWrite(1,HIGH);  digitalWrite(2,HIGH);    delay(100);}