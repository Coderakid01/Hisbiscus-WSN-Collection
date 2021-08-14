#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_APDS9960.h> // include the Adafruit_APDS9960 header file.
#include <Adafruit_BME280.h> // include the Adafruit_BME280 header file.
#include <Adafruit_MPU6050.h> // include the Adafruit_MPU6050 header file.

Adafruit_APDS9960 apds; // instance of the Adafruit_APDS9960 class named as apds.
Adafruit_BME280 bme; // instance of the Adafruit_BME280 class named as bme.
Adafruit_MPU6050 mpu; // instance of the Adafruit_MPU6050 class named as mpu.

sensors_event_t a, g, temp;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
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

  // Initialize MPU6050 sensor and if the connection is not successful,
  // print the failed status to the Serial Monitor.
  if (!mpu.begin()){
    Serial.println("Failed to find Hibiscus Sense MPU6050 chip");
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

  myData.prox = apds.readProximity();
  myData.alt = bme.readAltitude(1013.25);
  myData.pres = bme.readPressure()/100.00;
  myData.humid = bme.readHumidity();
  myData.temp = bme.readTemperature();
  
  myData.accx = a.acceleration.x;
  myData.accy = a.acceleration.y;
  myData.accz = a.acceleration.z;

  myData.gyrox = g.gyro.x;
  myData.gyroy = g.gyro.y;
  myData.gyroz = g.gyro.z;

  mpu.getEvent(&a,&g,&temp);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
