/* Transmitter Code
*  Code to read Joystick position and transmit it with a RF24L01+ to a receiver 
*/

#include <SPI.h>
#include <RF24.h>

// Radio Configuration

RF24 radio(9,10); //sets CE and CSN pins to 9 & 10

byte addresses[][6] = {"LTa","LTb"}; //sets addresses for communication
//change these values to have unique communication! they can be anything

bool radioNumber=1;
bool role = 1;  //Control transmit 1/receive 0

//Analog joystick pins
int p1 = 0;       
int p2 = 1;      
int p3 = 2;  
int p4 = 3;

struct package{
   int leftX ;             // create struct for data package
   int leftY ;   
   int rightY ; 
   int fire ;   
} go;                  //initialize variable of struct type

void setup() {
    Serial.begin(9600);                   // Get ready to send data back for debugging purposes
    radio.begin();                        // Get the transmitter ready
    radio.setPALevel(RF24_PA_LOW);        // Set the power to low
    radio.openWritingPipe(addresses[1]);  // Where we send data out
    radio.openReadingPipe(1,addresses[0]); // Where we receive data back
    radio.setPayloadSize(32);  
//Sets payload size, however this is the default of the library. 
// May want to make value smaller if you are sending a small amount of data per packet
}

void loop() {
    //go.leftX = analogRead(0);//map(analogRead(0), 0, 1023, -255, 255);             // Read the joystick values, puts values into package variables
    //go.leftY = analogRead(1);//map(analogRead(1), 0, 1023, -255, 255);   
    //go.rightY = analogRead(2);//map(analogRead(2), 0, 1023, -255, 255); 
    //go.fire = digitalRead(2); 
    go.leftY = map(analogRead(p1), 0, 1023, -255, 255) * -1;             // Read the joystick values, puts values into package variables
    go.leftX = map(analogRead(p2), 0, 1023, 5, -5);   
    go.rightY = map(analogRead(p3), 0, 1023, -255, 255); 
    go.fire = abs(map(analogRead(p4), 0, 1023, -5, 5)); 

    Serial.print(go.leftY);
    Serial.print(go.rightY);
    radio.stopListening();                                 // Stop listening and begin transmitting
    delay(10);                                            // to stop jittering data

    if(radio.write(&go, sizeof(go)),Serial.println("SENT"));  // writes values to receiver, prints confirmation
 
    radio.startListening();   //listens for receiver's confirmation
}

