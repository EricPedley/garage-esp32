// bootstrapped from https://RandomNerdTutorials.com/esp32-websocket-server-arduino/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wifiCredentials.h"
#include "blinker.hpp"


Blinker blinker(LED_BUILTIN);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
int gpioState = LOW;
int gpioPin = GPIO_NUM_13;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <link rel='icon' href='data:,'>
    <style>
        html, body {
            width: 100%;
            height: 100%;
            margin: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            background-color: lightgrey;
        }
        button {
            background-color: lightgrey;
            border: 2px solid black;
            font-size: 24px;
            border-radius: 8px;
            text-align: center;
            width: calc(100vw - 60px);
            height: calc(100% - 60px);
        }
    </style>
</head>
<body>
    <button>Tap anywhere to manipulate door</button>
    <script>
        var ws;
        window.addEventListener('load', ()=>{
            initWebSocket();
            initButton();
        });
        function initWebSocket() {
            console.log('Trying to open a WebSocket connection...');
            ws = new WebSocket(`ws://${window.location.hostname}/ws`);
            ws.onopen = onOpen;
            ws.onclose = onClose;
            ws.onmessage = onMessage;
        }
        function onOpen(event) {
            console.log('Connection opened');
        }
        function onClose(event) {
            console.log('Connection closed');
            setTimeout(initWebSocket, 2000);
        }
        function onMessage(event) {
            document.body.style.backgroundColor = event.data === '1' ? 'green' : 'lightgrey';
        }
        function initButton() {
            const button = document.querySelector('button')
            button.onmousedown = ()=>{
                button.style.backgroundColor = 'green';
                ws.send('on');
            }
            button.onmouseup = ()=>{
                button.style.backgroundColor = 'lightgrey';
                ws.send('off');
            }
        }
    </script>
</body>
</html>
)rawliteral";

void notifyClients() {
    ws.textAll(String(gpioState));
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char*)data, "toggle") == 0) {
            gpioState = !gpioState;
            notifyClients();
        }
        else if (strcmp((char*)data, "on") == 0) {
            gpioState = HIGH;
            notifyClients();
        }
        else if (strcmp((char*)data, "off") == 0) {
            gpioState = LOW;
            notifyClients();
        }
    }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
    void* arg, uint8_t* data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

String processor(const String& var) {
    Serial.println(var);
    if (var == "STATE") {
        if (gpioState) {
            return "ON";
        }
        else {
            return "OFF";
        }
    }
    return "";
}

void setup() {
    // Serial port for debugging purposes
    Serial.begin(115200);

    pinMode(gpioPin, OUTPUT);
    digitalWrite(gpioPin, LOW);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());

    initWebSocket();

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send_P(200, "text/html", index_html, processor);
        });

    // Start server
    server.begin();
    blinker.setBlinkPeriod(0);
}

void loop() {
    ws.cleanupClients();
    digitalWrite(gpioPin, gpioState);
}
