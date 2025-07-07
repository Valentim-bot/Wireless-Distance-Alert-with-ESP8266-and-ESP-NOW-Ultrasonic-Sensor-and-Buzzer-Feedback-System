#include <ESP8266WiFi.h>
#include <espnow.h>
#include <VARSTEP_ultrasonic.h>

#define trigger_pin 12
#define echo_pin 14

// REPLACE WITH RECEIVER MAC Address
// 4C:75:25:37:1F:2C 
VARSTEP_ultrasonic my_HCSR04(trigger_pin, echo_pin); // You can create multiple instances for multiple sensors

//4c:75:25:37:1f:2c
uint8_t broadcastAddress[] = {0x4C, 0x75, 0x25, 0x37, 0x1F, 0x2C};

// Structure example to send data
// Must match the receiver structure
typedef struct ultra_messagedata {
  double distance_cm, distance_m;
  double time = 0;
  
} ultra_messagedata;

// Create a struct_message called mySonicData
ultra_messagedata mySonicData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print(". Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println(". Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
    delay(20);
    //double distance_cm, distance_m;
    

    // distance_cm = my_HCSR04.distance_cm();
    // distance_m = my_HCSR04.distance_m();

    mySonicData.distance_cm = my_HCSR04.distance_cm();
   // Serial.print(". distance_cm: ");
    //Serial.print(mySonicData.distance_cm);
    mySonicData.time = millis();
    Serial.print(". Time: ");
    Serial.print(mySonicData.time);
    Serial.print(". my_HCSR04.distance_cm();: ");
    Serial.print(my_HCSR04.distance_cm());

   /*if(distance_cm == -1.0) {
    Serial.println("No distance detected");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance_cm);
    Serial.print("cm | ");
    Serial.print(distance_m);
    Serial.println("m");
  }
*/ 
  delay(200); // We recommend 200ms minimum delay for good accuracy.

    // Send message via ESP-NOW
    esp_now_send(broadcastAddress, (uint8_t *) &mySonicData, sizeof(mySonicData));

    

}