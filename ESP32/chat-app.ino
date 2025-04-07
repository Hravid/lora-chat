#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <SD.h>
#include <SPI.h>
#include <ESPmDNS.h>

Preferences preferences;

// Wi-Fi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Create a web server object on port 80
WebServer server(80);

// Message storage
String receivedMessage = "";  // Stores the last message received via POST
String responseMessage = "";  // Default message to send via GET

// SD card settings
const int chipSelect = 5;  // Common CS pin for ESP32, adjust if needed
const String LOG_FILE = "/message_log.txt";  // File to store messages

// Initialize SD card
bool initSD() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return false;
  }
  Serial.println("SD card initialization done.");
  return true;
}

// Save a message to SD card
void saveMessageToSD(String message) {
  File dataFile = SD.open(LOG_FILE, FILE_APPEND);
  
  if (dataFile) {
    // Add timestamp (you could implement actual time with an RTC module)
    String logEntry = String(millis()) + ": " + message + "\n";
    
    dataFile.print(logEntry);
    dataFile.close();
    Serial.println("Message saved to SD card");
  } else {
    Serial.println("Error opening log file");
  }
}

// Handle GET requests
void handleGet() {
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  server.send(200, "application/json", "{\"message\":\"" + responseMessage + "\"}");
}

// Handle POST requests
void handlePost() {
  // Add CORS headers
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  if (server.hasArg("plain")) {
    receivedMessage = server.arg("plain");
    Serial.println("Received message: " + receivedMessage);
    
    // Save the message to SD card
    saveMessageToSD(receivedMessage);
    
    server.send(200, "text/plain", "Message received and saved");
    responseMessage = receivedMessage;
  } else {
    server.send(400, "text/plain", "No message sent");
  }
}

// Handle OPTIONS preflight requests
void handleOptions() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
}

// Add a new endpoint to get all messages
void handleGetAllMessages() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  
  if (SD.exists(LOG_FILE)) {
    File dataFile = SD.open(LOG_FILE);
    String messages = "";
    
    if (dataFile) {
      while (dataFile.available()) {
        messages += (char)dataFile.read();
      }
      dataFile.close();
      
      server.send(200, "text/plain", messages);
    } else {
      server.send(500, "text/plain", "Error opening log file");
    }
  } else {
    server.send(404, "text/plain", "No message log found");
  }
}

void setup() {
  Serial.begin(9600);

  // Initialize SD card
  if (!initSD()) {
    Serial.println("Warning: Proceeding without SD card functionality");
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  if (MDNS.begin("esp32")) { // Hostname: esp32.local
    Serial.println("mDNS responder started");
  }
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());

  // Define routes
  server.on("/get-message", HTTP_GET, handleGet);   // Endpoint for GET requests
  server.on("/post-message", HTTP_POST, handlePost); // Endpoint for POST requests
  server.on("/get-message", HTTP_OPTIONS, handleOptions); // Handle preflight for GET
  server.on("/post-message", HTTP_OPTIONS, handleOptions); // Handle preflight for POST
  server.on("/get-all-messages", HTTP_GET, handleGetAllMessages); // New endpoint to retrieve all messages

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
  preferences.begin("storage", false); // Open namespace in read/write mode
  preferences.putInt("counter", 42);    // Save integer
  preferences.putString("ssid", "WiFi_Name"); // Save string
  preferences.end(); // Close namespace
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}