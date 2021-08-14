#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {

  float prox;
  float alt;
  float pres;
  float humid;
  float temp; 
  float accx;
  float accy;
  float accz;
  float gyrox;
  float gyroy;
  float gyroz;

} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
 
  Serial.print("Proximity : ");
  Serial.println(myData.prox);
  Serial.print("Altitude : ");
  Serial.println(myData.alt);
  Serial.print("Pressure : ");
  Serial.println(myData.pres);
  Serial.print("Humidity : ");
  Serial.println(myData.humid);
  Serial.print("Temperature : ");
  Serial.println(myData.temp); 

  Serial.print("Acceleration X : ");
  Serial.println(myData.accx);
  Serial.print("Acceleration Y : ");
  Serial.println(myData.accy);
  Serial.print("Acceleration Z : ");
  Serial.println(myData.accz);

  Serial.print("Gyro X : ");
  Serial.println(myData.gyrox);
  Serial.print("Gyro Y : ");
  Serial.println(myData.gyroy);
  Serial.print("Gyro Z : ");
  Serial.println(myData.gyroz);

}
 
void setup() {
  
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
