//Once you have upload this code to any Hibiscus or the Sender one, Please press the Reset Button or RST Button to acquire the MAC Address
#include <WiFi.h>

void setup(){
  Serial.begin(115200); //Set the baud Rate at 115200
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress()); //The MAC Address will appear 
}
 
void loop(){

}
