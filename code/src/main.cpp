#include <Esp.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <MMA7660.h>
#include <ESP8266TimerInterrupt.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <string>
#include <time.h>
#include "RTCmenager.h"
#include "sites/configureSite.h"
#include "EEPROMmenager.h"
#include "indicatorLed.h"
#include "serverConnection.h"
// #include "websiteHandles.h"
//#include <ESP8266_ISR_Timer.h>

#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

ESP8266Timer timer;
//ESP8266_ISR_Timer isrTimer;

#define MEASURMENTS 500
#define MAX_NUM_OF_CONNETION_ATTEMPTS 3

#define VOLTAGE_MEASURE_ADC A0
#define VOLTAGE_MEASURE_ENABLE_PIN 12




MMA7660 accelerometer;
ESP8266WebServer server(80);
// MyRTCData myData;


/*
Battery: ~2000mAh (<<<2400mAh)

00 -> 12 deep sleep
14:00 12:15
16
18
20
*/


enum MODE {
    CONFIGURE = 0b00000000,
    WORKING = 0b00000010,
    START = 0b00000001,
    OFFLINE = 0b00000000,
    ONLINE = 0b00010000
};

uint8_t mode = CONFIGURE | OFFLINE;
uint8_t sleepTime = 6;
inline bool ConnectToWiFi()
{
    NetworkData networkData = EEPROMMenager::readNetworkData();
    for(short i = 0; i < MAX_NUM_OF_CONNETION_ATTEMPTS; i++)
    {
        WiFi.begin(networkData.ssid, networkData.pass);
        Serial.print("Try connect to: ");
        Serial.print(networkData.ssid);
        // Serial.print(" with pass: ");
        // Serial.print(networkData.pass);
        
        for (int i = 0; i < 10; i++)
        {
            Serial.print(".");
            delay(1000);

            if(WiFi.status() == WL_CONNECTED)
                return true;
        }
        Serial.println();

        if(WiFi.status() == WL_CONNECTED)
            return true;
    }
    return false;
}

inline AccelerationData measureAcceleration()
{
    float ax,ay,az;
    float sumx = 0., sumy = 0., sumz = 0.;

    for(int i = 0; i < MEASURMENTS; i++)
    {
        accelerometer.getAcceleration(&ax, &ay, &az);
        sumx += ax;
        sumy += ay;
        sumz += az;
        delay(1);
    }

    sumx /= MEASURMENTS;
    sumy /= MEASURMENTS;
    sumz /= MEASURMENTS;
    
    return {sumx, sumy, sumz};
}

inline float calculateBLG(AccelerationData&accelerationData)
{
    return 1.;
}

//TODO adjust voltage reading
inline float calculateVoltage()
{
    digitalWrite(VOLTAGE_MEASURE_ENABLE_PIN, HIGH);
    delay(1);

    int adc = analogRead(VOLTAGE_MEASURE_ADC);
    Serial.println("ADC:" + String(adc));
    float voltage = (adc * 4.9) / 1024;
    
    digitalWrite(VOLTAGE_MEASURE_ENABLE_PIN, LOW);
    return voltage;
}

inline void goToSleep()
{
    Serial.println("Go to sleep");
    ESP.deepSleep(sleepTime * 3600 * 1e6);
    // ESP.deepSleep(10e6); //10s
    // ESP.deepSleep(1e6 * 3600); //1h
}

inline void work();

inline void setServerHandles()
{
    server.on("/", HTTP_GET, [](){
        Serial.println("Client connected");
        server.send_P(200,"text/html", configureSite);
        });

//------------Saving Configurations--------------
    server.on("/saveWiFiConfig", HTTP_GET, [](){
        String response = "saved";
        if(server.args() == 2)
        {
            if(!(server.argName(0) == "ssid" && server.argName(1) == "pass"))
                response = "Bad arguments sent";
            else
            {
                NetworkData netData;
                netData.ssid = server.arg(0);
                netData.pass = server.arg(1);
                EEPROMMenager::writeNetworkData(netData);
                response= "saved <br>ssid = " + server.arg(0)+"<br>pass = "+server.arg(1);
            }
        }
        
        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/readWiFiConfig", HTTP_GET, [](){
        NetworkData netData = EEPROMMenager::readNetworkData();
        String response = "ssid = "+netData.ssid;
        Serial.println("ssid = "+netData.ssid);

        server.send_P(200, "text/html", response.c_str());
    });

    server.on("/saveServerConfig", HTTP_GET, [](){
        String response = "not saved";
        if(server.args() == 2)
        {
            if(!(server.argName(0) == "host" && server.argName(1) == "url"))
                response = "Bad arguments sent";
            else
            {
                ServerConfig serverConfig;
                serverConfig.host = server.arg(0);
                serverConfig.url = server.arg(1);
                EEPROMMenager::writeServerConfig(serverConfig);
                response = "saved <br>host = " + serverConfig.host + "<br>url = " + serverConfig.url;
            }
        }
        
        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/readServerConfig", HTTP_GET, [](){
        ServerConfig serverConfig = EEPROMMenager::readServerConfig();
        String response = "host = " + serverConfig.host + "<br>url = " + serverConfig.url;
        Serial.println("host = " + serverConfig.host);
        Serial.println("url = " + serverConfig.url);

        server.send_P(200, "text/html", response.c_str());
    });

    server.on("/saveDeviceName", HTTP_GET, [](){
        String response = "Not saved";
        if(server.args() == 1)
        {
            String deviceName = server.arg(0);
            Serial.printf("Device name: %s\n", deviceName.c_str());
            EEPROMMenager::writeDeviceName(deviceName);
            response = "Saved";
        }

        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/readDeviceName", HTTP_GET, [](){
        String deviceName = EEPROMMenager::readDeviceName();
        String response = "Device name: " + deviceName;
        Serial.printf("Device name: %s\n", deviceName.c_str());

        server.send_P(200, "text/html", response.c_str());
    });
//-------------------Start----------------------
    server.on("/startWork", HTTP_GET, [](){
        if(server.args() == 1) //Number of wake up at day
        {
            sleepTime = server.arg(0).toInt();
            mode = MODE::START;
            Serial.println(sleepTime);
            server.send_P(200, "text/html", "Starting work");
            delay(10);
        }
            server.send_P(200, "text/html", "Err");
    });

//-------------------Testing----------------------
    server.on("/cleanRTC", HTTP_GET, [](){
        RTCmenager::clean();
    });
    server.on("/testWriteRTC", HTTP_GET, [](){
        RTCmenager::writeRTC();
        String response = "Wrote:<br>" + RTCmenager::rtcDataToHTML();
        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/testReadRTC", HTTP_GET, [](){
        RTCmenager::readRTC();
        String response = "Red:<br>" + RTCmenager::rtcDataToHTML();
        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/testCalcVoltage", HTTP_GET, [](){
        String response = "Voltage:" + String(calculateVoltage());
        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/testCalcBLG", HTTP_GET, [](){
        AccelerationData ad = measureAcceleration();
        float blg = calculateBLG(ad);
        
        String response = "BLG: " + String(blg) + "<br>" + toString(ad); 
        server.send_P(200, "text/html", response.c_str());
    });
    server.on("/testCalcAll", HTTP_GET, [](){
        String response = "Voltage:" + String(calculateVoltage())
                        + "<br>BLG: ";// + String(calculateBLG());
        server.send_P(200, "text/html",response.c_str());
    });
    server.on("/testServerSend", HTTP_GET, [](){
        //TODO change to test "serverConnection"

        if(WiFi.status() != WL_CONNECTED)
        {
            server.send_P(200, "text/html", "No Internet connection!");
            return;
        }

        ServerConfig serverConfig = EEPROMMenager::readServerConfig();
        ServerConnetion serverConnetion(serverConfig.host, serverConfig.url);
        if(!serverConnetion.connectToServer())
        {
            server.send_P(200, "text/html", "Error while connecting to server!");
            return;
        }

        AccelerationData aData = measureAcceleration();

        String s;
        DynamicJsonDocument json(1024);
        json["authCode"] = "bx1oPeUNQSjSIm5pwibr18naNvmG2grX";
        json["device"] = "esp8266";
        json["ax"] = aData.ax;
        json["ay"] = aData.ay;
        json["az"] = aData.az;
        json["blg"] = calculateBLG(aData);
        json["battery"] = calculateVoltage();


        serializeJson(json, s);
        String response = serverConnetion.send(s);
        deserializeJson(json, response);
        serverConnetion.disconnect();

        server.send_P(200, "text/html", String("Got:" + response).c_str());
    });
    server.on("/testSleeping", HTTP_GET, [](){
        String response;
        server.send_P(200, "text/html", response.c_str());
    });

    server.begin();
    Serial.println("Server started");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

inline void configure()
{
    IPAddress ip(192, 168, 1, 1);
    IPAddress gate(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(ip, gate, subnet);
    WiFi.softAP("BLGmeter", "password");
    
    setServerHandles();
}

inline void work()
{
    Serial.println("working");
    float voltage = calculateVoltage();

    AccelerationData accelerationData = measureAcceleration();
    float blg = calculateBLG(accelerationData);

    String deviceName = EEPROMMenager::readDeviceName();

    ServerConfig serverConfig = EEPROMMenager::readServerConfig();
    ServerConnetion serverConnetion(serverConfig.host, serverConfig.url);

    bool connectedToServer = serverConnetion.connectToServer();
    if(WiFi.status() != WL_CONNECTED || connectedToServer != true)
    {
        RTCmenager::addAccelerationData(accelerationData);
        // myData.accelerationData[myData.counter % MAX_ACCELERATION_DATA] = accelerationData;
        // myData.counter++;

        // writeRTC(myData);
        RTCmenager::writeRTC();
        serverConnetion.disconnect();
        Serial.println("No WiFi connection or no server connection");
        goToSleep();
        return;
    }
 
    String s;
    DynamicJsonDocument json(1024);
    json["authCode"] = "bx1oPeUNQSjSIm5pwibr18naNvmG2grX";
    json["device"] = deviceName;
    // json["ax"] = accelerationData.ax;
    // json["ay"] = accelerationData.ay;
    // json["az"] = accelerationData.az;
    // json["blg"] = blg;
    // json["battery"] = voltage;

    // Send old data
    // RTCmenager::rtcDataToHTML();
    // Serial.println("counter "+String( myData.counter));

/*
RTC acc data (3): 2     4
                  1     4
                  2     2
                  0     3
*/

    uint32_t counter = RTCmenager::getCounter();
    uint32_t startIndex = 0;
    if (counter / MAX_ACCELERATION_DATA > 0)
        startIndex = ((counter % MAX_ACCELERATION_DATA) + 1) > MAX_ACCELERATION_DATA ? 0 : (counter % MAX_ACCELERATION_DATA) + 1; //counter - MAX_ACCELERATION_DATA;
    
    for (uint32_t i = 0; i < MAX_ACCELERATION_DATA && i < counter; i++)
    {
        AccelerationData accelerationData = RTCmenager::getAccelerationData(i + startIndex);
        json["ax"] = accelerationData.ax;//myData.accelerationData[myData.counter % MAX_ACCELERATION_DATA].ax;
        json["ay"] = accelerationData.ay;//myData.accelerationData[myData.counter % MAX_ACCELERATION_DATA].ay;
        json["az"] = accelerationData.az;//myData.accelerationData[myData.counter % MAX_ACCELERATION_DATA].az;
        json["blg"] = blg;
        json["battery"] = voltage;

        s="";
        serializeJson(json, s);
        serverConnetion.send(s); //TODO add check if lost connection
        Serial.print("Send old data["+String(i)+"]:");
        Serial.println(s);
    }

    
    // new data
    json["ax"] = accelerationData.ax;
    json["ay"] = accelerationData.ay;
    json["az"] = accelerationData.az;
    json["blg"] = blg;
    json["battery"] = voltage;
    

    // String body = R"=====(
    //     {
    //         "authCode":"bx1oPeUNQSjSIm5pwibr18naNvmG2grX",
    //         "device":"esp8266",
    //         "ax":1,	
    //         "ay":0,
    //         "az":0,
    //         "blg":0,
    //         "battery":0
    //     }
    // )=====";
    
    serializeJson(json, s);
    String response = serverConnetion.send(s);
    deserializeJson(json, response);


    Serial.println("Sent:");
    Serial.println(s);

    Serial.println("Response:");
    Serial.println(response);

    Serial.print("status:");
    Serial.println(static_cast<String>(json["status"]));

    // Serial.print("Date and time:");
    // Serial.println(static_cast<String>(json["time"]));

    serverConnetion.disconnect();

    //Clean RTC memory
    if(connectedToServer == true)
    {
        RTCmenager::cleanAccelerationData();
        RTCmenager::writeRTC();        
    }
    
    mode = MODE::WORKING;
    RTCmenager::setMode(mode);
    RTCmenager::writeRTC();

    goToSleep();
}

//#define TEST_FUN_ONLY
// inline void test()
// {
// NetworkData networkData = readNetworkData();
//     for(short i = 0; i < MAX_NUM_OF_CONNETION_ATTEMPTS; i++)
//     {
//         WiFi.begin(networkData.ssid, networkData.pass);
//         Serial.print("Try connect to: ");
//         Serial.print(networkData.ssid);
//         for (int i = 0; i < 10; i++)
//         {
//             Serial.print(".");
//             delay(1000);
//             if(WiFi.status() == WL_CONNECTED)
//                 break;
//         }
//         if(WiFi.status() == WL_CONNECTED)
//             break;
//     }
//     String s;
//     DynamicJsonDocument json(1024);
//         json["authCode"] = "bx1oPeUNQSjSIm5pwibr18naNvmG2grX";
//         json["device"] = "esp8266";
//         json["ax"] = 1;
//         json["ay"] = 2;
//         json["az"] = 3;
//         json["blg"] = 1;
//         json["battery"] = calculateVoltage();
//     serializeJson(json, s);
//     Serial.println(s);
//    serverConnetion.connectToServer();
//    String response = serverConnetion.send(s);
//         //deserializeJson(json, response);
//         //Serial.println(response);
//         delay(2000);
//         //serverConnetion.disconnect();
// }


void setup()
{
    system_set_os_print(0);

    IndicatorLed::setPin(LED_BUILTIN);
    IndicatorLed::blinkCode(IndicatorLed::ledCodes::start);

    Serial.begin(115200);
    Serial.println("Serial started");

    accelerometer.init();
    Serial.println("Accelerometer initialized");

    pinMode(VOLTAGE_MEASURE_ENABLE_PIN, OUTPUT);
    digitalWrite(VOLTAGE_MEASURE_ENABLE_PIN, LOW);

    RTCmenager::readRTC();
    mode = RTCmenager::getMode();
    if(!(mode == MODE::WORKING))
        mode = MODE::CONFIGURE;

    if (ConnectToWiFi())
    {
        mode |= MODE::ONLINE;
        IndicatorLed::blinkCode(IndicatorLed::connectedToWiFi);
    }
    else
    {
        // mode |= MODE::OFFLINE;
        IndicatorLed::blinkCode(IndicatorLed::noConnectedToWiFi);
    }

    Serial.println(WiFi.localIP());
    Serial.printf("Mode: %d ", mode);

    if((mode & 0x03) ==  MODE::CONFIGURE)
    {
        configure();
        Serial.println("configure");
    }
    else if((mode & 0x03) == MODE::WORKING)
    { 
        work();
        Serial.println("working");
    }

    return;

    // secureClient.setInsecure();
    // if(https.begin(secureClient, host, 443, "/macros/s/AKfycbzFrZCrbK0yf9HH4DaTyBdeArdpXP3eOsXj_SetOq7hpc2xPywu3h6EPHKxs5Ku9p0bDA/exec"))
    // {
    //     https.setFollowRedirects(followRedirects_t::HTTPC_FORCE_FOLLOW_REDIRECTS);
    //     https.addHeader("Host", String(String(host) + ":" + 443));
    //     https.addHeader("Content-Type", "application/json");
    //     https.addHeader("User-Agent", "ESP8266");
    //     https.addHeader("Content-Length", String(body.length()));
    //     Serial.println("sending post body: "+ body);
    //     int httpsCode = https.POST(body);
    //     if (httpsCode > 0) {
    //         Serial.println(httpsCode);
    //         redirected = https.getLocation();
    //         Serial.println(redirected);
    //         Serial.println(https.getString());
    //     if (httpsCode == HTTP_CODE_OK) {
    //         Serial.println(https.getString());
    //     }
    //     } else {
    //         Serial.print("failed to POST");
    //     }
    // }
    // https.end();
    // String h2 = "https://script.googleusercontent.com";
    // if(https.begin(secureClient, h2, 443, redirected.substring(h2.length())))
    // {
    //     int code = https.GET();
    //     if(code > 0)
    //     {
    //         Serial.println(code);
    //     }
    // }
    // https.end();
    // timer.attachInterruptInterval(20*1000000, [](){
    //     //timer.detachInterrupt();
    //    // timer.disableTimer();
    //     Serial.println("interrupt");
    //     float ax,ay,az;
    //     accelerometer.getAcceleration(&ax, &ay, &az);
    //     Serial.print("ax = ");
    //     Serial.print(ax);
    //     Serial.print("g\t ay = ");
    //     Serial.print(ay);
    //     Serial.print("g\t az = ");
    //     Serial.print(az);
    //     Serial.println();
    // });
}

void loop()
{
#ifndef TEST_FUN_ONLY
    server.handleClient(); 

    if(mode == MODE::START)
    {
        Serial.println("Starting work");
        mode = MODE::WORKING;
        RTCmenager::clean();
        RTCmenager::setMode(mode);
        RTCmenager::setSleepConfig(sleepTime);
        RTCmenager::writeRTC();

        //Send firt measure
        String deviceName = EEPROMMenager::readDeviceName();
        
        ServerConfig serverConfig = EEPROMMenager::readServerConfig();
        ServerConnetion serverConnetion(serverConfig.host, serverConfig.url);
        if(!serverConnetion.connectToServer())
        {
            server.send_P(200, "text/html", "Error while connecting to server!");
            return;
        }

        AccelerationData aData = measureAcceleration();

        String s;
        DynamicJsonDocument json(1024);
        json["authCode"] = "bx1oPeUNQSjSIm5pwibr18naNvmG2grX";
        json["device"] = deviceName;
        json["ax"] = aData.ax;
        json["ay"] = aData.ay;
        json["az"] = aData.az;
        json["blg"] = calculateBLG(aData);
        json["battery"] = calculateVoltage();


        serializeJson(json, s);
        String response = serverConnetion.send(s);
        deserializeJson(json, response);
        serverConnetion.disconnect();

    //     Serial.print("Date and time:");
    //     String timeString = static_cast<String>(json["time"]); 
    //     Serial.println(timeString);
        
    //     //"time":"2023-06-15T16:24:18.338Z"
    // //2023 6 15 : 17 7 3
    //     tm ttd={.tm_sec = timeString.substring(17,19).toInt(),
    //         .tm_min = timeString.substring(14,16).toInt(),
    //         .tm_hour = timeString.substring(11,13).toInt(),
    //         .tm_mday = timeString.substring(8,10).toInt(),
    //         .tm_mon = timeString.substring(5,7).toInt(), 
    //         .tm_year = timeString.substring(0,4).toInt()};
    //     Serial.printf("%d %d %d : %d %d %d", ttd.tm_year, ttd.tm_mon, ttd.tm_mday, ttd.tm_hour, ttd.tm_min, ttd.tm_sec);
        // jest 11 -> tm_hour
        // 6 -> sleepTime
        //o 6 pomiar
        //o 12 pomiar
        // sleepTime
        // uint8_t sleep = sleepTime;
        // while (sleep < ttd.tm_hour)
        // {
        //     sleep+=sleepTime;
        // }
        
        goToSleep();
    }    
#endif
}