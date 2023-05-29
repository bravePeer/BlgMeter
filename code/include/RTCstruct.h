#pragma once 
#include <Esp.h>
#include <ESP8266WiFi.h>

#define MAX_ACCELERATION_DATA 10

struct AccelerationData{
    // uint32_t ax;
    // uint32_t ay;
    // uint32_t az;
    float ax;
    float ay;
    float az;
};

struct MyRTCData
{
    uint8_t mode;            //1
    uint8_t waitTime;        //1 How long ESP will wait (minutes)
    uint8_t numberOfWakeUp;  //1 How many times ESP will wake up
    uint8_t add3;            //1
    uint32_t counter;   //4
    AccelerationData accelerationData[MAX_ACCELERATION_DATA];    //12 * 10
};  //128 bytes

void serialPrintRTCMode(const MyRTCData& myData)
{
    Serial.print("mode: ");
    Serial.print(myData.mode);
    Serial.print("\tcounter: ");
    Serial.println(myData.counter);
}

String rtcDataToHTML(const MyRTCData& myData)
{
    String s="";
    s += "mode: " + String(myData.mode) + "  conunter:" + String(myData.counter)+"<br>";
    for (int i = 0; i < MAX_ACCELERATION_DATA; i++)
    {
        s+=String(myData.accelerationData[i].ax) + " "+String(myData.accelerationData[i].ay) +" "+String(myData.accelerationData[i].az)+"<br>";
    }
    return s;
}

String toString(const AccelerationData& ad)
{
    return "ax = " + String(ad.ax) + "\tay = " + String(ad.ay) + "\taz = " + String(ad.az);
}

void readRTC(MyRTCData& data)
{
    system_rtc_mem_read(65, &data, sizeof(data));
}

void writeRTC(const MyRTCData& data)
{
    system_rtc_mem_write(65, &data, sizeof(data));
}