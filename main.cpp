#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo valvula;

// Pinos
const int potPin = A0;
const int motorPin = 9;
const int servoPin = 10;

// LEDs
const int led10 = 2;
const int led30 = 3;
const int led50 = 4;
const int led70 = 5;
const int led90 = 6;

// Variáveis
int nivel = 0;
int nivelAnterior = 0;
float consumo = 0;

bool bombaLigada = false;

// Controle de troca de tela
unsigned long tempoAnterior = 0;
int tela = 0;

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

  // Consumo (quando nível sobe)
  if (nivel > nivelAnterior) {
    consumo += (nivel - nivelAnterior) * 100;
  }
  nivelAnterior = nivel;

  // Controle bomba
  if (nivel <= 10) bombaLigada = true;
  if (nivel >= 90) bombaLigada = false;

  // Atuações
  digitalWrite(motorPin, bombaLigada);
  valvula.write(bombaLigada ? 90 : 0);

  // LEDs
  digitalWrite(led10, nivel >= 10);
  digitalWrite(led30, nivel >= 30);
  digitalWrite(led50, nivel >= 50);
  digitalWrite(led70, nivel >= 70);
  digitalWrite(led90, nivel >= 90);

  // Alterna tela a cada 2 segundos
  if (millis() - tempoAnterior > 2000) {
    tela++;
    if (tela > 1) tela = 0;
    lcd.clear();
    tempoAnterior = millis();
  }

  // Tela 1 → nível + bomba
  if (tela == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Nivel: ");
    lcd.print(nivel);
    lcd.print("%   "); // Espaços extras ajudam a limpar caracteres antigos sem precisar usar lcd.clear() toda hora

    lcd.setCursor(0, 1);
    lcd.print("Bomba: ");
    lcd.print(bombaLigada ? "Ligado   " : "Desligado");
  }

  // Tela 2 → consumo
  if (tela == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Consumo total:  ");

    lcd.setCursor(0, 1);
    lcd.print((int)consumo);
    lcd.print(" Litros       ");
  }

  // Serial (mostra tudo sempre)
  Serial.print("Nivel: ");
  Serial.print(nivel);
  Serial.print("% | Consumo: ");
  Serial.print(consumo);
  Serial.print(" L | Bomba: ");
  Serial.println(bombaLigada ? "Ligado" : "Desligado");

  delay(300);
}