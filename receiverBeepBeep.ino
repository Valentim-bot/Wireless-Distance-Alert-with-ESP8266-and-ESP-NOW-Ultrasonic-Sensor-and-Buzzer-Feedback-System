#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <LCD-I2C.h>


// Initialize the LCD: I2C address 0x27, 16 columns, 2 rows
LCD_I2C lcd(0x27, 16, 2); // Address 0x27, Columns 16, rows 2

/*
ESP8266........................LCD I2C DISPLAY

D1, GPIO5 ...................... SCL 

D2, GPIO4 ...................... SDA  

Vin ............................ Vcc 

GND ............................ GND

*/

const int buzzer = D3;
const int ledPin = D4;

// Structure to receive ultrasonic sensor data
typedef struct ultra_messagedata {
  double distance_cm, distance_m;
  double time = 0;
} ultra_messagedata;

ultra_messagedata mySonicData;

String lastMessage = ""; // Used to prevent repeated LCD updates

void displayMessage(const String& message) {
  if (message != lastMessage) {
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print(message);
    lastMessage = message;
  }
}

// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&mySonicData, incomingData, sizeof(mySonicData));
  
  double distance = mySonicData.distance_cm;

  if (distance < 0.0 || distance > 400.0) {
    Serial.println("No distance detected");
    lcd.clear();
    delay(300);
    lcd.setCursor(0, 0);
    lcd.print("No distance");
    noTone(buzzer); // Turn off buzzer if invalid
    return;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("distance_cm:");
  lcd.setCursor(0, 1);
  lcd.print(distance, 2);
  lcd.print(" cm");

  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print(". distance_cm: ");
  Serial.println(distance);

  // --- Buzzer logic: fast beep when close ---
  if (distance <= 15.0 && distance >= 2.3) {
    // Map distance from 15cm to 2.3cm → delay from 500ms to 50ms
    int beepDelay = map((int)(distance * 10), 150, 23, 500, 50); // multiplied by 10 to use integers

    tone(buzzer, 2000);         // Play tone (2kHz)
    delay(50);                  // Duration of beep
    noTone(buzzer);             // Stop tone
    delay(beepDelay);           // Delay before next beep
  } else {
    noTone(buzzer); // Outside range, silence
  }
}

void setup() {
  pinMode(buzzer, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

 Wire.begin();
 lcd.begin(&Wire);
  lcd.display();
  lcd.backlight();
  displayMessage("Waiting Data...");

  WiFi.mode(WIFI_STA); // Set as WiFi Station

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  //delay(1000);
}

void loop() {
  // No need for code here
  
}
