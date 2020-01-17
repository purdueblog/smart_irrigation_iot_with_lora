

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
#include <DHT.h>
#include <SPI.h>
#include <RH_RF95.h>
 
#define dht_tnh A0 // Use A0 pin as Data pin for DHT11. 
#define dht_sm A1 // 

RH_RF95 rf95;
DHT dht (dht_tnh, DHT22);

byte bGlobalErr;
char dht_dat[7]; // Store Sensor Data
char node_id[3] = {1,1,1}; //LoRa End Node ID 
 
float frequency = 868.0;
unsigned int count = 1;
 
void setup()
{
    InitDHT();
    dht.begin();
    Serial.begin(9600);
    if (!rf95.init())
        Serial.println("init failed");
        
    // Setup ISM frequency
    rf95.setFrequency(frequency);
    
    // Setup Power,dBm
    rf95.setTxPower(17);
    
    //rf95.setSyncWord(0x34); Not using
    
    Serial.println("LoRa End Node Example --"); 
    Serial.println("    DHT11 Temperature and Humidity Sensor\n");
    Serial.print("LoRa End Node ID: ");
 
    for(int i = 0;i < 3; i++)
    {
        Serial.print(node_id[i],HEX);
    }
    Serial.println();
}
 
void InitDHT()
{   //Arduino pin (DHT is A0, Soil sensor is A1. It can be changed for user's decide.)
    pinMode(dht_tnh,OUTPUT);//Set A0 to output
    digitalWrite(dht_tnh,HIGH);//Pull high A0
//    pinMode(A1,OUTPUT);//Set A1 to output
//    digitalWrite(A1,HIGH);//Pull high A1
}
 
//Get Sensor Data
void ReadDHT()
{
  
    bGlobalErr=0;
//    byte dht_in;
//    byte i;
//        
//    //pinMode(dht_tnh,OUTPUT);
//    digitalWrite(dht_tnh,LOW);//Pull Low A0 and send signal
//    delay(30);//Delay > 18ms so DHT11 can get the start signal
//        
//    digitalWrite(dht_tnh,HIGH);
//    delayMicroseconds(40);//Check the high level time to see if the data is 0 or 1
//    pinMode(dht_tnh,INPUT);
//    // delayMicroseconds(40);
//    
//    dht_in=digitalRead(dht_tnh);//Get A0 Status
//    //   Serial.println(dht_in,DEC);
//    if(dht_in){
//        bGlobalErr=1;
//        return;
//    }
//    //DHT11 send response, pull low A0 80us
//    delayMicroseconds(80);
//    dht_in=digitalRead(dht_tnh);
//    
//    if(!dht_in){
//        bGlobalErr=2;
//        return;
//    }
//    //DHT11 send response, pull low A0 80us
//    delayMicroseconds(80);
//    
//    for (i=0; i<5; i++)//Get sensor data
//    dht_dat[i] = read_dht_dat();
// 
//    //Read soil humidity data 
//    dht_dat[5]= digitalRead(A1); 
//    
//    pinMode(dht_tnh,OUTPUT);
//    digitalWrite(dht_tnh,HIGH);//release signal and wait for next signal
//    pinMode(A1,OUTPUT);
//    digitalWrite(A1,HIGH);

    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int soil_moisture = analogRead(dht_sm);

    dht_dat[0] = (int)(humidity); // humidity
    dht_dat[1] = (unsigned int)(humidity*100) % 100;
    dht_dat[2] = (int)(temperature); // temperature
    dht_dat[3] = (unsigned int)(temperature * 100) % 100;
    dht_dat[5] = (int) soil_moisture / 100; // soil moisture
    dht_dat[6] = (int) soil_moisture % 100;
    
    //For DHT11.
//    byte dht_check_sum = dht_dat[0]+dht_dat[1]+dht_dat[2]+dht_dat[3];//calculate check sum
//    Serial.print("dht_dat[4], dht_check_sum : ");
//    Serial.print(dht_dat[4]);
//    Serial.print(", ");
//    Serial.println(dht_check_sum);
//    if(dht_dat[4]!= dht_check_sum)//check sum mismatch
//        {bGlobalErr=3;}
 
};
 
 
 
 
byte read_dht_dat(){
    byte i = 0;
    byte result=0;
    for(i=0; i< 8; i++)
    {
        while(digitalRead(dht_tnh)==LOW);//wait 50us
        delayMicroseconds(30);//Check the high level time to see if the data is 0 or 1
        if (digitalRead(dht_tnh)==HIGH)
        result |=(1<<(7-i));//
        while (digitalRead(dht_tnh)==HIGH);//Get High, Wait for next data sampleing. 
    }
    return result;
}
 
//Do not change
uint16_t calcByte(uint16_t crc, uint8_t b)
{
    uint32_t i;
    crc = crc ^ (uint32_t)b << 8;
    
    for ( i = 0; i < 8; i++)
    {
        if ((crc & 0x8000) == 0x8000)
            crc = crc << 1 ^ 0x1021;
        else
            crc = crc << 1;
    }
    return crc & 0xffff;
}
//Do not change.
uint16_t CRC16(uint8_t *pBuffer,uint32_t length)
{
    uint16_t wCRC16=0;
    uint32_t i;
    if (( pBuffer==0 )||( length==0 ))
    {
      return 0;
    }
    for ( i = 0; i < length; i++)
    { 
      wCRC16 = calcByte(wCRC16, pBuffer[i]);
    }
    return wCRC16;
}
 
void loop()
{
    Serial.print("###########    ");
    Serial.print("COUNT=");
    Serial.print(count);
    Serial.println("    ###########");
    count++;

    //Get sensor data
    ReadDHT();
 
    //Make array for send data.
    char data[50] = {0} ;
    int dataLength = 9; // Payload Length
    
    // Use data[0], data[1],data[2] as Node ID
    data[0] = node_id[0] ;
    data[1] = node_id[1] ;
    data[2] = node_id[2] ;
    
    data[3] = dht_dat[0];//Get Humidity Integer Part
    data[4] = dht_dat[1];//Get Humidity Decimal Part
    data[5] = dht_dat[2];//Get Temperature Integer Part
    data[6] = dht_dat[3];//Get Temperature Decimal Part
    data[7] = dht_dat[5];//Get soil adove 2digit
    data[8] = dht_dat[6];//Get soil under 2digit 

    //Check error
    switch (bGlobalErr)
    {
      case 0:
          Serial.print("Current humidity = ");
          Serial.print(data[3], DEC);//Show humidity
          Serial.print(".");
          Serial.print(data[4], DEC);//Show humidity
          Serial.print("%  ");
          Serial.print("temperature = ");
          Serial.print(data[5], DEC);//Show temperature
          Serial.print(".");
          Serial.print(data[6], DEC);//Show temperature
          Serial.print("C  ");
          Serial.print("soil moisture = ");
          Serial.println(((int)data[7]*100 + (int)data[8]), DEC);//Show soil moisture
          break;
//       case 1:
//          Serial.println("Error 1: DHT start condition 1 not met.");
//          break;
//       case 2:
//          Serial.println("Error 2: DHT start condition 2 not met.");
//          break;
//       case 3:
//          Serial.println("Error 3: DHT checksum error.");
//          break;
       default:
          Serial.println("Error: Unrecognized code encountered.");
          break;
    }
    
    //get CRC DATA
    uint16_t crcData = CRC16((unsigned char*)data,dataLength);
    
    //Serial.println(crcData,HEX);
    Serial.print("Data to be sent(without CRC): ");
    
    int i;
    for(i = 0;i < dataLength; i++)
    {
        Serial.print(data[i],HEX);
        Serial.print(" ");
    }
    Serial.println();
        
    unsigned char sendBuf[50]={0};
 
    for(i = 0;i < dataLength;i++)
    {
        sendBuf[i] = data[i] ;
    }
    
    sendBuf[dataLength] = (unsigned char)crcData; // Add CRC to LoRa Data
    sendBuf[dataLength+1] = (unsigned char)(crcData>>8); // Add CRC to LoRa Data
 
    Serial.print("Data to be sent(with CRC):    ");
    for(i = 0;i < (dataLength + 2); i++)
    {
        Serial.print(sendBuf[i],HEX);
        Serial.print(" ");
    }
    Serial.println();
    
    //Send LoRa Data
    rf95.send(sendBuf, dataLength+2);
     
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];//Reply data array
    uint8_t len = sizeof(buf);//reply data length
 
    if (rf95.waitAvailableTimeout(3000))// Check If there is reply in 3 seconds.
    {
        // Should be a reply message for us now   
        if (rf95.recv(buf, &len))//check if reply message is correct
       {
            if(buf[0] == node_id[0] && buf[1] == node_id[2] && buf[2] == node_id[2] ) // Check if reply message has the our node ID
           {
               pinMode(4, OUTPUT);
               digitalWrite(4, HIGH);
               Serial.print("Got Reply from Gateway: ");//print reply
               Serial.println((char*)buf);
              
               delay(400);
               digitalWrite(4, LOW); 
               Serial.print("RSSI: ");  // print RSSI
               Serial.println(rf95.lastRssi(), DEC);        
           }
        }
        else
        { //When receive is failed. (From gateway | server)
           Serial.println("recv failed");//
           rf95.send(sendBuf, strlen((char*)sendBuf));//resend if no reply
        }
    }
    else
    {
        Serial.println("No reply, is LoRa gateway running?");//No signal reply
        rf95.send(sendBuf, strlen((char*)sendBuf));//resend data
    }
    delay(10000); // Send sensor data every 30 seconds
    Serial.println("");
}
