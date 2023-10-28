#include <ESP8266WiFi.h>// Import ESP8266 Wifi library
#include <PubSubClient.h> //Import PubSubClient library to initialize MQTT Protocol

// Update these with values suitable for your network.

const char* ssid = ".......";//use your ssid
const char* password = ".....";//use your password
const char* mqtt_server = "broker.mqtt-dashboard.com";
int ldrValue;
WiFiClient espClient;
PubSubClient client(espClient);// Define MQTT client
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Check for for Meaasege being received for deined topic topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D2, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(D2, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(" n"); // prints a space between the numbers
    delay(1000); // wait 100ms for next reading
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("LEDTEST", buf);
      // ... and resubscribe
      client.subscribe("LDRTEST");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(D2, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ldrValue = analogRead(A0); // read analog input pin A0
  Serial.print("LDRVALUE On Serial Port ="); // prints a space between the numbers
  Serial.print(ldrValue, DEC); // prints the value read
  Serial.print(" n"); // prints a space between the numbers
  sprintf(buf, "Published LDR Value=%d", ldrValue);
  client.publish("LDRTEST", buf);
  delay(1000); // wait 100ms for next reading
}
