#include <AccelStepper.h>
#include <MultiStepper.h>



/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */
// The ESPAsynWebServer is found here:
// https://github.com/me-no-dev/ESPAsyncWebServer
// 

#include <WiFi.h>
#include <FS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);


const char* ssid     = "dhomez";
const char* password = "Work@home1";

const char* webPage = "<!DOCTYPE HTML><html><head><meta name='viewport' content='width=device-width, initial-scale=1'></head><h1>ESP32 - Web Server</h1><p>LED #1 <a href='on1'><button>ON</button></a>&nbsp;<a href='off1'><button>OFF</button></a></p>"
                       "<p>LED #2 <a href='on2'><button>ON</button></a>&nbsp;<a href='off2'><button>OFF</button></a></p>"
                       "<p>Motor 1 <a href='forward?motor=M1'><button>Forward</button></a>&nbsp;<a href='backward?motor=M1'><button>Backward</button></a></p>"
                       "<p>Motor 2 <a href='forward?motor=M2'><button>Forward</button></a>&nbsp;<a href='backward?motor=M2'><button>Backward</button></a></p>"
                       "<p>Motor 3 <a href='forward?motor=M3'><button>Forward</button></a>&nbsp;<a href='backward?motor=M3'><button>Backward</button></a></p>"
                       "</html>";

// AccelStepper mystepper(1, pinStep, pinDirection);

//AccelStepper doorStepper(1, 5, 4);

class BlockingStepper{

  int stepPin = 1;
  int dirPin = 1;
  int motorPosition = 0;
  int reqPosition = 0;
  int stepsToMove = 0;
  
  public:

    BlockingStepper( int stepPin, int directionPin )
    {
      this->stepPin = stepPin;
      dirPin = directionPin;

      pinMode(dirPin, OUTPUT);
      pinMode(stepPin, OUTPUT);
      
    }

    void setPosition( int newPos )
    {
      reqPosition = newPos;
      stepsToMove = newPos - motorPosition;
      Serial.print("Steps to move ");
      Serial.println( stepsToMove );
      Serial.print("Current position : ");
      Serial.println( motorPosition );
    }

    void blockingMove()
    {
      if( stepsToMove < 0 )
      {
        digitalWrite( dirPin, LOW );
      }
      else
      {
        digitalWrite( dirPin, HIGH );
      }

      stepsToMove = abs(stepsToMove);
      //Serial.print("Steps to move ");
      //erial.println( stepsToMove );
      for( int i=0; i< stepsToMove; i++ )
      {
        digitalWrite( stepPin, HIGH );
        delayMicroseconds(750);
        digitalWrite( stepPin, LOW );
        delayMicroseconds(750);
      }
      stepsToMove = 0;
      motorPosition = reqPosition;
    }
    
    void blockingMove( int destinationPos )
    {
      setPosition( destinationPos );
      blockingMove();
    }
  
};

BlockingStepper xStepper( 5, 18 );
BlockingStepper yStepper( 22, 21 );
BlockingStepper zStepper( 15, 23 );

volatile int stepCounterGlob = 0;
void countSteps(void)
{
  
  stepCounterGlob++;
  
}

void setup()
{
    Serial.begin(115200);

//	doorStepper.setMaxSpeed(600);
//	doorStepper.setAcceleration(4000);


    
    pinMode(2, OUTPUT);      // set the LED pin mode

  pinMode(4, OUTPUT);      // 
  pinMode(5, OUTPUT);      //
  pinMode(18, OUTPUT);      //
  pinMode(15, OUTPUT);      // 
  pinMode(19, OUTPUT);      // 
  
 	digitalWrite(18,HIGH);
	digitalWrite(19,HIGH);
  digitalWrite(4,HIGH); // Global motor Enable
//attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);

  attachInterrupt( digitalPinToInterrupt(5), countSteps, RISING);
    delay(10);

    // We start by connecting to a WiFi network

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
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    /*server.on("/hello", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello from ESP32 server route");
  });
 
  server.on("/redirect/internal", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("/hello");
  });
 
  server.on("/redirect/external", HTTP_GET, [](AsyncWebServerRequest *request){
    request->redirect("https://techtutorialsx.com/");
  });
 */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", webPage);
  });
  
    server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request){
 
    int paramsNr = request->params();
    Serial.println(paramsNr);
 
    for(int i=0;i<paramsNr;i++){
         
        AsyncWebParameter* p = request->getParam(i);
        Serial.print("Param name: ");
        Serial.println(p->name());
        Serial.print("Param value: ");
        Serial.println(p->value());
        Serial.println("------");
    }
     
    request->send(200, "text/html", webPage);
  });
  
  server.begin();

}

int value = 0;
int moveDir = 0;
int ledstate = 0;
void loop(){

   
 //WiFiClient client = server.available();   // listen for incoming clients

  ledstate = !ledstate;
  
   if( ledstate == 1 )
   {
      digitalWrite(2, HIGH);
      //digitalWrite(15, HIGH);
      
   }
   else
   {
      digitalWrite(2, LOW);
      //digitalWrite(15, LOW);
   }
   delay(50);
    /*
       
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
          	client.print( webPage );
          	/*
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /on1")) {
          digitalWrite(2, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /off1")) {
          digitalWrite(2, LOW);                // GET /L turns the LED off
        }

        //Serial.print("Current Line=");
        //Serial.println(currentLine);
        if (currentLine.indexOf("forward") > -1 ) {
          if( currentLine.indexOf("M1") > -1 )
          {
            moveDir = 1;
        	  Serial.write("move motor forward M1");
          }
          else if( currentLine.indexOf("M2") > -1 )
          {
            moveDir = 2;
            Serial.write("move motor forward M2");
          }
          else if( currentLine.indexOf("M3") > -1 )
          {
            moveDir = 3;
            Serial.write("move motor forward M3");
            
          }
        }
        if (currentLine.indexOf("backward") > -1 ) {
          if( currentLine.indexOf("M1") > -1 )
          {
        	  Serial.write("move motor backward M1");
            moveDir = 6;
          }
          else if( currentLine.indexOf("M2") > -1 )
          {
            Serial.write("move motor backward M2");
            moveDir = 7;
          }
          else if( currentLine.indexOf("M3") > -1 )
          {
            Serial.write("move motor backward M3");
            moveDir = 8;
          }
                    //doorStepper.move(-1000);
          //doorStepper.setPosition(-1000);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
    if( moveDir == 1 ){
      xStepper.setPosition(3100);
     Serial.print("Glob Count = ");
     Serial.println(stepCounterGlob);
     
    }
    else if( moveDir == 2 )  
      yStepper.setPosition(3100);
    else if( moveDir == 3 )
      zStepper.setPosition(3100);
    else if( moveDir == 6 )
      xStepper.setPosition(0);  
    else if( moveDir == 7 )
      yStepper.setPosition(0);  
    else if( moveDir == 8 )
      zStepper.setPosition(0);  
  }

  */
  
  xStepper.blockingMove();
  yStepper.blockingMove();
  zStepper.blockingMove();
  

} 
