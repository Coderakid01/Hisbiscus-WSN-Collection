#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;

  float prox;
  float alt;
  float pres;
  float humid;
  float temp; 

}struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

// Create an array with all the structures
struct_message boardsStruct[3] = {board1, board2};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];

 Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);

  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);

  boardsStruct[myData.id].prox = myData.prox;
  boardsStruct[myData.id].alt = myData.alt;
  boardsStruct[myData.id].pres = myData.pres;
  boardsStruct[myData.id].humid = myData.humid;
  boardsStruct[myData.id].temp = myData.temp;
  
  Serial.printf("Proximity value: %f \n", boardsStruct[myData.id].prox);
  Serial.printf("Altitude value: %f \n", boardsStruct[myData.id].alt);
  Serial.printf("Pressure value: %f \n", boardsStruct[myData.id].pres);
  Serial.printf("Humidity value: %f \n", boardsStruct[myData.id].humid);
  Serial.printf("Temperature value: %f \n", boardsStruct[myData.id].temp);
  Serial.println();  
  
}
 
void setup() {
  //Initialize Serial Monitor
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
