//----------------------------------------Including Libraries.
#include <Adafruit_NeoPixel.h>
//----------------------------------------

#define ON_BOARD_IO2LED_PIN          2
#define ON_BOARD_RGBLED_WS2812_PIN   48

#define NUMPIXELS  1  //--> Only 1 ON BOARD RGB LED WS2812.

int cnt = 0;


// Declaring the "Adafruit_NeoPixel" object as "pixels" and its settings.
Adafruit_NeoPixel pixels(NUMPIXELS, ON_BOARD_RGBLED_WS2812_PIN, NEO_GRB + NEO_KHZ800);



//________________________________________________________________________________ VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  delay(1000);
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  pinMode(ON_BOARD_IO2LED_PIN, OUTPUT);
  digitalWrite(ON_BOARD_IO2LED_PIN, LOW);

  //---------------------------------------- Checking PSRAM.
  // Check if PSRAM is found by the system.
  if (psramFound()) {
    Serial.println("PSRAM found.");
    
    // Print the total size and free memory of PSRAM.
    Serial.printf("Total PSRAM : %d bytes (%d kB).\n", ESP.getPsramSize(), ESP.getPsramSize() / 1024);
    Serial.printf("Free PSRAM  : %d bytes (%d kB).\n", ESP.getFreePsram(), ESP.getFreePsram() / 1024);
  } else {
    Serial.println("PSRAM was not found or is disabled.");
  }
  Serial.println();
  //---------------------------------------- 

  // INITIALIZE NeoPixel strip object (REQUIRED).
  pixels.begin();
  // Set all pixel colors to 'off'.
  pixels.clear();
  pixels.show();
}
//________________________________________________________________________________ 



//________________________________________________________________________________ VOID LOOP
void loop() {
  // put your main code here, to run repeatedly:

  Serial.println();
  for (byte i = 0; i < 6; i++) {
    // Toggle the On-board LED (IO2).
    digitalWrite(ON_BOARD_IO2LED_PIN,!digitalRead(ON_BOARD_IO2LED_PIN));
    if (digitalRead(ON_BOARD_IO2LED_PIN) == HIGH) Serial.println("On-board LED (IO2) : ON");
    if (digitalRead(ON_BOARD_IO2LED_PIN) == LOW)  Serial.println("On-board LED (IO2) : OFF");
    delay(1000);
  }

  Serial.println();
  for (byte j = 0; j < 5; j++) {
    if (j == 0) {
      // Set the WS2812 LED to Red.
      Serial.println("On-board WS2812 LED : Red");
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      pixels.show();
    } else if (j == 1) {
      // Set the WS2812 LED to Green.
      Serial.println("On-board WS2812 LED : Green");
      pixels.setPixelColor(0, pixels.Color(0, 255, 0));
      pixels.show();
    } else if (j == 2) {
      // Set the WS2812 LED to Blue.
      Serial.println("On-board WS2812 LED : Blue");
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      pixels.show();
    } else if (j == 3) {
      // Set the WS2812 LED to White.
      Serial.println("On-board WS2812 LED : White");
      pixels.setPixelColor(0, pixels.Color(255, 255, 255));
      pixels.show();
    } else if (j == 4) {
      // Set the WS2812 LED to OFF.
      Serial.println("On-board WS2812 LED : OFF");
      pixels.clear();
      pixels.show();
    } 
    delay(1000);
  }
}
//________________________________________________________________________________ 
