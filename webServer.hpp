#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "definitions.hpp"

void web_task(void *pvParameters);

typedef struct WebStruct{

    const char* ssid = "xxx";  // Enter SSID here
    const char* password = "xxx";  //Enter Password here

} webStructure;



class MyWebServer {
    private:
        webStructure web;
        void refresh_page();
        void handle_not_found();
        String SendHTML(float valueNumber);
    public:
        WebServer server;
        float average;
        void config_webserver();
};
