#include "webServer.hpp"

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

extern Variables*var;


void web_task(void *pvParameters){

	Serial.print("web_task running on core ");
	Serial.println(xPortGetCoreID());

    MyWebServer web_server;

	// -----  Web Server config  ---------
    web_server.config_webserver();

	for(;;){
		
		web_server.server.handleClient();

		web_server.average = var->counter ? var->average/var->counter : 0;

		vTaskDelay(pdMS_TO_TICKS(500)); //ticks para ms
	}
}


String MyWebServer::SendHTML(float valueNumber){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Equinos</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Equinos</h1>\n";
  ptr +="<h2>Media: ";
  ptr += valueNumber;
  ptr += "</h2>\n";
  ptr +="<h3>Vitau Automation</h3>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";

  Serial.print("value: ");
  Serial.println(valueNumber);

  return ptr;
}

void MyWebServer::handle_not_found(){
    server.send(404, "text/plain", "Not found");
}

void MyWebServer::refresh_page() {
    server.send(200, "text/html", SendHTML(average)); 
}
  
void MyWebServer::config_webserver() {
    WiFi.begin("2G_CLARO_304", "38879336");  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("\nConectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());  // print the IP address

	// WiFi.softAP(web.ssid, web.password);
	// WiFi.softAPConfig(local_ip, gateway, subnet);
	delay(100);
	server.on("/", std::bind(&MyWebServer::refresh_page, this));
	server.onNotFound(std::bind(&MyWebServer::handle_not_found, this));
	server.begin();
	Serial.println("HTTP server started");
}