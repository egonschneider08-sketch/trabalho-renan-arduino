#include <Arduino.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>

// Protótipos de funções (Boa prática no PlatformIO/C++)
// Nota: Para este código simples, o Arduino.h cuida disso, 
// mas é bom se acostumar a declarar funções se criar novas.

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
unsigned long consumo = 0;
unsigned long consumoSalvo = 0;
bool bombaLigada = false;

// Controle de tempo
unsigned long tempoAnterior = 0;
unsigned long tempoEEPROM = 0;
int tela = 0;

// Endereço EEPROM
const int enderecoEEPROM = 0;

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

  // Lê valor salvo
  EEPROM.get(enderecoEEPROM, consumo);

  // Proteção (caso memória esteja "lixo")
  if (consumo > 1000000000) {
    consumo = 0;
  }
}

void loop() {
  int leitura = analogRead(potPin);
  nivel = map(leitura, 0, 1023, 0, 100);

  // Cálculo de consumo simplificado
  if (nivel > nivelAnterior) {
    consumo += (nivel - nivelAnterior) * 100;
  }
  nivelAnterior = nivel;

  // Controle bomba
  if (nivel <= 10) bombaLigada = true;
  if (nivel >= 90) bombaLigada = false;

  digitalWrite(motorPin, bombaLigada);
  valvula.write(bombaLigada ? 90 : 0);

  // LEDs de indicação
  digitalWrite(led10, nivel >= 10);
  digitalWrite(led30, nivel >= 30);
  digitalWrite(led50, nivel >= 50);
  digitalWrite(led70, nivel >= 70);
  digitalWrite(led90, nivel >= 90);

  // 🔄 Alternância de telas do LCD (2 segundos)
  if (millis() - tempoAnterior > 2000) {
    tela++;
    if (tela > 1) tela = 0;
    lcd.clear();
    tempoAnterior = millis();
  }

  if (tela == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Nivel: ");
    lcd.print(nivel);
    lcd.print("%");
    lcd.setCursor(0, 1);
    lcd.print("Bomba: ");
    lcd.print(bombaLigada ? "Ligado" : "Desligado");
  } else if (tela == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Consumo total:");
    lcd.setCursor(0, 1);
    lcd.print(consumo);
    lcd.print(" L");
  }

  // Salva na EEPROM a cada 5 segundos (se houver alteração)
  if (millis() - tempoEEPROM > 5000) {
    if (consumo != consumoSalvo) {
      EEPROM.put(enderecoEEPROM, consumo);
      consumoSalvo = consumo;
    }
    tempoEEPROM = millis();
  }

  // Monitor Serial
  Serial.print("Nivel: ");
  Serial.print(nivel);
  Serial.print("% | Consumo: ");
  Serial.print(consumo);
  Serial.print(" L | Bomba: ");
  Serial.println(bombaLigada ? "Ligado" : "Desligado");

  delay(300);
}