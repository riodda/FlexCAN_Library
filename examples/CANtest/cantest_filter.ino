// -------------------------------------------------------------
// CANtest for Teensy 3.1
// by riodda based on teachop Caantest sketch
//
// This sktch wil send over the bus one example frame and display on the console
// the frames that are passing the filters myFilter_0 and myFilter_1
//

#include <Metro.h>
#include <FlexCAN.h>

Metro sysTimer = Metro(1);// milliseconds

int led = 13;
FlexCAN CANbus(500000);
static CAN_message_t msg,rxmsg;
static uint8_t hex[17] = "0123456789abcdef";

int txCount,rxCount;
unsigned int txTimer,rxTimer;
static CAN_filter_t myFilter_0, myFilter_1;
static CAN_filter_t myMask;


// -------------------------------------------------------------
static void hexDump(uint8_t dumpLen, uint8_t *bytePtr)
{
  uint8_t working;
  while( dumpLen-- ) {
    working = *bytePtr++;
    Serial.write( hex[ working>>4 ] );
    Serial.write( hex[ working&15 ] );
  }
  Serial.write('\r');
  Serial.write('\n');
}


// -------------------------------------------------------------
void setup(void)
{
  Serial.begin(115200);
  myMask.rtr=0;
  myMask.ext=0;
  myMask.id=0xfff;
  CANbus.begin(myMask);
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  myFilter_0.id = 0x002;
  myFilter_0.ext = 0;
  myFilter_0.rtr = 0;
  myFilter_1.id = 0x004;
  myFilter_1.ext = 0;
  myFilter_1.rtr = 0;
  delay(10000);
  Serial.println(F("Hello Teensy 3.1 CAN Test."));
  CANbus.setFilter(myFilter_0,0);
  CANbus.setFilter(myFilter_0,1);
  CANbus.setFilter(myFilter_0,2);
  CANbus.setFilter(myFilter_0,3);
  CANbus.setFilter(myFilter_1,4);
  CANbus.setFilter(myFilter_1,5);
  CANbus.setFilter(myFilter_1,6);
  CANbus.setFilter(myFilter_1,7);
  sysTimer.reset();
}


// -------------------------------------------------------------
void loop(void)
{
  // service software timers based on Metro tick
  if ( sysTimer.check() ) {
    if ( txTimer ) {
      --txTimer;
    }
    if ( rxTimer ) {
      --rxTimer;
    }
  }

  // if not time-delayed, read CAN messages and print 1st byte
  if ( !rxTimer ) {
    while ( CANbus.read(rxmsg) ) {
      //hexDump( sizeof(rxmsg), (uint8_t *)&rxmsg );
      Serial.print(rxmsg.id,HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[0],HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[1],HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[2],HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[3],HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[4],HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[5],HEX);
      Serial.print(":");
      Serial.print(rxmsg.buf[6],HEX);
      Serial.print(":");
      Serial.println(rxmsg.buf[7],HEX);
      rxCount++;
    }
  }

  // insert a time delay between transmissions
  if ( !txTimer ) {
    // if frames were received, print the count
    if ( rxCount ) {
      Serial.write('=');
      Serial.print(rxCount);
      rxCount = 0;
    }
    txTimer = 100;//milliseconds
    msg.len = 8;
    msg.id = 0x222;
    for( int idx=0; idx<8; ++idx ) {
      msg.buf[idx] = '0'+idx;
    }
    // send 6 at a time to force tx buffering
    txCount = 6;
    digitalWrite(led, 1);
    Serial.println(".");
    while ( txCount-- ) {
      CANbus.write(msg);
      msg.buf[0]++;
    }
    digitalWrite(led, 0);
    // time delay to force some rx data queue use
    rxTimer = 3;//milliseconds
  }

}
