/*
  ############ Proyecto Casa Domótica ############
  -Módulo: Entrada
  -Funcionalidad:
    *Controlar las entradas con servos
    *Gestionar la seguridad mediante RFID
    *Detectar presencia para encender la luz
  -Integrantes:
    * 220087 - Eli Haziel Ortiz Ramírez
    * 220732 - Irving Morales Domínguez
*/
/*servo motor driver board control
   Home Page
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// RFID
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  //create a MIFARE_Key struct named 'key', which will hold the card information

//this is the block number we will write into and then read.
int block = 2;
int buzzerPIN = 7;

Adafruit_PWMServoDriver srituhobby = Adafruit_PWMServoDriver();

#define servoMIN 150
#define servoMAX 600
int servoPin = 0;  // Ajusta el número del pin del servo según tu configuración
int anguloApertura = 90;  // Define el ángulo de apertura deseado
unsigned long tiempoInicio;


void setup() {
  // Comunicación Serial
  Serial.begin(9600);  // Inicia la comunicación serie a 9600 bps
  Wire.begin(9);       // Dirección I2C del Arduino Uno

  // PCA
  srituhobby.begin();
  srituhobby.setPWMFreq(60);

  // RFID
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");

  pinMode(buzzerPIN, HIGH);
}

void loop() {

  // RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Tarjeta detectada! UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();

    // Verifica si el UID coincide con el esperado
    if (compararUID(mfrc522.uid.uidByte)) {
      tiempoInicio = millis();  // Inicia el temporizador
      abrirPuerta();            // Función para abrir la puerta (girar el servo)
    }

    if (compararUID2(mfrc522.uid.uidByte)) {
      tiempoInicio = millis();  // Inicia el temporizador
      abrirPuerta2();            // Función para abrir la puerta (girar el servo)
    }

    mfrc522.PICC_HaltA();
  }

  // Verifica si ha pasado el tiempo deseado y cierra la puerta
  if (millis() - tiempoInicio > 2500) {
    cerrarPuerta();  // Función para cerrar la puerta (volver a la posición inicial)
    cerrarPuerta2();  // Función para cerrar la puerta (volver a la posición inicial)
  }
}

// Función para comparar el UID con el esperado
bool compararUID(byte* uid) {
  // Cambia estos valores por el UID específico que estás buscando
  byte UID_esperado[] = {0x63, 0x42, 0x96, 0x2F};

  for (int i = 0; i < 4; i++) {
    if (uid[i] != UID_esperado[i]) {
      return false;  // No coincide el UID
    }
  }
  return true;  // Coincide el UID
}
bool compararUID2(byte* uid) {
  // Cambia estos valores por el UID específico que estás buscando
  byte UID_esperado[] = {0x73, 0xF1, 0x75, 0xA6};

  for (int i = 0; i < 4; i++) {
    if (uid[i] != UID_esperado[i]) {
      return false;  // No coincide el UID
    }
  }
  return true;  // Coincide el UID
}

// Función para abrir la puerta (girar el servo)
void abrirPuerta() {
  anguloApertura = 90;  // Define el ángulo de apertura deseado
  int pulsos = map(anguloApertura, 0, 180, servoMIN, servoMAX);
  srituhobby.setPWM(0, 0, pulsos);  // Ajusta el número del pin del servo según tu configuración
}
void abrirPuerta2() {
  anguloApertura = 120;  // Define el ángulo de apertura deseado
  int pulsos = map(anguloApertura, 0, 180, servoMIN, servoMAX);
  srituhobby.setPWM(1, 0, pulsos);  // Ajusta el número del pin del servo según tu configuración
}

// Función para cerrar la puerta (volver a la posición inicial)
void cerrarPuerta() {
  int pulsos = map(servoPin, 0, 180, servoMIN, servoMAX);
  srituhobby.setPWM(servoPin, 0, pulsos);  // Ajusta el número del pin del servo según tu configuración
}

void cerrarPuerta2() {
  int pulsos = map(1, 0, 180, servoMIN, servoMAX);
  srituhobby.setPWM(1, 0, pulsos);  // Ajusta el número del pin del servo según tu configuración
}
