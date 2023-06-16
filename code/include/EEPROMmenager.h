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
    // enum class SIZES : int {
    //     SSID = 32,
    //     PASS = 64
    // };

    enum ADDRESSES : int {
        SSID = 0,  //size: 32 
        PASS = 32, //      64
        TMP = 96,  
        HOST = 96, //      64
        URL = 160, //     320
        NAME = 480,//      32
        END = 512 
    };


    //TODO change from string to const char*  
    static inline bool write(const String& data, const uint16& begin, const uint16_t& end)
    {
        if(data.length() > static_cast<unsigned int>(end - begin))
            return false;

        for (uint16 i = 0; i < data.length(); i++)
            EEPROM.put(begin + i, data[i]);

        for (uint16 i = begin + data.length(); i < end; i++)
            EEPROM.write(i, 0); 

        return true;
    }
    static const String read(const uint16& begin, const uint16& end)
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

    static void writeDeviceName(const String& name)
    {
        EEPROM.begin(EEPROM_SIZE);
        delay(10);
        
        write(name, NAME, END);

        EEPROM.commit();
        EEPROM.end();
    }
    static const String readDeviceName()
    {
        EEPROM.begin(EEPROM_SIZE);
        delay(10);

        String name = read(NAME, END);

        EEPROM.end();
        return name;
    }
};
