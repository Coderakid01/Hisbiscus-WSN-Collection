#include <esp_now.h>
#include <WiFi.h>

// include the Adafruit_BME280 header file.
#include <Adafruit_BME280.h>

// instance of the Adafruit_BME280 class named as bme.
Adafruit_BME280 bme;

// REPLACE WITH THE RECEIVER'S MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int id; // must be unique for each sender board
    float temp;
    float humid;
    float pres;
    float alt;
} struct_message;

//Create a struct_message called myData
struct_message myData;
struct_message incomingData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;

// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2, board3};

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID : %u \n", myData.id);
  Serial.printf("Data Size : %u Bytes \n", len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].temp = myData.temp;
  boardsStruct[myData.id-1].humid = myData.humid;
  boardsStruct[myData.id-1].pres = myData.pres;
  boardsStruct[myData.id-1].alt = myData.alt;
    
  Serial.print("Temperature : ");
  Serial.print(boardsStruct[myData.id-1].temp);
  Serial.println(" ºC");
  Serial.print("Humidity : ");
  Serial.print(boardsStruct[myData.id-1].humid);
  Serial.println(" %");
  Serial.print("Pressure : ");
  Serial.print(boardsStruct[myData.id-1].pres);
  Serial.println(" hPa");
  Serial.print("Altitude : ");
  Serial.print(boardsStruct[myData.id-1].alt);
  Serial.println(" M");
  Serial.println();
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
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
  esp_now_register_recv_cb(OnDataRecv);
  
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
  myData.id = 3;
  myData.temp = bme.readTemperature();
  myData.humid = bme.readHumidity();
  myData.pres = (bme.readPressure() / 100.0F);
  myData.alt = bme.readAltitude(1013.25);
   
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
