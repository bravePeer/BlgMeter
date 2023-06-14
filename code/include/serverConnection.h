#include "HTTPSRedirect.h"

class ServerConnetion
{
    HTTPSRedirect * client = nullptr;
    int httpsPort = 443;
    const String& host;
    const String& url;
public:
    ServerConnetion(const String& _host, const String& _url) 
        :host(_host), url(_url)
    { }

    inline bool connectToServer()
    {
        client = new  HTTPSRedirect(httpsPort);
        client->setInsecure();
        client->setPrintResponseBody(true);
        client->setContentTypeHeader("application/json");


        bool flag = false;
        for (int i = 0; i < 5; i++) 
        {
            int retval = client->connect(host, httpsPort);
            if (retval == 1) {
                flag = true;
                Serial.println("Connected to server");
                break;
            }
            else
                Serial.println("Connection failed. Retrying...");
        }

        if (!flag) 
        {
            Serial.print("Could not connect to server: ");
            Serial.println(host);
            Serial.println("Exiting...");
            return false;
        }

        return true;
   } 
   inline String send(String s)
   {
        Serial.println();
        Serial.print("url: ");
        Serial.println(url);
        Serial.print("host: ");
        Serial.println(host);
        // Serial.print("s: ");
        // Serial.println(s);
        Serial.println();

        client->POST(url, host.c_str(), s, false);

        Serial.print("status code: ");
        Serial.println(client->getStatusCode());

        return client->getResponseBody();
   }
   inline void disconnect()
   {
        delete client;
   }
 
};