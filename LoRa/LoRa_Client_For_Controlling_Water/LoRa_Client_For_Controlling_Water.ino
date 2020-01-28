/*
  Require Library: 
  https://github.com/dragino/RadioHead
  
  Upload Data to IoT Server ThingSpeak (https://thingspeak.com/):
  Support Devices: LoRa Shield + Arduino 
  
  Example sketch showing how to read Temperature and Humidity from DHT11 sensor,  
  Then send the value to LoRa Gateway, the LoRa Gateway will send the value to the 
  IoT server
  It is designed to work with the other sketch dht11_server. 
  modified 24 11 2016
  by Edwin Chen <support@dragino.com>
  Dragino Technology Co., Limited
*/

#include <SPI.h>
#include <RH_RF95.h>

#define motorPin 7  //it can be changed if you want
#define water_on digitalWrite(motorPin, HIGH) //Turn on relay switch & motor
#define water_off digitalWrite(motorPin, LOW) //Turn off relay switch & motor

RH_RF95 rf95;
 
float frequency = 868.0;

void setup()
{
    Serial.begin(9600);
    if (!rf95.init())
        Serial.println("init failed");
//        
//    // Setup ISM frequency
    rf95.setFrequency(frequency);
//    
//    // Setup Power,dBm
    rf95.setTxPower(17);
//    
//    //rf95.setSyncWord(0x34); Not using
//    
    Serial.println("LoRa End Node Example --"); 
    Serial.println("    Water Irrigation\n");

//    for(int i = 0;i < 3; i++)
//    {
//        Serial.print(node_id[i],HEX);
//    }
//    Serial.println();
    
    pinMode(motorPin, OUTPUT);
}
 
 
void loop()
{
    int i;
    if (rf95.waitAvailableTimeout(2000))// Listen Data from LoRa Node
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//receive data buffer
        uint8_t len = sizeof(buf);//data buffer length
        if (rf95.recv(buf, &len))//Check if there is incoming data
        {
            if(buf[0] == 'c' && buf[1] == 1)
            {
                Serial.print("Got Packet for gateway: ");
                for(i = 0; i < 3; i++){
                    Serial.print(buf[i], HEX);
                }
                Serial.println();
                if(buf[2] == 0){
                    digitalWrite(motorPin, HIGH);
                    Serial.println("On2");
                    delay(1000);
                }
                else if(buf[2] == 1){
                    Serial.println("Off");
                    water_off;
                    delay(1000);
                }
            }
        }
    }
}
