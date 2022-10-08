/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

/* (1) Add the Azure RTOS ThreadX library header file. */
#include <tx_api.h>

/* (3)(8) Add the thread stack memory and thread control block. */
#define THREAD_STACK_SIZE 512

TX_THREAD thread_0;
TX_THREAD thread_1;

UCHAR thread_0_stack[THREAD_STACK_SIZE];
UCHAR thread_1_stack[THREAD_STACK_SIZE];

/* (4) Define the thread's entry function. */
void thread_0_entry(ULONG thread_input)
{
  (VOID)thread_input;

  /* This thread simply sits in while-forever-sleep loop.  */
  while(1)
  {
    /* (5) Move the LED blink logic into the thread's entry function. */
    digitalWrite(LED_BUILTIN, HIGH);            // turn the LED on
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND); // wait for a second
    digitalWrite(LED_BUILTIN, LOW);             // turn the LED off
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND); // wait for a second  
  }
}

/* (9) Define the thread's entry function. */
void thread_1_entry(ULONG thread_input)
{
  (VOID)thread_input;

  /* (10) Add serial read logic to the thread's entry function. */
  Serial.begin(115200);
  
  while(1)
  {
    if (Serial.available() > 0)
    {
      char byteRead = Serial.read();
      Serial.print(byteRead);
    }
  }
}

/* (6) Add the application's environment setup function. */
void tx_application_define(void *first_unused_memory)
{
  (VOID)first_unused_memory;

  /* Put system definition stuff in here, e.g. thread creates and other assorted
     create information.  */

  /* (7)(11) Create the thread. */
  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
    thread_0_stack, THREAD_STACK_SIZE, 
    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

  tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
    thread_1_stack, THREAD_STACK_SIZE, 
    4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  /* (2) Add the kernel entry function. */
  tx_kernel_enter();
} 

// the loop function runs over and over again forever
void loop() {
  /* (5) Move the LED blink logic into the thread's entry function. */
  /* This will never be called. */
}
