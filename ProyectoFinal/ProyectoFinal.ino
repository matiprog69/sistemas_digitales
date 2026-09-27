// =====================================================
// LIBRERÍAS
// =====================================================

// Incluye la librería Wire.
// Esta librería permite la comunicación I2C,
// que utilizaremos para conectar el LCD.
#include <Wire.h>

// Incluye la librería para controlar el LCD mediante I2C.
#include <LiquidCrystal_I2C.h>

// Incluye la librería necesaria para utilizar
// el sensor DHT22.
#include <DHT.h>


// =====================================================
// CONFIGURACIÓN DEL SENSOR DHT22
// =====================================================

// Define el pin digital donde está conectado
// el pin DATA del DHT22.
// En nuestro circuito DATA está conectado al D2.
#define DHTPIN 2

// Indica a la librería DHT qué tipo de sensor estamos usando.
// En este proyecto utilizamos DHT11.
#define DHTTYPE DHT22

// Crea un objeto llamado "dht".
// Este objeto permite que Arduino pueda leer
// temperatura y humedad del sensor.
DHT dht(DHTPIN, DHTTYPE);


// =====================================================
// CONFIGURACIÓN DEL LCD 16x2
// =====================================================

// Crea el objeto "lcd".

// 0x27 = dirección I2C del LCD.
// 16 = cantidad de columnas.
// 2 = cantidad de filas.

LiquidCrystal_I2C lcd(0x27, 16, 2);


// =====================================================
// DEFINICIÓN DE LOS PINES
// =====================================================

// El LDR está conectado a la entrada analógica A0.
#define LDR_PIN A0

// El LED verde está conectado al pin digital 9.
#define LED_VERDE 9

// El LED rojo está conectado al pin digital 10.
#define LED_ROJO 10

// El buzzer está conectado al pin digital 8.
#define BUZZER 8


// =====================================================
// LÍMITES AMBIENTALES
// =====================================================

// Temperatura máxima permitida.
// Si la temperatura supera 27 °C,
// el sistema considera que existe una alerta.
#define TEMP_MAX 25.0

// Temperatura mínima permitida.
// Si la temperatura baja de 18 °C,
// existe una alerta.
#define TEMP_MIN 20.0

// Humedad máxima permitida.
// Por encima de 65 % se genera una alerta.
#define HUM_MAX 60.0

// Humedad mínima permitida.
// Por debajo de 40 % se genera una alerta.
#define HUM_MIN 50.0

// Valor mínimo de luz considerado adecuado.
// El LDR entrega un valor entre 0 y 1023.
// Si el valor es menor que 580,
// se considera que hay poca luz.
#define LUZ_MIN 500


// =====================================================
// FUNCIÓN SETUP
// =====================================================

// setup() se ejecuta UNA SOLA VEZ
// cuando Arduino se enciende o se reinicia.
void setup() {

  // ---------------------------------------------------
  // CONFIGURAR LOS PINES
  // ---------------------------------------------------

  // Configura el pin 9 como salida.
  // Arduino utilizará este pin para controlar
  // el LED verde.
  pinMode(LED_VERDE, OUTPUT);

  // Configura el pin 10 como salida.
  // Arduino utilizará este pin para controlar
  // el LED rojo.
  pinMode(LED_ROJO, OUTPUT);

  // Configura el pin 8 como salida.
  // Arduino utilizará este pin para controlar
  // el buzzer.
  pinMode(BUZZER, OUTPUT);


  // ---------------------------------------------------
  // COMUNICACIÓN SERIAL
  // ---------------------------------------------------

  // Inicia la comunicación entre Arduino
  // y el Monitor Serial a una velocidad de 9600 baudios.
  Serial.begin(9600);


  // ---------------------------------------------------
  // INICIAR SENSOR DHT22
  // ---------------------------------------------------

  // Inicializa el sensor DHT22.
  // A partir de aquí Arduino puede realizar
  // lecturas de temperatura y humedad.
  dht.begin();


  // ---------------------------------------------------
  // INICIAR LCD
  // ---------------------------------------------------

  // Inicializa el LCD.
  lcd.init();

  // Enciende la luz de fondo del LCD.
  lcd.backlight();


  // ---------------------------------------------------
  // MENSAJE INICIAL
  // ---------------------------------------------------

  // Coloca el cursor en la columna 0,
  // fila 0 del LCD.
  lcd.setCursor(0, 0);

  // Escribe "MONITOREO" en la primera fila.
  lcd.print("MONITOREO");


  // Coloca el cursor en la columna 0,
  // fila 1 del LCD.
  lcd.setCursor(0, 1);

  // Escribe "AMBIENTAL" en la segunda fila.
  lcd.print("AMBIENTAL");


  // Espera 2 segundos.
  // 2000 milisegundos = 2 segundos.
  delay(2000);

  // Borra todo lo que aparece actualmente
  // en la pantalla LCD.
  lcd.clear();
}


// =====================================================
// FUNCIÓN LOOP
// =====================================================

// loop() se ejecuta continuamente.
// Cuando termina, Arduino vuelve al principio
// y lo ejecuta nuevamente.
void loop() {


  // ===================================================
  // LEER EL DHT22
  // ===================================================

  // Lee la temperatura del DHT22
  // y guarda el resultado en la variable "temperatura".
  //
  // float permite almacenar números con decimales.
  float temperatura = dht.readTemperature();


  // Lee la humedad del DHT22
  // y guarda el resultado en la variable "humedad".
  float humedad = dht.readHumidity();


  // ===================================================
  // LEER EL LDR
  // ===================================================

  // Lee el voltaje producido por el divisor de tensión
  // formado por el LDR y la resistencia de 10 kΩ.
  //
  // analogRead() devuelve un valor entre 0 y 1023.
  int luz = analogRead(LDR_PIN);


  // ===================================================
  // COMPROBAR SI EL DHT22 FUNCIONA
  // ===================================================

  // isnan() significa "is not a number".
  //
  // Comprueba si la lectura de temperatura o humedad
  // no es válida.
  if (isnan(temperatura) || isnan(humedad)) {


    // Borra la pantalla LCD.
    lcd.clear();


    // Coloca el cursor en la primera fila.
    lcd.setCursor(0, 0);

    // Muestra un mensaje indicando
    // que existe un problema con el DHT22.
    lcd.print("ERROR DHT22");


    // Coloca el cursor en la segunda fila.
    lcd.setCursor(0, 1);

    // Indica que se deben revisar las conexiones.
    lcd.print("Revise conexion");


    // Apaga el LED verde.
    digitalWrite(LED_VERDE, LOW);

    // Enciende el LED rojo.
    digitalWrite(LED_ROJO, HIGH);


    // Activa el buzzer con una frecuencia
    // de 1000 Hz.
    tone(BUZZER, 1000);


    // Envía un mensaje al Monitor Serial.
    Serial.println("ERROR: No se puede leer el DHT22");


    // Espera 2 segundos.
    delay(2000);


    // Sale de la función loop() actual
    // y vuelve a comenzar una nueva lectura.
    return;
  }


  // ===================================================
  // MOSTRAR DATOS EN EL MONITOR SERIAL
  // ===================================================

  // Escribe "Temperatura: " en el Monitor Serial.
  Serial.print("Temperatura: ");

  // Muestra el valor de temperatura.
  Serial.print(temperatura);

  // Escribe la unidad °C.
  Serial.println(" C");


  // Escribe "Humedad: ".
  Serial.print("Humedad: ");

  // Muestra el valor de humedad.
  Serial.print(humedad);

  // Escribe el símbolo % y pasa a la siguiente línea.
  Serial.println(" %");


  // Escribe "Luz: ".
  Serial.print("Luz: ");

  // Muestra el valor obtenido del LDR.
  Serial.println(luz);


  // Imprime una línea para separar
  // las diferentes mediciones.
  Serial.println("--------------------");


  // ===================================================
  // MOSTRAR TEMPERATURA Y HUMEDAD EN EL LCD
  // ===================================================

  // Borra la información anterior del LCD.
  lcd.clear();


  // Coloca el cursor en columna 0, fila 0.
  lcd.setCursor(0, 0);

  // Escribe "Temp:".
  lcd.print("Temp:");

  // Muestra la temperatura.
  //
  // El segundo parámetro "1" indica que queremos
  // mostrar solamente un decimal.
  lcd.print(temperatura, 1);

  // (char)223 representa el símbolo de grados.
  lcd.print((char)223);

  // Escribe la letra C.
  lcd.print("C");


  // Coloca el cursor en columna 0, fila 1.
  lcd.setCursor(0, 1);

  // Escribe "Hum:".
  lcd.print("Hum:");

  // Muestra la humedad sin decimales.
  lcd.print(humedad, 0);

  // Escribe el símbolo de porcentaje.
  lcd.print("%");


  // Mantiene esta información visible
  // durante 2 segundos.
  delay(2000);


  // ===================================================
  // MOSTRAR ILUMINACIÓN
  // ===================================================

  // Borra la pantalla.
  lcd.clear();


  // Coloca el cursor en la primera fila.
  lcd.setCursor(0, 0);

  // Escribe "Luz:".
  lcd.print("Luz:");


  // Muestra el valor obtenido del LDR.
  lcd.print(luz);


  // Coloca el cursor en la segunda fila.
  lcd.setCursor(0, 1);


  // Comprueba si la iluminación
  // es igual o superior a 580.
  if (luz >= LUZ_MIN) {

    // Si es mayor o igual a 580,
    // muestra "Luz adecuada".
    lcd.print("Luz adecuada");

  }

  // Si la condición anterior NO se cumple:
  else {

    // Muestra "Luz baja".
    lcd.print("Luz baja");
  }


  // Mantiene la información durante 2 segundos.
  delay(2000);


  // ===================================================
  // EVALUAR LAS CONDICIONES AMBIENTALES
  // ===================================================

  // Comprueba si la temperatura está dentro
  // del rango permitido.
  //
  // true  = temperatura correcta.
  // false = temperatura fuera del rango.
  bool temperaturaOK =
    (temperatura >= TEMP_MIN && temperatura <= TEMP_MAX);


  // Comprueba si la humedad está dentro
  // del rango permitido.
  bool humedadOK =
    (humedad >= HUM_MIN && humedad <= HUM_MAX);


  // Comprueba si la iluminación
  // es suficiente.
  bool luzOK =
    (luz >= LUZ_MIN);


  // ===================================================
  // COMPROBAR SI TODO ESTÁ NORMAL
  // ===================================================

  // Aquí se comprueba que las TRES condiciones
  // sean verdaderas:
  //
  // temperaturaOK = true
  // humedadOK     = true
  // luzOK         = true
  //
  // El operador && significa "Y".
  if (temperaturaOK && humedadOK && luzOK) {


    // Enciende el LED verde.
    digitalWrite(LED_VERDE, HIGH);


    // Apaga el LED rojo.
    digitalWrite(LED_ROJO, LOW);


    // Apaga el buzzer.
    noTone(BUZZER);


    // Borra el LCD.
    lcd.clear();


    // Coloca el cursor en la primera fila.
    lcd.setCursor(0, 0);

    // Muestra "ESTADO:".
    lcd.print("ESTADO:");


    // Coloca el cursor en la segunda fila.
    lcd.setCursor(0, 1);

    // Indica que las condiciones ambientales
    // son normales.
    lcd.print("NORMAL");


  }


  // ===================================================
  // SI EXISTE ALGUNA ALERTA
  // ===================================================

  else {


    // Apaga el LED verde.
    digitalWrite(LED_VERDE, LOW);


    // Enciende el LED rojo.
    digitalWrite(LED_ROJO, HIGH);


    // Activa el buzzer a 1000 Hz.
    tone(BUZZER, 1000);


    // Borra la pantalla.
    lcd.clear();


    // Coloca el cursor en la primera fila.
    lcd.setCursor(0, 0);

    // Muestra "ALERTA".
    lcd.print("ALERTA");


    // =================================================
    // COMPROBAR TEMPERATURA
    // =================================================

    // Si temperaturaOK es falso,
    // significa que la temperatura está fuera
    // del rango de 18 a 27 °C.
    if (!temperaturaOK) {


      // Coloca el cursor en la segunda fila.
      lcd.setCursor(0, 1);


      // Comprueba si la temperatura
      // supera el máximo.
      if (temperatura > TEMP_MAX) {

        // Muestra que la temperatura es alta.
        lcd.print("Temp. ALTA");

      }

      // Si no supera el máximo,
      // significa que está por debajo del mínimo.
      else {

        // Muestra que la temperatura es baja.
        lcd.print("Temp. BAJA");
      }
    }


    // =================================================
    // COMPROBAR HUMEDAD
    // =================================================

    // Si la temperatura está correcta,
    // se comprueba la humedad.
    else if (!humedadOK) {


      // Coloca el cursor en la segunda fila.
      lcd.setCursor(0, 1);


      // Comprueba si la humedad
      // supera el máximo.
      if (humedad > HUM_MAX) {

        // Muestra humedad alta.
        lcd.print("Hum. ALTA");

      }

      // Si no supera el máximo,
      // significa que está por debajo del mínimo.
      else {

        // Muestra humedad baja.
        lcd.print("Hum. BAJA");
      }
    }


    // =================================================
    // COMPROBAR ILUMINACIÓN
    // =================================================

    // Si temperatura y humedad están correctas,
    // se comprueba la iluminación.
    else if (!luzOK) {


      // Coloca el cursor en la segunda fila.
      lcd.setCursor(0, 1);


      // Indica que existe poca iluminación.
      lcd.print("Luz BAJA");
    }
  }


  // ===================================================
  // ESPERA ANTES DE REPETIR EL PROCESO
  // ===================================================

  // Espera 2 segundos.
  //
  // Después de estos 2 segundos,
  // loop() vuelve a comenzar desde arriba.
  delay(2000);
}