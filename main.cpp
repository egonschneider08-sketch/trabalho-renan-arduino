#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Configuração do LCD e Servo
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo valvula;

// Definição dos Pinos
const int potPin = A0;
const int motorPin = 9;
const int servoPin = 10;

// LEDs
const int led10 = 2;
const int led30 = 3;
const int led50 = 4;
const int led70 = 5;
const int led90 = 6;

int nivel = 0;
bool bombaLigada = false;

void setup() {
    // Inicialização do LCD
    lcd.init();
    lcd.backlight();

    // Configuração do Servo e Motor
    valvula.attach(servoPin);
    pinMode(motorPin, OUTPUT);

    // Configuração dos LEDs
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

    // Lógica de Controle (Histerese)
    if (nivel <= 10) {
        bombaLigada = true;
    } else if (nivel >= 90) {
        bombaLigada = false;
    }

    // Atuações (Motor e Servo)
    if (bombaLigada) {
        digitalWrite(motorPin, HIGH);
        valvula.write(90);
    } else {
        digitalWrite(motorPin, LOW);
        valvula.write(0);
    }

    // Controle dos LEDs de nível
    digitalWrite(led10, nivel >= 10);
    digitalWrite(led30, nivel >= 30);
    digitalWrite(led50, nivel >= 50);
    digitalWrite(led70, nivel >= 70);
    digitalWrite(led90, nivel >= 90);

    // Atualização do LCD
    lcd.setCursor(0, 0);
    lcd.print("Nivel: ");
    lcd.print(nivel);
    lcd.print("%   "); // Espaços extras evitam resquícios de números anteriores

    lcd.setCursor(0, 1);
    lcd.print("Bomba: ");
    lcd.print(bombaLigada ? "Ligado   " : "Desligado");

    // Monitor Serial
    Serial.print("Nivel: ");
    Serial.print(nivel);
    Serial.print("% | Bomba: ");
    Serial.println(bombaLigada ? "Ligado" : "Desligado");

    delay(500);
}