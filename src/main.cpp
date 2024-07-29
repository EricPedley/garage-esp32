/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
  https://raw.githubusercontent.com/RuiSantosdotme/ESP32-Course/master/code/WiFi_Web_Server_Outputs/WiFi_Web_Server_Outputs.ino
*********/

// Load Wi-Fi library
#include <WiFi.h>


// Replace with your network credentials
const char* ssid = "mywifi";
const char* password = "abcd1234";

const char* htmlPage = R""""(
<!DOCTYPE HTML>
<html>
<body style='margin:0'>
    <button 
        style='background-color: grey;width: 100vw;height: 100vh;padding:0'
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
        Serial.println("New client");
        connectionStartMS = millis();
        while(client.connected() && !(millis() - connectionStartMS > TIMEOUT_MS)) {
            if(client.available()) {
                Serial.println("Reading characters");
                String request = readRequest(client);
                Serial.println(request);
                client.flush();
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
    String readRequest(WiFiClient client) {
        String request = "";
        char buff[3];
        while(client.connected()) {
            if(client.available()) {
                char c = client.read();
                request += c;
                bool end = (buff[0] == '\r') && (buff[1] == '\n') && (buff[2] == '\r') && (c == '\n');
                if(end) {
                    break;
                }
                buff[0] = buff[1];
                buff[1] = buff[2];
                buff[2] = c;
            }
        }
        return request;
    }
    WiFiServer server;
    bool pinState = LOW;
    int connectionStartMS = 0;
    const static int TIMEOUT_MS = 1000;
};

WebServer webServer;

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(GPIO_NUM_5, OUTPUT);

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(GPIO_NUM_5, LOW);

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
}

void loop() {
    webServer.tick();
    digitalWrite(GPIO_NUM_5, webServer.getPinState());
}