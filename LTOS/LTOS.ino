#include <SPI.h>
#include <RF24.h>
#include <Servo.h>

Servo turretServo;

//Radio Configuration

RF24 radio(7,10);
byte addresses[][6] = {"LTa1","LTb1"}; // make sure this matches the transmitter adresses

bool radioNumber=0;
bool role = 0;  //Control transmit/receive

int leftSensor;
int rightSensor;
int turretAngle = 90;
int health = 255;

struct package{
   int leftX ;             // create struct for data package
   int leftY ;   
   int rightY ; 
   int fire ;   
} go;        

void setup() { 
  Serial.begin(9600);        
  radio.begin();             // Initializes radio
  radio.setPALevel(RF24_PA_LOW);   // Sets power output to low.
  radio.openWritingPipe(addresses[0]); //opens writing pipeline
  radio.openReadingPipe(1,addresses[1]); //opens reading pipeline
  radio.setPayloadSize(32); //sets payload size, but this is default
  radio.startListening();

  pinMode(8,OUTPUT);       //Channel A Brake Pin Initialize
  pinMode(9,OUTPUT);       //Channel B Brake Pin Initialize 
  pinMode(4,OUTPUT);      //Channel A Direction Pin Initialize
  pinMode(2,OUTPUT);      //Channel B Direction Pin Initialize
  pinMode(3, OUTPUT);      //Channel A Speed Pin Initialize
  pinMode(6, OUTPUT);     //Channel B Speed Pin Initialize

  turretServo.attach(5);
}
void loop() {  
  delay(20);               //increase for debuggy, decrease to decrease jitter
  if(radio.available()){
    radio.read(&go,sizeof(go)); //reads value of go
  } else { 
    Serial.println("NO RADIO"); 
  }
/*
  Serial.print(go.leftY);    //prints all 4 values from joysticks
  Serial.print("\t");
  Serial.print(go.leftX);
  Serial.print("\t");
  Serial.print(go.rightY);
  Serial.print("\t");
  Serial.print(go.fire);
  Serial.print("\t");
  Serial.println(turretAngle);
  delay(20);
  */
  //LASER CODE
  if(go.fire > 1){
    analogWrite(A5, 255);
  }else{
    digitalWrite(A5, 0);
  }
  leftSensor = analogRead(A3);
  rightSensor = analogRead(A4);
  if(leftSensor >= rightSensor + 75 || rightSensor >= leftSensor + 75){
    digitalWrite(15, HIGH);
    Serial.println("LASER");
    analogWrite(A5, 255);
  }
  Serial.println(rightSensor);
  Serial.println(leftSensor);

  //MOTOR CODE
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  if(go.leftY >= 0){
    digitalWrite(4, LOW);
  }else{
    digitalWrite(4, HIGH);
  }
  if(go.rightY >= 0){
    digitalWrite(2, LOW);
  }else{
    digitalWrite(2, HIGH);
  }
  if(radio.available()){
    analogWrite(3, abs(go.leftY)/2);
    analogWrite(6, abs(go.rightY)/2);
  }else{

    
    analogWrite(3, 0);
    analogWrite(6, 0);
  }

  //SERVO CODE
  if(go.leftX > 2 && turretAngle < 180 && radio.available()){
    turretAngle += 2;
  }
  if(go.leftX < -2 && turretAngle > 0 && radio.available()){
    turretAngle -= 2;
  }
  if(radio.available()){
    turretServo.write(turretAngle);
  }else{
    turretServo.write(90);
    turretAngle = 90;
  }
}

