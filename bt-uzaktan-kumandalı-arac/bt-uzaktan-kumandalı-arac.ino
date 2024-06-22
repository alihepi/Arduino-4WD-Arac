#include <HCSR04.h>

// Hız Tanımlamaları
int MAX_MOTOR_POWER = 255;
int SLOW_MOTOR_POWER = 50;
int BACK_MOTOR_POWER = 100;

int L_MOTOR_POWER = 180;
int M_MOTOR_POWER = 50;

int MIN_MOTOR_POWER = 10;


// Motor pinler
const byte MotorENA = 10;
const byte leftMotorForwardPin = 2;
const byte leftMotorBackwardPin = 3;

const byte MotorENB = 11;
const byte rightMotorForwardPin = 5;
const byte rightMotorBackwardPin = 4;


// Bluetooth değerleri için değişken
char btValue_StSp = '0';

char btValue_UD = '0';
char btValue_RL = '0';

char btValue_Lgt = '0';


//FAR PIN
const byte FAR_PIN = 12;


// HC-SR04 sensör pinleri
const byte trigPin2 = 8;
const byte echoPin2 = 9;

//Sensor Tanımlama
UltraSonicDistanceSensor sensor2(trigPin2, echoPin2);

void setup() {
  Serial.begin(9600);

  // Değer göndereceğimiz pinleri çıkış pinleri yapıyoruz
  pinMode(MotorENA, OUTPUT);
  pinMode(leftMotorForwardPin, OUTPUT);
  pinMode(leftMotorBackwardPin, OUTPUT);

  pinMode(MotorENB, OUTPUT);
  pinMode(rightMotorForwardPin, OUTPUT);
  pinMode(rightMotorBackwardPin, OUTPUT);

  pinMode(FAR_PIN, OUTPUT);
  opCl();
}

void loop() {

  float distanceSensorOrta = sensor2.measureDistanceCm();

  // HC05 Bluetooth Modülü ile seri haberleşme
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    Serial.println(receivedChar);

    if (receivedChar == '1') {  //START
      btValue_StSp = '1';
    } else if (receivedChar == '0') {  //STOP
      btValue_StSp = '0';
    }
    //ILERI
    if (receivedChar == 'W') {
      btValue_UD = 'W';
    } else if (receivedChar == 'Q') {
      btValue_UD = '4';
    }
    //GERI
    if (receivedChar == 'S') {
      btValue_UD = 'S';
    } else if (receivedChar == 'X') {
      btValue_UD = '3';
    }
    //SAĞ
    if (receivedChar == 'O') {
      btValue_RL = 'O';
    } else if (receivedChar == 'P') {
      btValue_RL = '0';
    }
    //SOL
    if (receivedChar == 'K') {
      btValue_RL = 'K';
    } else if (receivedChar == 'L') {
      btValue_RL = '0';
    }
    //Far
    if (receivedChar == 'T') {  // FAR AÇ
      btValue_Lgt = 'T';
    } else if (receivedChar == 'C') {  // FAR KAPAT
      btValue_Lgt = 'C';
    }
  }

  Serial.print("BT SS: ");
  Serial.print(btValue_StSp);
  Serial.print(", BT SP: ");
  Serial.print(btValue_UD);
  Serial.print(", BT RL: ");
  Serial.print(btValue_RL);
  Serial.print(", BT FAR: ");
  Serial.print(btValue_Lgt);
  Serial.print(", SD: ");
  Serial.println(distanceSensorOrta);


  if (btValue_StSp == '1') {
    selectorCtrl(distanceSensorOrta);

    if (btValue_UD == '0') {
      startMotors();
    } else if (btValue_UD == 'W') {
      maxSpeed();
      selectorCtrl(distanceSensorOrta);
    } else if (btValue_UD == '4') {
      slowSpeed();
      selectorCtrl(distanceSensorOrta);
    } else if (btValue_UD == 'S') {
      goBack();
      backSpeed();
      selectorCtrl(distanceSensorOrta);
    } else if (btValue_UD == '3') {
      startMotors();
      slowSpeed();
      selectorCtrl(distanceSensorOrta);
    } else {
      slowSpeed();
    }

    if (btValue_RL == 'O') {
      slowDown(MotorENB, MotorENA);
      selectorCtrl(distanceSensorOrta);
    } else if (btValue_RL == 'K') {
      slowDown(MotorENA, MotorENB);
      selectorCtrl(distanceSensorOrta);
    }

    if (btValue_Lgt == 'T') {
      digitalWrite(FAR_PIN, HIGH);
      selectorCtrl(distanceSensorOrta);
    } else {
      digitalWrite(FAR_PIN, LOW);
      selectorCtrl(distanceSensorOrta);
    }

  } else if (btValue_StSp == '0') {
    stopMotor();
    digitalWrite(FAR_PIN, LOW);
  }
}

// Motorların ileri gitmeleri için güç verme kodu
void startMotors() {
  digitalWrite(MotorENA, SLOW_MOTOR_POWER);
  digitalWrite(leftMotorForwardPin, HIGH);
  digitalWrite(leftMotorBackwardPin, LOW);

  digitalWrite(MotorENB, SLOW_MOTOR_POWER);
  digitalWrite(rightMotorForwardPin, HIGH);
  digitalWrite(rightMotorBackwardPin, LOW);
}

// Motorların gücünü ayarlıyoruz
void maxSpeed() {
  analogWrite(MotorENA, MAX_MOTOR_POWER);
  analogWrite(MotorENB, MAX_MOTOR_POWER);
}

// Motorların yavaşlamasını sağlamak için
void slowSpeed() {
  analogWrite(MotorENA, SLOW_MOTOR_POWER);
  analogWrite(MotorENB, SLOW_MOTOR_POWER);
}

void backSpeed() {
  analogWrite(MotorENA, BACK_MOTOR_POWER);
  analogWrite(MotorENB, BACK_MOTOR_POWER);
}

// Sağ/Sol dönüşler için
void slowDown(byte primaryMotor, byte secondaryMotor) {
  analogWrite(primaryMotor, M_MOTOR_POWER);    //Duran motor
  analogWrite(secondaryMotor, L_MOTOR_POWER);  //Çalışan Motor
  delay(10);
}

// Olası bir sıkışmada aracın geri gitmesini sağlamak için
void goBack() {
  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, HIGH);

  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, HIGH);
}

// Motorları durdurmak için
void stopMotor() {
  analogWrite(MotorENA, 0);
  analogWrite(MotorENB, 0);

  digitalWrite(leftMotorForwardPin, LOW);
  digitalWrite(leftMotorBackwardPin, LOW);

  digitalWrite(rightMotorForwardPin, LOW);
  digitalWrite(rightMotorBackwardPin, LOW);
}


//Acilis Far
void opCl() {
  digitalWrite(FAR_PIN, HIGH);
  delay(100);
  digitalWrite(FAR_PIN, LOW);
  delay(100);
  digitalWrite(FAR_PIN, HIGH);
  delay(100);
  digitalWrite(FAR_PIN, LOW);
}

void FarSelector() {
  digitalWrite(FAR_PIN, LOW);
  digitalWrite(FAR_PIN, HIGH);
  delay(100);
  digitalWrite(FAR_PIN, LOW);
  delay(100);
  digitalWrite(FAR_PIN, HIGH);
  delay(100);
  digitalWrite(FAR_PIN, LOW);
  delay(100);
  digitalWrite(FAR_PIN, HIGH);
  delay(100);
  digitalWrite(FAR_PIN, LOW);
}

void selectorCtrl(int distanceSensorOrta) {
  if (distanceSensorOrta < 30 && distanceSensorOrta != -1) {
    FarSelector();
  }
}