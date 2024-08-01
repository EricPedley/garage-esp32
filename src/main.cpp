// bootstrapped from https://RandomNerdTutorials.com/esp32-websocket-server-arduino/

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "wifiCredentials.h"
#include "blinker.hpp"
#include "html/index.h"
#include "html/override.h"

Blinker blinker(LED_BUILTIN);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
bool outputPinSetActive = false;
bool manualControl = false;
bool manualPinSetActive = false;
int outputPin = GPIO_NUM_13;
int inputPin = GPIO_NUM_12;
int clients = 0;

class OpenCloseHandler {
public:
    enum State {
        OPENING,
        CLOSING,
        OPEN,
        CLOSED
    };
    void Init() {
        state = digitalRead(inputPin) == LOW ? CLOSED : OPEN;
    }

    void tick() {
        switch (state) {
            case OPENING:
                if (millis() - startedOpening > 5000) {
                    state = OPEN;
                    outputPinSetActive = false;
                    ws.textAll("opened");
                } else if (millis() - startedOpening > 500) {
                    outputPinSetActive = false;
                } else {
                    outputPinSetActive = true;
                }
                break;
            case CLOSING:
                if (digitalRead(inputPin) == LOW || millis() - startedClosing > 20000) {
                    state = CLOSED;
                    ws.textAll("closed");
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
            ws.textAll("open");
        } else {
            ws.textAll("Door is already open or opening");
        }
    }

    void close() {
        if(state == OPEN) {
            state = CLOSING;
            outputPinSetActive = true;
            startedClosing = millis();
            ws.textAll("close");
        } else {
            ws.textAll("Door is already closed or closing");
        }
    }

    State getState() {
        return state;
    }

private:
    State state;
    int startedOpening = 0;
    int startedClosing = 0;
};

OpenCloseHandler openCloseHandler;

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        if (strcmp((char*)data, "on") == 0) {
            manualPinSetActive = true;
            ws.textAll(String(manualPinSetActive));
        } else if (strcmp((char*)data, "off") == 0) {
            manualPinSetActive = false;
            ws.textAll(String(manualPinSetActive));
        } else if (strcmp((char*)data, "open") == 0) {
            openCloseHandler.open();
        } else if (strcmp((char*)data, "close") == 0) {
            openCloseHandler.close();
        }
    }
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
    void* arg, uint8_t* data, size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        if(openCloseHandler.getState() == OpenCloseHandler::OPEN) {
            client->text("opened");
        } else if(openCloseHandler.getState() == OpenCloseHandler::CLOSED) {
            client->text("closed");
        }
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
        if (manualPinSetActive) {
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
        manualControl = false;
        request->send_P(200, "text/html", index_html, processor);
        });
    server.on("/override", HTTP_GET, [](AsyncWebServerRequest* request) {
        manualControl = true;
        request->send_P(200, "text/html", override_html, processor);
        });

    // Start server
    server.begin();
    blinker.setBlinkPeriod(0);
    blinker.setBlinkLength(100);
}

void loop() {
    ws.cleanupClients();
    blinker.setBlinkCount(outputPinSetActive? 3 : 1);
    blinker.tick();
    openCloseHandler.tick();
    digitalWrite(outputPin, !(manualControl? manualPinSetActive : outputPinSetActive));
}
