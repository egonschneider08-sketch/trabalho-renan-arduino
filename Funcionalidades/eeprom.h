#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>


int tela = 0;
unsigned long tempoTroca = 0;


// LCD I2C (endereço comum 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo valvula;


// Pinos das chaves
const int nivel10 = 2;
const int nivel30 = 3;
const int nivel50 = 4;
const int nivel70 = 5;
const int nivel90 = 6;


// LEDs
const int led10 = 7;
const int led30 = 8;
const int led50 = 9;
const int led70 = 10;
const int led90 = 11;


// Atuadores
const int bomba = 12;
const int servoPin = 13;


// Variáveis
unsigned long nivelAtual = 0;
bool bombaLigada = false;


unsigned long nivelAnterior = 0;
unsigned long consumo = 0;


void setup() {
  // EEPROM
  EEPROM.get(0, consumo);
  EEPROM.get(10, nivelAtual);
  EEPROM.get(20, bombaLigada);
 
  // Entradas
  pinMode(nivel10, INPUT);
  pinMode(nivel30, INPUT);
  pinMode(nivel50, INPUT);
  pinMode(nivel70, INPUT);
  pinMode(nivel90, INPUT);


  // Saídas
  pinMode(led10, OUTPUT);
  pinMode(led30, OUTPUT);
  pinMode(led50, OUTPUT);
  pinMode(led70, OUTPUT);
  pinMode(led90, OUTPUT);
  pinMode(bomba, OUTPUT);


  // Servo
  valvula.attach(servoPin);


  // LCD
  lcd.init();
  lcd.backlight();


  // Inicial
  digitalWrite(bomba, LOW);
  valvula.write(0); // válvula fechada
 
  nivelAnterior = nivelAtual;
 
  Serial.begin(9600);
}


void loop() {
  bool n10 = digitalRead(nivel10);
  bool n30 = digitalRead(nivel30);
  bool n50 = digitalRead(nivel50);
  bool n70 = digitalRead(nivel70);
  bool n90 = digitalRead(nivel90);
  // Leitura dos níveis
  if (n90 && n70 && n50 && n30 && n10) {
    nivelAtual = 90;
  }
  else if (n70 && n50 && n30 && n10 && !n90) {
    nivelAtual = 70;
  }
  else if (n50 && n30 && n10 && !n70 && !n90) {
    nivelAtual = 50;
  }
  else if (n30 && n10 && !n50 && !n70 && !n90) {
    nivelAtual = 30;
  }
  else if (n10 && !n30 && !n50 && !n70 && !n90) {
    nivelAtual = 10;
  }
  else if (!n10 && !n30 && !n50 && !n70 && !n90) {
    nivelAtual = 0;
  }
  else{
    nivelAtual = 999;
  }


  // ===== CONSUMO =====
  if (nivelAtual <= 100 && nivelAtual > nivelAnterior)
  {    
    int diferenca = nivelAtual - nivelAnterior;
    consumo += (diferenca * 100);
  }


  // LEDs
  digitalWrite(led10, nivelAtual >= 10);
  digitalWrite(led30, nivelAtual >= 30);
  digitalWrite(led50, nivelAtual >= 50);
  digitalWrite(led70, nivelAtual >= 70);
  digitalWrite(led90, nivelAtual >= 90);


  // Controle da bomba e válvula
  if (nivelAtual <= 10) {
    bombaLigada = true;
  }
 
  if (nivelAtual > 100) {
    bombaLigada = false;
  }


  if (nivelAtual >= 90) {
    bombaLigada = false;
  }


  // Aplicar estado
  if (bombaLigada) {
    digitalWrite(bomba, HIGH);
    valvula.write(90); // válvula aberta
  } else {
    digitalWrite(bomba, LOW);
    valvula.write(0); // válvula fechada
  }


  // LCD
  if (millis() - tempoTroca > 2000) { // troca a cada 2s
    tela++;
    if (tela > 1) tela = 0;
    tempoTroca = millis();
  }


  // ===== LCD SEM PISCAR =====


  if (nivelAtual > 100) {
    lcd.setCursor(0, 0);
    lcd.print("ERRO!!         ");


    lcd.setCursor(0, 1);
    lcd.print("Bomba: Desligada");
  }
  else if (tela == 0) {
    // Tela 1: Nivel + Bomba
    lcd.setCursor(0, 0);
    lcd.print("Nivel: ");
    lcd.print(nivelAtual);
    lcd.print("%   "); // limpa sobra


    lcd.setCursor(0, 1);
    lcd.print("Bomba: ");
    if (bombaLigada) {
      lcd.print("Ligada   ");
    } else {
      lcd.print("Desligada");
    }


  } else {
    // Tela 2: Consumo
    lcd.setCursor(0, 0);
    lcd.print("Consumo:      ");


    lcd.setCursor(0, 1);
    lcd.print(consumo);
    lcd.print(" L                 ");
  }


  Serial.print("Nivel: ");
  Serial.print(nivelAtual);
  Serial.print("% | Bomba: ");


  if (bombaLigada) {
    Serial.print("Ligada");
  } else {
    Serial.print("Desligada");
  }


  // ===== CONSUMO NO SERIAL =====
  Serial.print(" | Consumo Total: ");
  Serial.print(consumo);
  Serial.println(" L");


  // Atualiza nível anterior
  nivelAnterior = nivelAtual;
 
  if (nivelAtual <= 100) {
  EEPROM.put(0, consumo);
  EEPROM.put(10, nivelAtual);
  EEPROM.put(20, bombaLigada);
  }


  delay(500);
}
