# Azure RTOS ThreadX for Arduino 101: Threads

**Total completion time**:  30 minutes

- [Azure RTOS ThreadX 101: Threads](#azure-rtos-threadx-101-threads)
  - [Introduction](#introduction)
    - [What is covered](#what-is-covered)
    - [Prerequisites](#prerequisites)
  - [Setup](#setup)
  - [Part 1: Run the Arduino Blink example](#part-1-run-the-arduino-blink-example)
    - [Deep Dive](#deep-dive)
  - [Part 2: Convert the Blink example via ThreadX](#part-2-convert-the-blink-example-via-threadx)
    - [Deep Dive](#deep-dive-1)
  - [Part 3: Apply multi-threading to the Blink example via ThreadX](#part-3-apply-multi-threading-to-the-blink-example-via-threadx)
  
## Introduction

This tutorial will show you how to use multi-threading with Azure RTOS Threadx. You will start with the classic Blink example via Arduino and convert it.

**_Azure RTOS_**: A Microsoft development suite for embedded IoT applications on microcontrollers (MCUs).  [Azure RTOS](https://azure.microsoft.com/en-us/products/rtos/#overview) can be used **independent** of Microsoft's [Azure](https://azure.microsoft.com/en-us/resources/cloud-computing-dictionary/what-is-azure) cloud platform.

**_Azure RTOS ThreadX_**: One component of the Azure RTOS product offering. [ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/overview-threadx) is the real time operating system (RTOS) designed to run on MCUs.

**_Azure RTOS ThreadX for Arduino_**: A port of Azure RTOS ThreadX to Arduino as a library. Please visit [AzureRTOS-ThreadX-For-Arduino](https://github.com/xiongyu0523/AzureRTOS-ThreadX-For-Arduino) on GitHub for the [source code](https://github.com/xiongyu0523/AzureRTOS-ThreadX-For-Arduino/tree/main/src).

### What is covered
By the end of this tutorial, you should understand the following:

**Terms**: kernel, thread, thread control block, priority level, preemption, preemption threshold

**Actions**: How to implement a single thread using ThreadX; How to implement multiple threads using ThreadX;
  
**Final code**: View the full ThreadX multi-threaded Blink code example [here](). <need to add link> 

### Prerequisites
- Have the [Arduino IDE 1.8.x](https://www.arduino.cc/en/software) installed.
- Have a device using an ATSAMD21 or ATSAMD51 chip. View this [list](https://github.com/xiongyu0523/AzureRTOS-ThreadX-For-Arduino/tree/main#hardware-support) of verified boards.

_The following was run on Windows 11, Arduino IDE 1.8.19, and the Arduino MKR WiFi 1010._

## Setup

1. Open the Arduino IDE.

1. Install the Azure RTOS Arduino library.

    - Navigate to **Tools > Manage Libraries...**.
    - Search for **'Azure RTOS'**. 
    - Install **'Azure RTOS ThreadX'**.  _Be sure to install the latest version._
    
        <img src="docs/LibraryManager_AzureRTOS.png" alt="How to install the Azure RTOS Arduino library using the Arduino Library Manager" width="800"/>

2. Install the board package for your device. (_This sample uses the Arduino MKR WiFi 1010._)

    - Navigate to **Tools > Board: ... > Boards Manager...**
    - Search for **'MKR WiFi 1010'**.
    - Install **'Arduino SAMD Boards (32-bits ARM Cortex-M0+)'**. _Be sure to install the latest version._

        <img src="docs/BoardsManager_MKRWiFi1010.png" alt="How to install the board package for the MKR WiFi 1010 using the Arduino Boards Manager" width="800"/>
   
## Part 1: Run the Arduino Blink example
In this section we will run the traditional Blink example to confirm the device is setup properly.

1. Open the Blink example.

    - Navigate to **File > Examples > 01.Basics**.
    - Select **'Blink'**.
    
1. Save the example. 

    - Navigate to **File > Save As**.
    - Save the file as **'Blink_AzureRTOS.ino'**.
    
1. Connect your device.
   
   - Plug in your device to your PC.
   - Navigate to **Tools > Board: ... > Arduino SAMD Boards (32-bits ARM Cortex-M0+)**
   - Select **'Arduino MKR WiFi 1010'**.
   - Navigate to **Tools > Port**.
   - Select **'\<Port associated with device\>'**.

1. Run the example.
   
   - In the top left corner, select the **'Upload'** icon. _Verification will automatically occur first._
   - Observe the LED blink on and off every 1 second.

### Deep Dive

<details><summary><i>Code</i></summary>
<p>

```
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
```

See full Arduino [Blink](https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink) example for more information.

</p>
</details>

<details><summary><i>What is going on?</i></summary>
<p>

Arduino makes use of two core functions: [`setup()`](https://www.arduino.cc/reference/en/language/structure/sketch/setup/) and [`loop()`](https://www.arduino.cc/reference/en/language/structure/sketch/loop/).  Once `setup()` completes, `loop()` is internally kicked off and runs the remainder of the program.  Because there is no RTOS present, this code can be considered _bare metal_ programming.   

See full Arduino [Blink](https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink) example for more information.

</p>
</details>

## Part 2: Convert the Blink example via ThreadX
In this section we will convert the bare metal Blink example to a single-threaded RTOS version using ThreadX.

1. Add the Azure RTOS ThreadX library header file near the top of the file. Place it after the commentary, but before the `setup()` function.

    ```
    /* (1) Include the Azure RTOS ThreadX library. */
    #include <tx_api.h>
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    [`tx_api.h`](https://github.com/xiongyu0523/AzureRTOS-ThreadX-For-Arduino/blob/main/src/tx_api.h) is the only header file you need to include to use ThreadX for Arduino. `tx` is short for ThreadX. All functions in the API will begin with `tx`. All constatnts and data types will begin with `TX`. 

    </p>
    </details>

2. Add the **_kernel_** entry function to `setup()`.

    ```
    // the setup function runs once when you press reset or power the board
    void setup() {
      // initialize digital pin LED_BUILTIN as an output.
      pinMode(LED_BUILTIN, OUTPUT);

      /* (2) Enter the tx kernel. */
      tx_kernel_enter();
    }   
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    The **_kernel_** is the core component of an RTOS. Think of it as the lead coordinator or director of logistics for a project. By "entering" the kernel, the RTOS kernel can start running and managing your embedded application. 
    
    The program will never return from `tx_kernel_enter()`. As a result, the application will not return from `setup()` and `loop()` will not be called. 

    > IMPORTANT: _"The call to tx_kernel_enter() does not return, so do not place any processing after it."_

    Please see [Microsoft Learn's ThreadX Chapter 3: Functional Components of ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3) for more information on [`tx_kernel_enter()`](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#main-function).

UCHAR
    </p>
    </details>

3. Define the **_thread control block_** and its stack memory. Place this near the top of the file after `#include <tx_api.h>` and before `setup`.

    ```
    /* (1) Include the Azure RTOS ThreadX library. */
    #include <tx_api.h>
   
    /* (3) Define the thread and its stack memory. */
    #define THREAD_0_STACK_SIZE 512

    TX_THREAD thread_0;
    ULONG thread_0_stack[THREAD_0_STACK_SIZE/sizeof(ULONG)];

    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    A **_thread_** is a specific execution path within a process (i.e., a running application). A thread shares memory space with other threads but has its own allocated stack space. 

    > IMPORTANT: _"ThreadX does not use the term task. Instead, the more descriptive and contemporary name thread is used."_ Please see [Microsoft Learn's ThreadX Chapter 1: Introduction to ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter1) for more information on [tasks vs. threads](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter1#tasks-vs-threads).

    A **_thread control block_** contains specific data for the thread. `TX_THREAD` is the ThreadX data type for a thread control block. Please see [Microsoft Learn's ThreadX Chapter 3: Functional Components of ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3) for more information on [`TX_THREAD`](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#thread-control-block-tx_thread).

    Because we are only using one thread, declaring an array (`thread_0_stack`) suffices to allocate the stack memory. This stack will contain `THREAD_0_STACK_SIZE` bytes total. ULONG is 8 bytes, so the stack will hold up to `THREAD_0_STACK_SIZE`/8 elements.

    Please see [Microsoft Learn's ThreadX Chapter 3: Functional Components of ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3) for more information on the [thread stack area](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#thread-stack-area). 

    *****is it better do use ULONG here or UCHAR?  I've seen both used.  Andres used ULONG for this array approach, but the tx_byte_pool_create in the demo uses UCHAR.  would make sense to have UCHAR there.  Is there a preferred approach?  why?**

    </p>
    </details>

4. Define the thread's entry function. Place the function definition after the defined thread control block and before `setup()`.
   
    ```
    /* (4) Define the thread's entry function. */
    void thread_0_entry(ULONG thread_input)
    {
      (VOID)thread_input;

      /* This thread simply sits in while-forever-sleep loop.  */
      while(1)
      {
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
      }
    }
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    The thread's entry function is called by the kernel and contains the thread execution logic. Typically, this function will contain an infinite loop (i.e., `while(1)`) that will execute throughout the running program. The name of this function is determined by the user. 

    </p>
    </details>

5. Move the LED blink logic from `loop()` into the thread's entry function `while(1)` loop.  Replace the `delay(1000)` with `tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND)`.
   
    ```
    void thread_0_entry(ULONG thread_input)
    {
      (VOID)thread_input;

      /* This thread simply sits in while-forever-sleep loop.  */
      while(1)
      {
        /* (5) Move the LED blink logic into the thread's entry function. */
        digitalWrite(LED_BUILTIN, HIGH);            // turn the LED on (HIGH is the voltage level)
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND); // wait for a second
        digitalWrite(LED_BUILTIN, LOW);             // turn the LED off by making the voltage LOW
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND); // wait for a second  
      }
    }
    ```

    ```
    void loop() {
      /* (5) Move the LED blink logic into the thread's entry function. */
      /* Should never be here */
      while(1);
    }
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    Because `loop()` will no longer be called, the blink logic must be moved into the new thread. The [`delay()`](https://www.arduino.cc/reference/en/language/functions/time/delay/) function has limitations, and since we will later want to suspend the thread to allow other threads to execute, we will use ThreadX's [`tx_thread_sleep()`](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter4#tx_thread_sleep) function instead. This function takes timer ticks as its parameter instead of milliseconds.

    </p>
    </details>

6. Add the function expected by the kernel to setup the application environment and system resources. Place this function after `thread_0_entry()` and before `setup()`.

    ```
    /* (6) Add the application's environment setup function. */
    void tx_application_define(void *first_unused_memory)
    {
      (VOID)first_unused_memory;

      /* Put system definition stuff in here, e.g. thread creates and other assorted
         create information.  */
    }
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    The kernal entry function `tx_kernel_enter()` will call the function `tx_application_define()`. It is the user's responsibility to implement this function with the logic to create system resources for the RTOS environment.   

    Please see [Microsoft Learn's ThreadX Chapter 3: Functional Components of ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3) for more information on [`tx_application_define()`](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#application-definition-function).
    
    </p>
    </details>

7. Create the thread. Add `tx_thread_create()` to the the `tx_application_define()` function.

    ```
    /* (6) Add the application's environment setup function. */
    void tx_application_define(void *first_unused_memory)
    {
      (VOID)first_unused_memory;

      /* Put system definition stuff in here, e.g. thread creates and other assorted
         create information.  */

      /* (7) Create the thread. */
      tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
        thread_0_stack, THREAD_0_STACK_SIZE, 
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    }
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    [`tx_thread_create()`](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter4#tx_thread_create) creates a thread with specified arguments. The arguments used in this example reflect the following:

    - `&thread_0`  : Pointer to the defined thread control block. (See step 3.)
    - `"thread_0"` : The thread name (i.e. pointer to the name.)  
    - `thread_0_entry` : The defined thread entry function. (See steps 4 and 5.)
    - `0` : Entry input to the thread. _We are not utilizing this argument_.
    - `thread_0_stack` : Pointer to the start of the thread's stack. (See step 3.)
    - `THREAD_0_STACK_SIZE`: Size of the thread's stack in bytes. (See step 3.)
    - `1` : The priority level of the thread.
    - `1` : The preemption threshold of the thread.
    - `TX_NO_TIME_SLICE` : Time slicing is disabled.
    - `TX_AUTO_START` : The thread is automatically started.
    
    </br>

    The **_priority level_** of a thread helps the thread scheduler determine what thread to execute next. Some threads may be more critical to execute, and are therefore given a higher priority relative to others. ThreadX has 32 default priority levels from 0 to 31, with 0 being the highest priority and 31 being the lowest. This range can be changed by the user.
    
    **_Preemption_** refers to an existing thread's execution being stopped so a higher priorty can run instead. The scheduler controls this and when the interrupting thread completes, execution returns back to the thread that was suspended. 
    
    A **_preemption threshold_** is unique to ThreadX. Only priorities higher than this threshold may preempt the thread.

    Please see [Microsoft Learn's ThreadX Chapter 3: Functional Components of ThreadX](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3) for more information on [thread execution](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#thread-execution-1), [thread priorities](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#thread-priorities), [thread scheduling](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#thread-scheduling), and [thread preemption](https://learn.microsoft.com/en-us/azure/rtos/threadx/chapter3#preemption).

    </p>
    </details>

8. Run the Blink example using Azure RTOS ThreadX.  Follow the _Connect your device_ and _Run the example_ steps from [Part 1: Run the Arduino Blink example](#part-1-run-the-arduino-blink-example).
   
### Deep Dive

<details><summary><i>Code</i></summary>
<p>

```
/* (1) Include the Azure RTOS ThreadX library. */
#include <tx_api.h>

/* (3) Define the thread and its stack memory. */
#define THREAD_0_STACK_SIZE 512

TX_THREAD thread_0;
ULONG thread_0_stack[THREAD_0_STACK_SIZE/sizeof(ULONG)];

/* (4) Define the thread's entry function. */
void thread_0_entry(ULONG thread_input)
{
  (VOID)thread_input;

  /* This thread simply sits in while-forever-sleep loop.  */
  while(1)
  {
    /* (5) Move the LED blink logic into the thread's entry function. */
    digitalWrite(LED_BUILTIN, HIGH);            // turn the LED on (HIGH is the voltage level)
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND); // wait for a second
    digitalWrite(LED_BUILTIN, LOW);             // turn the LED off by making the voltage LOW
    tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND); // wait for a second  
  }
}

/* (6) Add the application's environment setup function. */
void tx_application_define(void *first_unused_memory)
{
  (VOID)first_unused_memory;

  /* Put system definition stuff in here, e.g. thread creates and other assorted
     create information.  */

  /* (7) Create the thread. */
  tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
    thread_0_stack, THREAD_0_STACK_SIZE, 
    1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  /* (2) Enter the tx kernel. */
  tx_kernel_enter();
}   

// the loop function runs over and over again forever
void loop() {
  /* (5) Move the LED blink logic into the thread's entry function. */
  /* Should never be here */
  while(1);
}
```

</p>
</details>

<details><summary><i>What is going on?</i></summary>
<p>

The code above demonstrates how to replace the Arduino bare metal single-threaded approach of `setup` and `loop` with Azure RTOS ThreadX. 

The prior bare metal code flow was:

- `setup()` -> `loop()` -> infinite loop Blink logic.

The ThreadX new code flow is:

- `setup()` -> `tx_kernel_enter()` -> `tx_application_define()` -> `thread_0_entry()` -> infinite loop Blink logic.

Although this approach still maintains the same single-threaded functionality, it is now setup to add additional threads as needed. Part 3 will demonstrate how to do this.

</p>
</details>

## Part 3: Apply multi-threading to the Blink example via ThreadX
In this section we will use the single-threaded ThreadX Blink code to create a multi-threaded version. 

1. (8) Define two more ThreadX **_thread_** control blocks and space for their stack memory. _Note: This will replace the existing code from **Part 2: Step 3 (Define the thread and its stack memory)** above_. 

    ```
    /* (1) Include the Azure RTOS ThreadX library. */
    #include <tx_api.h>
   
    /* (8) Define the threads and space for their stack memory. */
    #define THREAD_COUNT 3
    #define THREAD_STACK_SIZE 512
    #define BYTE_POOL_SIZE THREAD_COUNT * THREAD_STACK_SIZE

    TX_THREAD thread_0;
    TX_THREAD thread_1;
    TX_THREAD thread_2;

    TX_BYTE_POOL byte_pool;
    UCHAR memory_area[BYTE_POOL_SIZE];
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    *** Need help here explaining the importance to use a byte pool instea of having 3 separate allocated stacks as arrays. 

    </p>
    </details>
    <br/>

2. (9) Define the additional thread entry functions. Place these functions after the existing `thread_0_entry` but before `tx_application_define`. We will implement the specific functionality of each thread in a following step.

    ```
    /* (9) Define the additional thread entry functions. */
    void thread_1_entry(ULONG thread_input)
    {
      (VOID)thread_input;

      /* This thread simply sits in while-forever-sleep loop.  */
      while(1)
      {
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);     // wait for a second  
      }
    }

    void thread_2_entry(ULONG thread_input)
    {
      (VOID)thread_input;

      /* This thread simply sits in while-forever-sleep loop.  */
      while(1)
      {
        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);     // wait for a second  
      }
    }
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    *** To implement.

    </p>
    </details>
    <br/>

3. (10) Create a byte pool, allocate the thread stacks, and create the additional threads. This will all occur in the existing `tx_application_define` function. We will update priority levels for each thread in a following step. _Note: Changes have been made to the original `tx_thread_create` for `thread_0` from **Part 2: Step 7 (Create the Thread)**_. 

    ```
    /* (6) Add the application's environment setup function. */
    void tx_application_define(void *first_unused_memory)
    {
      (VOID)first_unused_memory;

      /* Put system definition stuff in here, e.g. thread creates and other assorted
         create information.  */
      
      /* (10) Create a byte pool, allocate the thread stacks, and create the additional threads. */
      UCHAR *pointer;

      tx_byte_pool_create(&byte_pool, "byte pool", memory_area, BYTE_POOL_SIZE);

      tx_byte_allocate(&byte_pool, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
      tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
        pointer, THREAD_STACK_SIZE, 
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

      tx_byte_allocate(&byte_pool, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
      tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
        pointer, THREAD_STACK_SIZE, 
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);

      tx_byte_allocate(&byte_pool, (VOID **) &pointer, THREAD_STACK_SIZE, TX_NO_WAIT);
      tx_thread_create(&thread_2, "thread 2", thread_2_entry, 0,  
        pointer, THREAD_STACK_SIZE, 
        1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    }
    ```

    <details><summary><i>What is going on?</i></summary>
    <p>

    *** why do I need the byte pool here?  Not clear on its advantages in this situation.. perhaps good for setup for future needs?  How should I explain this?

    </p>
    </details>
    <br/>

4. (11) Implement the two new threads and update the original thread.

    ```

    ```