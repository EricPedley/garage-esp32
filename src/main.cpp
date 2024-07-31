//  Code bootstrapped from a snippet found here:
//  https://raw.githubusercontent.com/RuiSantosdotme/ESP32-Course/master/code/WiFi_Web_Server_Outputs/WiFi_Web_Server_Outputs.ino
#include <WiFi.h>
#include "wifiCredentials.h"
#include "blinker.hpp"

const char* htmlPage = R""""(
<!DOCTYPE HTML>
<head rel='icon' href='data:,'>
<html>
<body style='margin:0'>
    <button 
        style='background-color: grey;width: 100vw;height: 100vh;padding:0;border:0'
        onmousedown='fetch(`/ON`);document.querySelector(`button`).style.backgroundColor=`green`' 
        onmouseup='fetch(`/OFF`);document.querySelector(`button`).style.backgroundColor=`grey`'
    >
    </button>
</body>
</html>
)"""";

class WebServer {
public:
    WebServer() : server(80) {}
    void Init() {
        server.begin();
    }
    void tick() {
        WiFiClient client = server.available();   // Listen for incoming clients
        if(client) {
            handleConnection(client);
        }
    }
    bool getPinState() {
        return pinState;
    }
private:
    void sendHTMLResponse(WiFiClient client) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("");
        client.println(htmlPage);
        client.println("");
    }
    void sendPlainResponse(WiFiClient client, String message) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println();
        client.println(message);
        client.println();
    }
    void handleConnection(WiFiClient client) {
        connectionStartMS = millis();
        while(client.connected() && !(millis() - connectionStartMS > TIMEOUT_MS)) {
            if(client.available()) {
                // we only care about the GET /HTTP/1.1 line
                String request = readRequestFirstLine(client);
                client.flush();
                Serial.println(request);
                if(request.indexOf("/ON") != -1) {
                    pinState = HIGH;
                    sendPlainResponse(client, "ON");
                } else if(request.indexOf("/OFF") != -1) {
                    pinState = LOW;
                    sendPlainResponse(client, "OFF");
                } else {
                    sendHTMLResponse(client);
                }
                break;
            }
        }
        client.stop();
    }

    /**
     * Reads first line of the request. Just skips all the characters after the first \r\n
    */
    const char* readRequestFirstLine(WiFiClient client) {
        static char requestBuffer[512];
        int start = millis();
        int i = 0;
        while(client.connected() && millis() - start < TIMEOUT_MS) {
            if(client.available()) {
                char c = client.read();
                requestBuffer[i++] = c;
                if(c == '\n') {
                    break;
                }
            }
        }
        requestBuffer[i] = '\0';
        return requestBuffer;
    }
    WiFiServer server;
    bool pinState = LOW;
    int connectionStartMS = 0;
    const static int TIMEOUT_MS = 1000;
};

WebServer webServer;
Blinker blinker(LED_BUILTIN);
int gpioPin = GPIO_NUM_13;

void setup() {
    Serial.begin(115200);

    blinker.Init();

    pinMode(gpioPin, OUTPUT);

    digitalWrite(gpioPin, LOW);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    webServer.Init();
    digitalWrite(LED_BUILTIN, HIGH);
    blinker.setBlinkCount(3);
    blinker.setBlinkLength(100);
}

void loop() {
    webServer.tick();
    if(webServer.getPinState() == HIGH) {
        digitalWrite(gpioPin, HIGH);
        blinker.setBlinkPeriod(500);
    } else {
        digitalWrite(gpioPin, LOW);
        blinker.setBlinkPeriod(2000);
    }
    blinker.tick();
}