// bootstrapped from https://RandomNerdTutorials.com/esp32-websocket-server-arduino/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wifiCredentials.h"
#include "blinker.hpp"
#include "indexHTML.h"

Blinker blinker(LED_BUILTIN);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
bool outputPinSetActive = false;
int outputPin = GPIO_NUM_13;
int inputPin = GPIO_NUM_12;
int clients = 0;

class OpenCloseHandler {
public:
    void Init() {
        state = digitalRead(inputPin) == LOW ? CLOSED : OPEN;
    }

    void tick() {
        switch (state) {
            case OPENING:
                if (millis() - startedOpening > 5000) {
                    state = OPEN;
                    outputPinSetActive = false;
                } else if (millis() - startedOpening > 500) {
                    outputPinSetActive = false;
                } else {
                    outputPinSetActive = true;
                }
                break;
            case CLOSING:
                if (digitalRead(inputPin) == LOW) {
                    state = CLOSED;
                } else {
                    outputPinSetActive = true;
                }
                break;
            default:
                outputPinSetActive = false;
        }
    }

    void open() {
        if (state == CLOSED) {
            state = OPENING;
            startedOpening = millis();
        } else {
            ws.textAll("Door is already open or opening");
        }
    }

    void close() {
        if(state == OPEN) {
            state = CLOSING;
            outputPinSetActive = true;
        } else {
            ws.textAll("Door is already closed or closing");
        }
    }

private:
    enum State {
        OPENING,
        CLOSING,
        OPEN,
        CLOSED
    } state;
    int startedOpening = 0;
};

OpenCloseHandler openCloseHandler;

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char*)data, "on") == 0) {
            outputPinSetActive = true;
            ws.textAll(String(outputPinSetActive));
        } else if (strcmp((char*)data, "off") == 0) {
            outputPinSetActive = false;
            ws.textAll(String(outputPinSetActive));
        } else if (strcmp((char*)data, "open") == 0) {
            openCloseHandler.open();
        } else if (strcmp((char*)data, "close") == 0) {
            openCloseHandler.close();
        }
        blinker.setBlinkCount(outputPinSetActive? 3 : 1);
    }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
    void* arg, uint8_t* data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        clients++;
        blinker.setBlinkPeriod(1000);
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        clients--;
        if(clients == 0) {
            blinker.setBlinkPeriod(0);
        }
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
        if (outputPinSetActive) {
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

    blinker.Init();

    pinMode(outputPin, OUTPUT);
    pinMode(inputPin, INPUT);
    digitalWrite(outputPin, HIGH);

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
    blinker.setBlinkLength(100);
}

void loop() {
    ws.cleanupClients();
    blinker.tick();
    openCloseHandler.tick();
    digitalWrite(outputPin, !outputPinSetActive);
}
