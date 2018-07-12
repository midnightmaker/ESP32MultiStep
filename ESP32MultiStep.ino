// The ESPAsynWebServer is found here:
// https://github.com/me-no-dev/ESPAsyncWebServer
// There is a dependency on this library
// https://github.com/me-no-dev/AsyncTCP


#include <dummy.h>
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

    pinMode(2, OUTPUT);      // set the LED pin mode

		pinMode(4, OUTPUT);      // 
		pinMode(5, OUTPUT);      //
		pinMode(18, OUTPUT);      //
		pinMode(15, OUTPUT);      // 
		pinMode(19, OUTPUT);      // 
  
 		digitalWrite(18,HIGH);
		digitalWrite(19,HIGH);
		digitalWrite(4,HIGH); // Global motor Enable


		attachInterrupt( digitalPinToInterrupt(5), countSteps, RISING);
    delay(10);

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
        
        Serial.println(p->name());
        Serial.print("=");
        Serial.println(p->value());
        
    }
		Serial.println("--- End of parameter list --- ");
     
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
   
  xStepper.blockingMove();
  yStepper.blockingMove();
  zStepper.blockingMove();
  

} 
