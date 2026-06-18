/*
 * ESTACIÓN METEOROLÓGICA PORTÁTIL - PRUEBA
 * Comunicación I2C compartida (Sensor BMP280 + Pantalla OLED SSD1306)
 * Pines de hardware adaptados para ESP32-C6
 */

#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definición de pines I2C para el ESP32-C6
#define I2C_SDA   6   // Pin SDA compartido
#define I2C_SCL   7   // Pin SCL compartido

// Configuración de la Pantalla OLED
#define SCREEN_WIDTH  128 // Ancho de la pantalla en píxeles
#define SCREEN_HEIGHT  64 // Alto de la pantalla en píxeles (cambiar a 32 si tu pantalla es más delgada)
#define OLED_RESET     -1 // Pin de reset (se pone -1 si comparte el pin de reset del ESP32)
#define SCREEN_ADDRESS 0x3C // Dirección I2C típica de las pantallas OLED SSD1306

// Crear los objetos para el sensor y la pantalla
Adafruit_BMP280 bmp; 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n======================================");
  Serial.println("   SISTEMA: BMP280 + PANTALLA OLED    ");
  Serial.println("======================================");

  // 1. Inicializar el bus I2C maestro con tus GPIOs personalizados
  Wire.begin(I2C_SDA, I2C_SCL);

  // 2. Inicializar la Pantalla OLED
  // SSD1306_SWITCHCAPVCC genera los voltajes altos de la pantalla internamente desde los 3.3V
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("ERROR: No se detecta la pantalla OLED SSD1306."));
    while(true); // Bloquear si falla la pantalla
  }
  
  // Mostrar pantalla de bienvenida rápida
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("Iniciando Estacion...");
  display.display();
  delay(1500);

  // 3. Inicializar el Sensor BMP280 (con SDD/SDO amarrado a GND)
  if (!bmp.begin(0x76)) {
    Serial.println("ERROR: No se detecta el sensor BMP280.");
    
    // Mostrar el error físicamente en la pantalla para diagnóstico rápido
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("ERROR HARDWARE:");
    display.println("No se encuentra BMP280");
    display.display();
    while (true);
  }

  Serial.println("Hardware inicializado correctamente.");
  
  // Configuración de muestreo optimizada para el BMP280
  bmp.setSampling(
    Adafruit_BMP280::MODE_NORMAL,
    Adafruit_BMP280::SAMPLING_X2,
    Adafruit_BMP280::SAMPLING_X16,
    Adafruit_BMP280::FILTER_X16,
    Adafruit_BMP280::STANDBY_MS_500
  );
}

void loop() {
  // Lectura de variables del sensor
  float temperatura = bmp.readTemperature();
  float presion = bmp.readPressure() / 100.0; // Convertir Pa a hPa

  // --- IMPRESIÓN EN MONITOR SERIE (Para depuración) ---
  Serial.println("--------------------------------------");
  Serial.print("Temperatura: "); Serial.print(temperatura); Serial.println(" °C");
  Serial.print("Presion: ");     Serial.print(presion);     Serial.println(" hPa");

  // --- IMPRESIÓN EN LA PANTALLA OLED ---
  display.clearDisplay(); // Limpia el búfer de la pantalla para refrescar los datos
  
  // Título o Encabezado
  display.setTextSize(1);
  display.setCursor(15, 0);
  display.println("METEO STATION C6");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE); // Línea decorativa horizontal

  // Mostrar Temperatura
  display.setCursor(0, 20);
  display.print("Temp: ");
  display.setTextSize(2); // Texto más grande para la lectura principal
  display.print(temperatura, 1); // Imprime con 1 decimal
  display.setTextSize(1);
  display.print(" C");

  // Mostrar Presión Atmosférica
  display.setCursor(0, 45);
  display.setTextSize(1);
  display.print("Pres: ");
  display.setTextSize(2);
  display.print(presion, 1);
  display.setTextSize(1);
  display.print(" hPa");

  // Enviar todo lo dibujado a la pantalla física
  display.display(); 

  delay(1000); // Muestreo cada segundo
}