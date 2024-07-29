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
        <h1>Remote button</h1>
        <p><button onclick='fetch(\"/ON\")'>ON</button></p>
        <p><button onclick='fetch(\"/OFF\")'>OFF</button></p>
        </html>
)"""";

class WebServer {
public:
    // void tick() {
    //     WiFiClient client = server.available();   // Listen for incoming clients
    //     if(client) {
    //         handleConnection(client);
    //     }
    // }
private:
    // void sendHTMLResponse(WiFiClient client) {
    //     client.println("HTTP/1.1 200 OK");
    //     client.println("Content-Type: text/html");
    //     client.println("");
    //     client.println("(╯°□°）╯︵ ┻━┻)");
    //     client.println("");
    // }
    // void sendPlainResponse(WiFiClient client, String message) {
    //     client.println("HTTP/1.1 200 OK");
    //     client.println("Content-Type: text/plain");
    //     client.println();
    //     client.println(message);
    //     client.println();
    // }
    // void handleConnection(WiFiClient client) {
    //     connectionStartMS = millis();
    //     while(client.connected() && !(millis() - connectionStartMS > TIMEOUT_MS)) {
    //         if(client.available()) {
    //             String request = readRequest(client);
    //             Serial.println(request);
    //             client.flush();
    //             if(request.indexOf("/ON") != -1) {
    //                 pinState = HIGH;
    //                 sendPlainResponse(client, "ON");
    //             } else if(request.indexOf("/OFF") != -1) {
    //                 pinState = LOW;
    //                 sendPlainResponse(client, "ON");
    //             } else {
    //                 sendHTMLResponse(client);
    //             }
    //             return;
    //         }
    //     }
    // }
    // String readRequest(WiFiClient client) {
    //     String request = "";
    //     char prevChar = 0;
    //     while(client.connected()) {
    //         if(client.available()) {
    //             char c = client.read();
    //             request += c;
    //             if(c == '\n' && prevChar == '\n') {
    //                 break;
    //             }
    //             prevChar = c;
    //         }
    //     }
    //     return request;
    // }
    bool pinState = LOW;
    int connectionStartMS = 0;
    // const static int TIMEOUT_MS = 1000;
};

WebServer webServer;

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);

    digitalWrite(LED_BUILTIN, LOW);

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
    digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
    // webServer.tick();
}