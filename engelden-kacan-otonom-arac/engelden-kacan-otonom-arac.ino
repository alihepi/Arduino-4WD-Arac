#include <HCSR04.h>

int MAX_MOTOR_POWER = 125;
int SLOW_MOTOR_POWER = 75;
int L_MOTOR_POWER = 180;
int M_MOTOR_POWER = 25;
int MIN_MOTOR_POWER = 10;

// HC-SR04 sensör pinleri
const byte trigPin1 = 6;
const byte echoPin1 = 7;

const byte trigPin2 = 8;
const byte echoPin2 = 9;

const byte trigPin3 = 12;
const byte echoPin3 = 13;

// Motor pinler
const byte MotorENA = 10;
const byte leftMotorForwardPin = 2;
const byte leftMotorBackwardPin = 3;

const byte MotorENB = 11;
const byte rightMotorForwardPin = 5;
const byte rightMotorBackwardPin = 4;


// LDR (Foto Direnç) pini
int LDR_PIN = A1;

// Bluetooth değerleri için değişken
char btValue = '0';

// Sensor Tanımlama
UltraSonicDistanceSensor sensor1(trigPin1, echoPin1);
UltraSonicDistanceSensor sensor2(trigPin2, echoPin2);
UltraSonicDistanceSensor sensor3(trigPin3, echoPin3);

void setup() {
  Serial.begin(9600);

  // Değer göndereceğimiz pinleri çıkış pinleri yapıyoruz
  pinMode(MotorENA, OUTPUT);
  pinMode(leftMotorForwardPin, OUTPUT);
  pinMode(leftMotorBackwardPin, OUTPUT);

  pinMode(MotorENB, OUTPUT);
  pinMode(rightMotorForwardPin, OUTPUT);
  pinMode(rightMotorBackwardPin, OUTPUT);

  pinMode(LDR_PIN, OUTPUT);
}

void loop() {
  // Sensor değerleri
  float distanceSensorSol = sensor1.measureDistanceCm();
  float distanceSensorOrta = sensor2.measureDistanceCm();
  float distanceSensorSag = sensor3.measureDistanceCm();

  // LDR (Foto Direnç) değeri
  int lightValue = analogRead(LDR_PIN);

  // HC05 Bluetooth Modülü ile seri haberleşme
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();
    if (receivedChar == '1') {
      btValue = '1';
    } else if (receivedChar == 'e') {
      btValue = 'e';
    }
  }

  Serial.println(btValue);

  if (btValue == '1') {
    Serial.print("Sol: ");
    Serial.print(distanceSensorSol);
    Serial.print(", Orta: ");
    Serial.print(distanceSensorOrta);
    Serial.print(", Sağ: ");
    Serial.print(distanceSensorSag);

    Serial.print(", Isik: ");
    Serial.println(lightValue);

    // Motorlarımızı çalıştırıp belirlediğimiz son hızda ilerletiyoruz.
    startMotors();
    maxSpeed();

    // Sensorlerin hassasiyetlerinden dolayı mesafeler değişmektedir.
    if (distanceSensorSol < 40) {
      slowDown(MotorENB, MotorENA);
    } else if (distanceSensorSag < 50) {
      slowDown(MotorENA, MotorENB);
    } else if (lightValue == 0) {
      // Yarışma kuralları gereği karanlıkta bir kere durma kuralından dolayı
      stopMotor();
      delay(3000);  // 3sn karanlıkta bekledi
      maxSpeed();
      delay(5000);  // 5sn de karanlık tünelden geçerek tamamladı
                    // Tünel uzunluğuna göre ayarlanabilir
    } else if (distanceSensorOrta < 9) {
      slowSpeed();
      // Araç, karşısında bir cisimle karşılaştığında hızını azaltıp
      // Manevra kabiliyetini kolaylaştırmak için
    } else if (distanceSensorSag > 55 && distanceSensorSag > 70) {
      if(distanceSensorOrta < 35){
        stopMotor();
      }
      // Sağ ve solda (parkurdan dolayı önceden olçulmuş değerlerle) boşluk algılandığında
      // Durma noktasına gelindiğinde karşılaşılan duvar engelinin önünde durması için
    } else {
      maxSpeed();
      // Hiçbir engelle karşılaşılmadığında son hız devam etmesi
    }

    // BT ile manuel durdurmak
  } else if (btValue == 'e') {
    stopMotor();
  }
}

// Motorların ileri gitmeleri için güç verme kodu
void startMotors() {
  digitalWrite(MotorENA, HIGH);
  digitalWrite(leftMotorForwardPin, HIGH);
  digitalWrite(leftMotorBackwardPin, LOW);

  digitalWrite(MotorENB, HIGH);
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

  analogWrite(MotorENA, SLOW_MOTOR_POWER);
  analogWrite(MotorENB, SLOW_MOTOR_POWER);
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
