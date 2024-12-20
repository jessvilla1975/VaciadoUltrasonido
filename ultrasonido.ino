#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD I2C con dirección 0x27 y 16x2 caracteres

// Pines para sensores y actuadores
int releValvula = 13;      // Pin del relé válvula
int relepin2 = 11;         // Pin del relé motobomba
int ledPin = 2;            // LED para estado de la motobomba
int ledPin2 = 3;           // LED para la segunda condición
int ledPin3 = 4;           // LED para la tercera condición
int sensorIRPin = 6;       // Pin del sensor infrarrojo

// Pines del sensor ultrasónico
const int trigPin = 9;     // Pin TRIG del sensor ultrasónico
const int echoPin = 10;    // Pin ECHO del sensor ultrasónico

// Variables para lógica
int objectCount = 0;       // Contador de objetos detectados
bool objectDetected = false; // Estado de detección infrarrojo
long duration;             // Duración del pulso ultrasónico
float distance;            // Distancia medida en cm
bool motobombaDesactivada = false; // Bandera para desactivar la motobomba permanentemente
float caudal = 0.0;        // Variable para almacenar el caudal aleatorio

void mensajeInicio() {
  lcd.setCursor(0, 0);
  lcd.print("Sistema Iniciado");
  delay(2000);
  lcd.clear();
}

void mensajeDistanciaAgua() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("D:");
  lcd.print(distance);
  lcd.print("cm");
  lcd.setCursor(11, 0);
  lcd.print("CV:");
  lcd.print(objectCount);
}

void mensajeCaudal() {
  lcd.clear();
  lcd.setCursor(0, 0); // Fila 2, columna 0
  lcd.print("Caudal: ");
  lcd.print(caudal, 2);  // Mostrar el caudal con 2 decimales
  lcd.print(" ml/s");
}

void setup() {
  // Inicialización del LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Configuración de pines
  pinMode(releValvula, OUTPUT);
  digitalWrite(releValvula, HIGH); // Asegurarse de que la válvula esté cerrada
  pinMode(relepin2, OUTPUT);
  digitalWrite(relepin2, LOW);
  pinMode(sensorIRPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);

  // Mensaje de inicio
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
  if (!motobombaDesactivada && distance >= 3.5 && distance <= 4.5) {
    digitalWrite(relepin2, LOW);  // Apagar la motobomba
    digitalWrite(ledPin, LOW);    // Encender LED de "Lleno"
    digitalWrite(ledPin2, HIGH);
    digitalWrite(ledPin3, HIGH);
    motobombaDesactivada = true; // Desactivar permanentemente la motobomba
  }

  if (!motobombaDesactivada) {
    // Solo controlar la motobomba si no está desactivada permanentemente
    if (distance >= 10.59 && distance <= 11.27) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin3, HIGH);
      digitalWrite(ledPin2, LOW); // Encender el segundo LED
    } else if (distance >= 13.34 && distance <= 15.21) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, LOW); // Encender el tercer LED
    } else {
      // Condición por defecto
      digitalWrite(relepin2, HIGH); // Encender la motobomba
      digitalWrite(releValvula, HIGH);
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
    }
  }

  // *** Lectura del sensor infrarrojo ***
  int sensorState = digitalRead(sensorIRPin);

  if (sensorState == HIGH && !objectDetected) {
    // Si el sensor detecta un objeto y no ha sido detectado previamente
    digitalWrite(releValvula, LOW); // Abrir válvula
    caudal = random(125, 246) / 100.0; // Generar un caudal aleatorio entre 1.25 y 2.45
    mensajeCaudal();  // Mostrar caudal en el LCD
    
    delay(3000); // Mantener abierta la válvula por 3 segundos
    digitalWrite(releValvula, HIGH); // Cerrar válvula automáticamente
    
    objectCount++;                  // Incrementar contador de objetos
    objectDetected = true;          // Marcar objeto detectado
  } else if (sensorState == LOW && objectDetected) {
    // Si el sensor deja de detectar un objeto
    objectDetected = false;         // Reiniciar estado
  }

  delay(500); // Pausa antes de la siguiente lectura
}
