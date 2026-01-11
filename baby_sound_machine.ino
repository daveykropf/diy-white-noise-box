#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <Adafruit_NeoPixel.h>

#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

// RGB LED setup for RP2040 Zero
#define LED_PIN    16  // RP2040 Zero RGB LED is on GPIO16
#define NUM_PIXELS 1

Adafruit_NeoPixel pixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// LED colors
void setLEDRed() {
  // pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  // pixel.show();
}

void setLEDYellow() {
  // pixel.setPixelColor(0, pixel.Color(255, 80, 0));
  // pixel.show();
}

void setup()
{
  // Initialize NeoPixel
  pixel.begin();
  pixel.setBrightness(100);  // Adjust brightness (0-255)
  setLEDRed();  // Start with red (not connected)

#if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  FPSerial.begin(9600);
#endif

  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    setLEDRed();  // Keep red - no connection
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer online"));
  delay(100);

  myDFPlayer.volume(15);
  delay(100);

  myDFPlayer.enableLoopAll();  // Loop all songs

  delay(100);
  myDFPlayer.play(1);  // Start playing

  setLEDYellow();  // Yellow - playing
  Serial.println(F("Setup complete - looping all songs"));
}

void loop()
{
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read());
  }
}

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      setLEDRed();
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      setLEDYellow();
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      setLEDRed();
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      setLEDYellow();
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      // Keep yellow - still playing (will auto-advance)
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      setLEDRed();  // Error - show red
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
