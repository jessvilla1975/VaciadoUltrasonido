#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C con dirección 0x27 y 16x2 caracteres

// Pines para sensores y actuadores
int releValvula = 13;      // Pin del relé válvula
int relepin2 = 11;         // Pin del relé motobomba
int ledPin = 2;            // LED para estado de la motobomba
int ledPin2 = 3;           // LED para la segunda condición
int ledPin3 = 4;   
int sensorIRPin = 6;       // Pin del sensor infrarrojo

// Pines del sensor ultrasónico
const int trigPin = 9;     // Pin TRIG del sensor ultrasónico
const int echoPin = 10;    // Pin ECHO del sensor ultrasónico

// Variables para lógica
int objectCount = 0;       // Contador de objetos detectados
bool objectDetected = false; // Estado de detección infrarrojo
long duration;             // Duración del pulso ultrasónico
float distance;            // Distancia medida en cm

void mensajeInicio() {
  lcd.setCursor(0, 0);
  lcd.print("Sistema Iniciado");
  delay(2000);
  lcd.clear();
}

void mensajeLleno() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Tanque lleno");
  delay(500);
}

void mensajeDistanciaAgua() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm");
  lcd.setCursor(0, 1);
  lcd.print("Vasos:");
  lcd.print(objectCount);
}

void setup() {
  // Inicialización del LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Configuración de pines
  pinMode(releValvula, OUTPUT); // Configurar el pin del relé como salida
  digitalWrite(releValvula, LOW); // Asegurarse de que el relé esté inicialmente apagado
  pinMode(relepin2, OUTPUT);     // Configurar el pin del relé como salida
  digitalWrite(relepin2, LOW);   // Asegurarse de que el relé esté inicialmente apagado
  pinMode(sensorIRPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT); 
  pinMode(ledPin2, OUTPUT); 
  pinMode(ledPin3, OUTPUT); 
 
  // Mensaje de inicio en LCD
  mensajeInicio();
}

void loop() {
  // *** Lectura del sensor ultrasonido ***
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;
  
  
  // Mostrar distancia en el LCD
  mensajeDistanciaAgua();

  // *** Lógica para la motobomba y LED ***
  if (distance >= 3.5 && distance <= 4.5) {
    // Si el nivel está por debajo de 5.21 cm, apagar la motobomba y encender el LED
    digitalWrite(ledPin, LOW);   // Encender el primer LED
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(relepin2, LOW);  // Motobomba apagada
    
    //mensajeLleno();
  } else if (distance >= 11.01 && distance <= 12.06) {
    // Si la distancia está entre 11.01 cm y 12.06 cm
     digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin3, HIGH);
    digitalWrite(ledPin2, LOW); // Encender el segundo LED
   
  } else if (distance >= 13.34 && distance <= 15.21) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, LOW); // Encender el tercer LED   
   
  } 
  else {
    // Si el nivel no está en ninguna de las condiciones anteriores
    digitalWrite(relepin2, HIGH); // Motobomba encendida
    digitalWrite(releValvula, HIGH);
    digitalWrite(ledPin, HIGH);
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
  }

  // *** Lectura del sensor infrarrojo ***
  int sensorState = digitalRead(sensorIRPin);

  if (sensorState == HIGH && !objectDetected) {
    // Si el sensor detecta un objeto y no ha sido detectado previamente
    digitalWrite(releValvula, LOW); // Abrir válvula
    delay(5000);                    // Mantener abierta la válvula
    objectCount++;                  // Incrementar contador de objetos
    objectDetected = true;          // Marcar objeto detectado
  } else if (sensorState == LOW && objectDetected) {
    // Si el sensor deja de detectar un objeto
    digitalWrite(releValvula, HIGH); // Cerrar válvula
    objectDetected = false;         // Reiniciar estado
  }

  delay(500); // Pausa antes de la siguiente lectura
}
