#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo valvula;

const int potPin = A0;
const int motorPin = 9;
const int servoPin = 10;

const int led10 = 2;
const int led30 = 3;
const int led50 = 4;
const int led70 = 5;
const int led90 = 6;

int nivel = 0;
bool bombaLigada = false;

void setup() {
  lcd.init();
  lcd.backlight();

  valvula.attach(servoPin);

  pinMode(motorPin, OUTPUT);

  pinMode(led10, OUTPUT);
  pinMode(led30, OUTPUT);
  pinMode(led50, OUTPUT);
  pinMode(led70, OUTPUT);
  pinMode(led90, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int leitura = analogRead(potPin);
  nivel = map(leitura, 0, 1023, 0, 100);

  if (nivel <= 10) {
    bombaLigada = true;
  }
  if (nivel >= 90) {
    bombaLigada = false;
  }

  if (bombaLigada) {
    digitalWrite(motorPin, HIGH);
    valvula.write(90);
  } else {
    digitalWrite(motorPin, LOW);
    valvula.write(0);
  }

  digitalWrite(led10, nivel >= 10);
  digitalWrite(led30, nivel >= 30);
  digitalWrite(led50, nivel >= 50);
  digitalWrite(led70, nivel >= 70);
  digitalWrite(led90, nivel >= 90);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nivel: ");
  lcd.print(nivel);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Bomba: ");
  if (bombaLigada) {
    lcd.print("Ligado");
  } else {
    lcd.print("Desligado");
  }

  Serial.print("Nivel: ");
  Serial.print(nivel);
  Serial.print("% | Bomba: ");

  if (bombaLigada) {
    Serial.println("Ligado");
  } else {
    Serial.println("Desligado");
  }

  delay(500);
}