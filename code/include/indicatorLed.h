#include <Arduino.h>

class IndicatorLed
{
    static uint16 pinLed;
    static uint16 delaysTime;

public:
    enum ledCodes
    {
        start = 0b11010101,
        wakeUp,
        goSleep,

        connectedToWiFi = 0b01000111,

        //Errors
        noConnectedToWiFi = 0b10100111
        // send data ...
    };

    static void setPin(uint16 newPinLed)
    {
        pinLed = newPinLed;
        pinMode(pinLed, OUTPUT);
    }

    static void blinkCode(uint8 code)
    {
        while((code = code >> 1))
        {
            pinMode(pinLed, code & 0x01);
            delay(delaysTime);
        }   
        
        pinMode(pinLed, 0);
        // delay(delaysTime);
    }
};

uint16 IndicatorLed::pinLed = 0;
uint16 IndicatorLed::delaysTime = 200;