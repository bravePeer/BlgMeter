#pragma once
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#define EEPROM_SIZE 512

struct NetworkData
{
    String ssid = "";
    String pass = "";
};

struct ServerConfig
{
    String host = "";
    String url = "";
};

class EEPROMMenager
{
    enum ADDRESSES {
    SSID = 0,  //size: 32 
    PASS = 32, //      64
    TMP = 96,  
    HOST = 96, //      64
    URL = 160, //     320
    NAME = 480,//      32
    END = 512 
    };


    //TODO change from string to const char*  
    static inline void write(const String data, uint16 begin, uint16_t end)
    {
        for (uint16 i = 0; i < data.length(); i++)
            EEPROM.put(begin + i, data[i]);

        for (uint16 i = begin + data.length(); i < end; i++)
            EEPROM.write(i, 0);
        
    }
    static const String read(uint16 begin, uint16 end)
    {
        String data = "";
        for (uint16 address = begin; address < end; address++)
        {
            char readedByte = static_cast<char>(EEPROM.read(address));
            if(readedByte == 0)
                break;
            data += readedByte;
        }

        return data;
    }

public:
    static void writeNetworkData(NetworkData& networkData)
    {
        EEPROM.begin(EEPROM_SIZE);
        delay(10);

        write(networkData.ssid, SSID, PASS);
        write(networkData.pass, PASS, TMP);

        // for (unsigned int i = 0; i < networkData.ssid.length(); i++)
        //     EEPROM.put(ADDRESSES::SSID + i, networkData.ssid[i]);
        // for(unsigned int address = ADDRESSES::SSID+networkData.ssid.length(); address < ADDRESSES::PASS; address++)
        //     EEPROM.put(address, 0);
        // for (unsigned int i = 0; i < networkData.pass.length(); i++)
        //     EEPROM.put(ADDRESSES::PASS + i, networkData.pass[i]);    
        // for(unsigned int address = ADDRESSES::PASS + networkData.pass.length(); address < ADDRESSES::TMP; address++)
        //     EEPROM.put(address, 0);

        EEPROM.commit();
        EEPROM.end();
    }
    static NetworkData readNetworkData()
    {
        EEPROM.begin(EEPROM_SIZE);
        delay(10);
        NetworkData networkData;

        networkData.ssid = read(SSID, PASS);
        networkData.pass = read(PASS, TMP);

        // for (unsigned int address = ADDRESSES::SSID; address < ADDRESSES::PASS; address++)
        // {
        //     char readedByte = static_cast<char>(EEPROM.read(address));
        //     if(readedByte == 0)
        //         break;
        //     networkData.ssid += readedByte;
        // }
        // for (unsigned int address = ADDRESSES::PASS; address < ADDRESSES::TMP; address++)
        // { 
        // char readedByte = static_cast<char>(EEPROM.read(address));
        //     if(readedByte == 0)
        //         break;
        //     networkData.pass += readedByte;
        // }
        
        EEPROM.end();
        return networkData;
    }

    static void writeServerConfig(const ServerConfig& serverConfig)
    {
        EEPROM.begin(EEPROM_SIZE);
        delay(10);
    
        write(serverConfig.host, HOST, URL);
        write(serverConfig.url, URL, NAME);

        EEPROM.commit();
        EEPROM.end();
    }
    static ServerConfig readServerConfig()
    {
        EEPROM.begin(EEPROM_SIZE);
        delay(10);
        ServerConfig serverConfig;

        serverConfig.host = read(HOST, URL);
        serverConfig.url = read(URL, NAME);
       
        EEPROM.end();
        return serverConfig;
    }

//TODO add save in EEPROM device name
    static bool writeDeviceName();
    static const String readDeviceName()
    {
        return "ESP8266 - temporary name";
    }
};
