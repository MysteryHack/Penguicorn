#include <SPI.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>
#include "SH1106.h"
#include "RF24.h"


#define SDA_PIN 4
#define SCL_PIN 5
/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(16,15);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 0;

int frame, wheelIt, pushTime, ledMode;
float brightness;
boolean increment;
unsigned long t0;
//Button* b;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, 0, NEO_GRB + NEO_KHZ800);
SH1106 display(0x3c, SDA_PIN, SCL_PIN);


void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_LOW);
  
  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
  
  // Start the radio listening for data
  radio.startListening();
  display.init();
  display.setContrast(255);
  display.flipScreenVertically();
  display.clear();
  display.drawString(26, 25, "PenguiOS");
  display.drawString(26, 35, "NRF24tesst");
  display.display();

  strip.begin();
  strip.show();
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  strip.setPixelColor(1, strip.Color(255, 0, 0));
  strip.show();
  delay(1000);
}

void loop() {
  
  
/****************** Ping Out Role ***************************/  
if (role == 1)  {
    
    radio.stopListening();                                    // First, stop listening so we can talk.
    
    
    Serial.println(F("Now sending"));

    unsigned long start_time = micros();                             // Take the time, and send it.  This will block until complete
     if (!radio.write( &start_time, sizeof(unsigned long) )){
       Serial.println(F("failed"));
     }
        
    radio.startListening();                                    // Now, continue listening
    
    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
    while ( ! radio.available() ){                             // While nothing is received
      if (micros() - started_waiting_at > 200000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
          timeout = true;
          break;
      }      
    }
        
    if ( timeout ){                                             // Describe the results
        Serial.println(F("Failed, response timed out."));
        display.clear();
        display.drawString(26, 15, "timeout");
        display.display();
        display.display();
        strip.setPixelColor(0, strip.Color(255, 0, 0));
        strip.setPixelColor(1, strip.Color(255, 0, 0));
        strip.show();
    }else{
        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_time, sizeof(unsigned long) );
        unsigned long end_time = micros();
        
        // Spew it
        Serial.print(F("Sent "));
        Serial.print(start_time);
        Serial.print(F(", Got response "));
        Serial.print(got_time);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time-start_time);
        Serial.println(F(" microseconds"));
        display.clear();
        display.drawString(26, 15, "transmit");
        display.drawString(26, 25, String(start_time));
        display.drawString(26, 35, String(got_time));
        display.drawString(26, 45, String(end_time-start_time));
        display.display();
        display.display();
        strip.setPixelColor(0, strip.Color(0, 0, 255));
        strip.setPixelColor(1, strip.Color(0, 0, 255));
        strip.show();
    }

    // Try again 1s later
    delay(1000);
  }



/****************** Pong Back Role ***************************/

  if ( role == 0 )
  {
    unsigned long got_time;
    
    if( radio.available()){
                                                                    // Variable for the received timestamp
      while (radio.available()) {                                   // While there is data ready
        radio.read( &got_time, sizeof(unsigned long) );             // Get the payload
      }
     
      radio.stopListening();                                        // First, stop listening so we can talk   
      radio.write( &got_time, sizeof(unsigned long) );              // Send the final one back.      
      radio.startListening();                                       // Now, resume listening so we catch the next packets.     
      Serial.print(F("Sent response "));
      Serial.println(got_time);  
   }
 }




/****************** Change Roles via Serial Commands ***************************/

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == 0 ){      
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      role = 1;                  // Become the primary transmitter (ping out)
      display.clear();
      display.drawString(26, 15, "transmit");
      display.display();
      strip.setPixelColor(0, strip.Color(0, 0, 255));
      strip.setPixelColor(1, strip.Color(0, 0, 255));
      strip.show();
   }else
    if ( c == 'R' && role == 1 ){
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));      
       role = 0;                // Become the primary receiver (pong back)
       radio.startListening();
       display.clear();
       display.drawString(26, 15, "Receive");
       display.display();
       strip.setPixelColor(0, strip.Color(0, 255, 0));
       strip.setPixelColor(1, strip.Color(0, 255, 0));
       strip.show();
    }
  }


} // Loop

