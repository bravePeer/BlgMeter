#pragma once 
#include <Esp.h>
#include <ESP8266WiFi.h>

#define MAX_ACCELERATION_DATA 10

struct AccelerationData
{
    float ax;
    float ay;
    float az;
};

String toString(const AccelerationData& ad)
{
    return "ax = " + String(ad.ax) + "\tay = " + String(ad.ay) + "\taz = " + String(ad.az);
}

class RTCmenager 
{
    struct Data2save
    {
        uint8_t mode = 0;            //1
        uint8_t sleepTime = 0;        //1 How long ESP will wait (minutes)
        uint8_t numberOfWakeUp = 0;  //1 How many times ESP will wake up
        uint8_t add3 = 0;            //1
        uint32_t counter = 0;   //4
        AccelerationData accelerationData[MAX_ACCELERATION_DATA] = {0.};    //12 * 10
    };  //128 bytes
    static Data2save data2save;
public:
    static void readRTC()
    {
        system_rtc_mem_read(65, &data2save, sizeof(data2save));
    }

    static void writeRTC()
    {
        system_rtc_mem_write(65, &data2save, sizeof(data2save));
    }

    static void clean()
    {
        data2save = Data2save();
        // data2save.mode = 0;
        // data2save.sleepTime = 0;
        // data2save.numberOfWakeUp = 0;
        // data2save.add3 = 0;
        // data2save.counter = 0;
        // for (int i = 0; i < MAX_ACCELERATION_DATA; i++)
        // {
        //     data2save.accelerationData[i].ax = 0;
        //     data2save.accelerationData[i].ay = 0;
        //     data2save.accelerationData[i].az = 0;
        // }
    }

    static void serialPrintRTCMode()
    {
        Serial.print("mode: ");
        Serial.print(data2save.mode);
        Serial.print("\tcounter: ");
        Serial.println(data2save.counter);
    }

    static String rtcDataToHTML()
    {
        String s="";
        s += "mode: " + String(data2save.mode) + "  conunter:" + String(data2save.counter)+"<br>";
        for (int i = 0; i < MAX_ACCELERATION_DATA; i++)
        {
            s+=String(data2save.accelerationData[i].ax) + " "+String(data2save.accelerationData[i].ay) +" "+String(data2save.accelerationData[i].az)+"<br>";
        }
        return s;
    }

    static uint8_t getMode()
    {
        return data2save.mode;
    }

    static uint8_t getsleepTime()
    {
        return data2save.sleepTime;
    }

    static void setMode(uint8_t mode)
    {
        data2save.mode = mode;
    }

    static void setSleepConfig(uint8_t sleepTime, uint8_t numberOfWakeUp)
    {
        data2save.sleepTime = sleepTime;
        data2save.numberOfWakeUp = numberOfWakeUp;
    }

    static void addAccelerationData(AccelerationData accelerationData)
    {
        uint8_t index = data2save.counter % MAX_ACCELERATION_DATA;
        data2save.accelerationData[index] = accelerationData;
        data2save.counter++;
    }

};

RTCmenager::Data2save RTCmenager::data2save = RTCmenager::Data2save();