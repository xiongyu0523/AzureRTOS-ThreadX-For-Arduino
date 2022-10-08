/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * This is a basic multi-threading demo using the Azure RTOS ThreadX for
 * Arduino library. It combines the classic Blink example with serial input.
 * This sketch is part of the Azure RTOS ThreadX tutorial series published on 
 * Hackster.io.
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
      char byte_read = Serial.read();
      Serial.print(byte_read);
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
