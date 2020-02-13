/*
   controll irrigation to use REST API
   http://LoRa gateway LG01-p ip/arduino/irrigation/(seconds time)
   for example)
   http://192.168.2.241/arduino/irrigation/1 (open water 1 seconds)
*/

#include <SPI.h>
#include <RH_RF95.h>

#define motorPin 7  //it can be changed if you want
#define water_on digitalWrite(motorPin, HIGH) //Turn on relay switch & motor
#define water_off digitalWrite(motorPin, LOW) //Turn off relay switch & motor

RH_RF95 rf95;
 
float frequency = 868.0;
unsigned long currentTime, setTime, startTime, endTime = 0;
bool controll;

void setup()
{
    Serial.begin(9600);
    if (!rf95.init())
        Serial.println("init failed");

    // Setup ISM frequency
    rf95.setFrequency(frequency);

    // Setup Power,dBm
    rf95.setTxPower(17);
    
    //rf95.setSyncWord(0x34); Not using
    
    Serial.println("LoRa End Node Example --"); 
    Serial.println("    Water Irrigation\n");
    
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
                  controll = false;
                }
                else{
                  controll = true;
                  water_on;
                  setTime = buf[2];
                  startTime = setTime*1000;
                  endTime = millis();
                }
                Serial.print("setTime : ");
                Serial.println(startTime);
                
            }
        }
    }
    
    Serial.print("CurrentTime");
    currentTime = millis() - endTime;
    Serial.println(currentTime);
    if(currentTime > startTime){
      Serial.println("stop water");
      setTime = -2;
      controll = false;
    }
    
    if(!controll){
      water_off;
      Serial.println("off");
      currentTime = 0;
    }
    currentTime += 2;
}
