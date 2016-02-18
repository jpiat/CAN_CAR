


#include <Metro.h>
#include <FlexCAN.h>
#include <Servo.h> 
#include <util/atomic.h>


Servo myservo;

unsigned char len = 0;
unsigned char buf[8];

unsigned char detached = 1 ;


#define EN_A_PIN 4
#define EN_B_PIN 5
#define PWM_PIN 6
#define SERVO_PIN 3

#define V_TO_A 0.13

#define ENCODER_PIN 3


int counter = 0;
unsigned long int current_time = 0 , old_time = 0  ;
int diff_time ;
float frequency ;
void handleEncoderInterrupt(){
  current_time = micros();
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    diff_time = current_time - old_time ;
    old_time = current_time ;
    if(diff_time < 0) diff_time = 4294967296UL - diff_time ; //handling overflow, this should not occur very often ...
  }  
  //}
}


//FOR VNH2SP25 motor driver
void setMotorSpeed(float motor_speed){
  
  if(motor_speed < 0.005  && motor_speed > -0.005){ //brake
      digitalWrite(EN_A_PIN, LOW);
      digitalWrite(EN_B_PIN, LOW);
  }else if(motor_speed > 0){ //foward
      digitalWrite(EN_A_PIN, HIGH);
      digitalWrite(EN_B_PIN, LOW);
   }else{//backward
     digitalWrite(EN_A_PIN, LOW);
     digitalWrite(EN_B_PIN, HIGH);
   } 
   motor_speed = motor_speed > 0 ? motor_speed * 255 : (-motor_speed * 255) ;
   analogWrite(PWM_PIN, motor_speed);
}

#define MOTOR_CMD_CAN_MASK  0x3FF

//RECEIVED COMMAND
#define MOTOR_CMD_CAN_SPEED_ID 0x000
#define MOTOR_CMD_CAN_STEER_ID 0x001



//PUBLISHED DATA
#define MOTOR_DATA_CAN_CURRENT_ID 0x002
#define MOTOR_DATA_CAN_ODO_ID 0x003


FlexCAN CANbus(500000);

CAN_filter_t motor_speed_filter = {0, 0, MOTOR_CMD_CAN_SPEED_ID};
CAN_filter_t steer_filter = {0, 0, MOTOR_CMD_CAN_STEER_ID};

CAN_message_t recv_msg ;
CAN_message_t send_msg ;

void setup()
{
    Serial.begin(115200);

    
    Serial.println("CAN BUS Shield init ok!");
    
    analogReference(INTERNAL);
    pinMode(PWM_PIN, OUTPUT);
    pinMode(EN_A_PIN, OUTPUT);
    pinMode(EN_B_PIN, OUTPUT);
    setMotorSpeed(0.0);

    pinMode(ENCODER_PIN, INPUT);           // set pin to input
    digitalWrite(ENCODER_PIN, HIGH);       // turn on pullup resistors
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), handleEncoderInterrupt, RISING);
    
    myservo.attach(SERVO_PIN);
    myservo.write(90); //center servo
    detached = 0 ;

    
    CANbus.begin();

    CANbus.setFilter(motor_speed_filter, 0);
    CANbus.setFilter(steer_filter, 1);
  
}

unsigned long elapsed = 0, last = 0, watchdog_timer = 0 ; 
unsigned long fity_hz_timer = 0 ;
unsigned long five_hundred_hz_timer = 0 ;
float motor_current = 0.0 ;
int current_measure_nb = 0 ;
void loop()
{
    elapsed = millis() - last ;
    watchdog_timer += elapsed ;
    fity_hz_timer += elapsed ;
    five_hundred_hz_timer += elapsed ;
    
    last = millis() ;
    if(CANbus.available())                   // check if get data
    {
        CANbus.read(recv_msg);
        if(recv_msg.id == MOTOR_CMD_CAN_SPEED_ID && recv_msg.len <= 4){
           float * new_speed ; 
           new_speed = (float *) recv_msg.buf ;
           setMotorSpeed(*new_speed);
           watchdog_timer = 0 ;
        }else if(recv_msg.id == MOTOR_CMD_CAN_STEER_ID && recv_msg.len <= 2){
           int * steering ; 
           steering = (int *) recv_msg.buf ;
           if(detached){
            myservo.attach(SERVO_PIN);
            detached = 0 ;
           }
           myservo.write(*steering); 
           watchdog_timer = 0 ;
        }
    }

    if(five_hundred_hz_timer > 5){
      int current_raw = analogRead(A0);
      motor_current += (((float) current_raw)*(1.1/1024.0)) * V_TO_A ;
      current_measure_nb ++ ;
      five_hundred_hz_timer = 0 ;
    }
    
    //motor current consumption is sent every 20ms
    if(fity_hz_timer > 20){
     
      motor_current = motor_current/current_measure_nb ;
      send_msg.id = MOTOR_DATA_CAN_CURRENT_ID ;
      send_msg.ext = 0 ;
      send_msg.len = sizeof(float);
      send_msg.timeout = 0 ;
      memcpy(send_msg.buf, &motor_current, sizeof(float));
      CANbus.write(send_msg);

      
      current_measure_nb = 0 ;
      motor_current = 0.0 ;

      if((micros() - old_time) > 100000UL){
        frequency = 0.0 ; // speed slower than 10Hz are reported as 0Hz
      }else{
        frequency = 1000000./ ((float) diff_time);
      }
      motor_current = motor_current/current_measure_nb ;
      send_msg.id = MOTOR_DATA_CAN_ODO_ID ;
      send_msg.ext = 0 ;
      send_msg.len = sizeof(float);
      send_msg.timeout = 0 ;
      memcpy(send_msg.buf, &frequency, sizeof(float));
      CANbus.write(send_msg);
       fity_hz_timer = 0 ;
    }
    
    
    // software watchdog. If no command is received for more than 100ms, the motor brakes, and servo default to center
    if(watchdog_timer > 100){
      setMotorSpeed(0.0); // prevent car from running away when no commands are received
      myservo.write(90); 
      myservo.detach();
      detached = 1 ;
    }
}
