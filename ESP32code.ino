#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "//your laptop hotspot name";
const char* password = "//password";

// Motor pins
#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 33
#define ENB 32

// Servo pin
#define SERVO_PIN 13

WebServer server(80);
Servo penServo;

void handleRoot() {
  String html = "<html><body><h1>Drawing Robot Control</h1>";
  html += "<p>Use /gcode endpoint with POST requests</p>";
  html += "</body></html>";
  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS fix
  server.send(200, "text/html", html);
  Serial.println("Served root page");
}

void handleGCode() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    Serial.println("Rejected non-POST request");
    return;
  }

  String gcode = server.arg("gcode");
  Serial.println("Received G-code: " + gcode); // Log received G-code
  processGCode(gcode);

  server.sendHeader("Access-Control-Allow-Origin", "*"); // CORS fix
  server.send(200, "text/plain", "OK");
  Serial.println("Processed G-code successfully");
}

void processGCode(String gcode) {
  Serial.print("Processing command: ");
  Serial.println(gcode);

  if (gcode.startsWith("G1")) {
    Serial.println("Pen DOWN movement");
    penServo.write(0);
    delay(300);
    int x = getValue(gcode, 'X');
    int y = getValue(gcode, 'Y');
    moveRobot(x, y, 200);
  } 
  else if (gcode.startsWith("G0")) {
    Serial.println("Pen UP movement");
    int x = getValue(gcode, 'X');
    int y = getValue(gcode, 'Y');
    moveRobot(x, y, 255);
    penServo.write(90);
    delay(300);
  }
  else if (gcode.startsWith("M03")) {
    Serial.println("Pen DOWN command");
    penServo.write(0);
    delay(300);
  }
  else if (gcode.startsWith("M05")) {
    Serial.println("Pen UP command");
    penServo.write(90);
    delay(300);
  }
  else {
    Serial.println("Unknown command");
  }
}

int getValue(String data, char key) {
  int index = data.indexOf(key);
  if (index == -1) return 0;
  int start = index + 1;
  int end = data.indexOf(' ', start);
  if (end == -1) end = data.length();
  return data.substring(start, end).toInt();
}

void moveRobot(int x, int y, int speed) {
  Serial.printf("Moving to X:%d Y:%d Speed:%d\n", x, y, speed);

  // X-axis movement (left motors)
  if (x > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, speed);
  } 
  else if (x < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, speed);
  }
  else {
    analogWrite(ENA, 0);
  }

  // Y-axis movement (right motors)
  if (y > 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, speed);
  } 
  else if (y < 0) {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, speed);
  }
  else {
    analogWrite(ENB, 0);
  }

  delay(100); // Movement duration
  stopMotors();
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection

  // Motor setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Servo setup
  penServo.attach(SERVO_PIN);
  penServo.write(90); // Start with pen up

  // Connect to WiFi
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Server routes
  server.on("/", handleRoot);
  server.on("/gcode", HTTP_POST, handleGCode);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
