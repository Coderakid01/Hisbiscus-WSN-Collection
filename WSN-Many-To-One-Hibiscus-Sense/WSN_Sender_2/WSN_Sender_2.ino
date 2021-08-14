#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_APDS9960.h> // include the Adafruit_APDS9960 header file.
#include <Adafruit_BME280.h> // include the Adafruit_BME280 header file.

Adafruit_APDS9960 apds; // instance of the Adafruit_APDS9960 class named as apds.
Adafruit_BME280 bme; // instance of the Adafruit_BME280 class named as bme.

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int id; // must be unique for each sender board

  float prox;
  float alt;
  float pres;
  float humid;
  float temp; 

} struct_message;

//Create a struct_message called myData
struct_message myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

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

}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

    // Initialize APDS9960 sensor and if the connection is not successful,
  // print the failed status to the Serial Monitor.
  if (!apds.begin()){
    Serial.println("Failed to find Hibiscus Sense APDS9960 chip");
  }

 // enable the proximity mode on APDS9960
  apds.enableProximity(true);

  // Initialize BME280 sensor and if the connection is not successful,
  // print the failed status to the Serial Monitor.
  if (!bme.begin()){
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // Set values to send
  myData.id = 2;
  myData.prox = apds.readProximity();
  myData.alt = bme.readAltitude(1013.25);
  myData.pres = bme.readPressure()/100.00;
  myData.humid = bme.readHumidity();
  myData.temp = bme.readTemperature();

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(10000);
}
