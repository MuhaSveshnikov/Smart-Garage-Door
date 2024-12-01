#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Wi-Fi Credentials
#define WIFI_SSID "MojaTV_Full_511570"
#define WIFI_PASSWORD "GFAQSMVIFWGFAQSMVIFW"

// Firebase Config and Auth
#define FIREBASE_HOST "garaza-3e795-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "vF3T2pbEd1RapyDnhVI9IoO9cbOge1buhRAeLAoH"  // Replace with your Firebase Database Secret
#define FIREBASE_URL "https://garaza-3e795-default-rtdb.europe-west1.firebasedatabase.app"

// Firebase Configuration
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Pin configuration
const int relayPin = 0; // Pin connected to the relay
const int ledPin = 2;   // Pin connected to the blue LED (GPIO2)

// Gate state: 0 = Closed, 1 = Open
int gateState = 0;  // Initially the door is closed

// Function prototypes
void toggleGate();

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as output

  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(relayPin, LOW);
  digitalWrite(ledPin, LOW); // Start with LED off

  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi...");
  
  // Wait for Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Once connected to Wi-Fi, turn on the LED
  Serial.println("\nWiFi connected.");
  digitalWrite(ledPin, HIGH);  // Turn on the blue LED to indicate Wi-Fi connection
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set Firebase config
  firebaseConfig.database_url = FIREBASE_URL;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;  // Use the Firebase Database secret (legacy token)

  // Initialize Firebase (pass a pointer to firebaseConfig and firebaseAuth)
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Serial.println("Connected to Firebase.");

  // Set the initial gate state in Firebase
  Firebase.setString(firebaseData, "/gateState", "closed"); // Initially closed
}

void loop() {
  // Get the current gate state from Firebase
  if (Firebase.getString(firebaseData, "/gateState")) {
    String gateCommand = firebaseData.stringData();
    Serial.print("Gate command received: ");
    Serial.println(gateCommand);

    if (gateCommand == "open" && gateState == 0) {
      // Open the gate
      toggleGate(); // Call the function to open the gate
      gateState = 1; // Update the gate state to 'open'
      Serial.println("Gate opened.");
    }
    else if (gateCommand == "closed" && gateState == 1) {
      // Close the gate
      toggleGate(); // Call the function to close the gate
      gateState = 0; // Update the gate state to 'closed'
      Serial.println("Gate closed.");
    }

    // Update Firebase with the new gate state
    if (Firebase.setString(firebaseData, "/gateState", gateState == 1 ? "open" : "closed")) {
      Serial.println("Gate state updated in Firebase.");
    } else {
      Serial.print("Failed to update gate state in Firebase: ");
      Serial.println(firebaseData.errorReason());
    }
  } else {
    Serial.print("Failed to read gate state: ");
    Serial.println(firebaseData.errorReason());
  }

  delay(1000);  // Check every 1 second for gate state updates
}

void toggleGate() {
  if (gateState == 0) {  // If gate is closed, open it
    digitalWrite(relayPin, HIGH);
    delay(750);  // Simulate the time for opening the gate
    digitalWrite(relayPin, LOW);
    gateState = 1;  // Set gate state to OPEN
    Serial.println("Gate opened.");
  } else {  // If gate is open, close it
    digitalWrite(relayPin, HIGH);
    delay(750);  // Simulate the time for closing the gate
    digitalWrite(relayPin, LOW);
    gateState = 0;  // Set gate state to CLOSED
    Serial.println("Gate closed.");
  }
}
