#include <ESP8266WiFi.h>

#define CMD_SIZE 5
const char* ssid = "TP-LINK_home";
const char* password = "vital_uta";
 
int ledPin = 2; // GPIO2
WiFiServer server(80);

char cmd[CMD_SIZE];
int cmd_write_index = 0;
int new_cmd_char = 0;

typedef struct
{
  char cmd_str[CMD_SIZE];
  void (*fn)(void);
}cmd_table_t;

void echo_my_ip(void)
{
  Serial.print("My IP: ");
  Serial.print(WiFi.localIP());
  Serial.print("\n");
}
void interpret_cmd()
{
    static cmd_table_t cmd_table [] = 
    {
      {"MYIP", echo_my_ip},
    };
    int cmd_table_size = sizeof(cmd_table)/sizeof(cmd_table[0]);
    int i = 0;
    for(i=0; i<cmd_table_size; i++)
    {
        if(0 == strncmp(cmd, cmd_table[i].cmd_str, CMD_SIZE))
        {
          (*(cmd_table[i].fn))();
        }
    }
}


void setup() {
  Serial.begin(115200);
  delay(10);
 
 
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
   
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
    
}
 
void loop() {
  if(0 < Serial.available())
  {
      new_cmd_char = Serial.read();
      if(-1 != new_cmd_char && CMD_SIZE > cmd_write_index)
      {
        cmd[cmd_write_index] = new_cmd_char;
        cmd_write_index++;
      }
      if(4 <= cmd_write_index)
      {
         cmd[CMD_SIZE] = '\0';
         interpret_cmd();
         cmd_write_index = 0;
         Serial.flush();
      }
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
   
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    //digitalWrite(ledPin, HIGH);
    analogWrite(ledPin, 1000);
    value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1){
    analogWrite(ledPin, 0);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
   
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
   
  client.print("Led pin is now: ");
   
  if(value == HIGH) {
    client.print("On");  
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED on pin 2 ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED on pin 2 OFF<br>");
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
 
