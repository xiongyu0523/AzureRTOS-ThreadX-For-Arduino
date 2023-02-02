/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * This is a multi-threading demo using the Azure RTOS ThreadX for Arduino library. It uses message 
 * queues and mutexes for thread communication and synchronization. The code is designed to run on 
 * a Seeed Wio Terminal as a light-monitoring app. This sketch is part of the Azure RTOS ThreadX 
 * tutorial series published on Hackster.io.
 *
 * For info on the Wio Terminal components and sprite library used, please see:
 * https://wiki.seeedstudio.com/Wio-Terminal-Switch/
 * https://wiki.seeedstudio.com/Wio-Terminal-Light/
 * https://wiki.seeedstudio.com/Wio-Terminal-LCD-Overview/
 * https://wiki.seeedstudio.com/Wio-Terminal-Buzzer/
 * https://github.com/Bodmer/TFT_eSPI/blob/master/examples/Sprite/Transparent_Sprite_Demo/Transparent_Sprite_Demo.ino
 */

/* Azure RTOS ThreadX and Sprite libraries*/
#include <TFT_eSPI.h>
#include <tx_api.h>

#define THREAD_STACK_SIZE 1024
#define QUEUE_STACK_SIZE 240
#define DEFAULT_LIGHT_THRESHOLD 200
#define LIGHT_VALUE_STRING_SIZE 20

TFT_eSPI tft;
TFT_eSprite img = TFT_eSprite(&tft);

/* Thread, message queue, and mutex control blocks */
TX_THREAD light_sensor_controller_thread;
TX_THREAD switch_press_controller_thread;
TX_THREAD button_a_controller_thread;
TX_THREAD button_b_controller_thread;
TX_THREAD button_c_controller_thread;
TX_THREAD lcd_controller_thread;
TX_QUEUE lcd_queue;
TX_MUTEX light_threshold_mutex;

/* Thread and message queue stacks */
UCHAR light_sensor_controller_stack[THREAD_STACK_SIZE];
UCHAR switch_press_controller_stack[THREAD_STACK_SIZE];
UCHAR button_a_controller_stack[THREAD_STACK_SIZE];
UCHAR button_b_controller_stack[THREAD_STACK_SIZE];
UCHAR button_c_controller_stack[THREAD_STACK_SIZE];
UCHAR lcd_controller_stack[THREAD_STACK_SIZE];
UCHAR lcd_queue_stack[QUEUE_STACK_SIZE];

/* Light threshold variables */
int32_t light_threshold = DEFAULT_LIGHT_THRESHOLD;
char light_value_string[LIGHT_VALUE_STRING_SIZE];

/* Message related data types for the message queue */
enum lcd_message_type
{
    lcd_message_undefined,
    lcd_message_light_value,
    lcd_message_light_threshold_reset,
    lcd_message_light_threshold_increased,
    lcd_message_light_threshold_decreased,
    lcd_message_light_threshold_reached,
    lcd_message_buzzer_silenced,
};

struct lcd_message 
{
    lcd_message_type type;
    int32_t value;
};

/* Thread entry functions */
void light_sensor_controller_entry(ULONG thread_input);
void switch_press_controller_entry(ULONG thread_input);
void button_a_controller_entry(ULONG thread_input);
void button_b_controller_entry(ULONG thread_input);
void button_c_controller_entry(ULONG thread_input);
void lcd_controller_entry(ULONG thread_input);

/* LCD drawing functions */
void lcd_draw_background();
void lcd_draw_light_value(char* value);
void lcd_draw_alarm(lcd_message_type message_type);
void lcd_draw_light_threshold_value(char* value);

void setup()
{
    /* Setup serial */
    Serial.begin(9600);

    /* Setup environment sensor */
    pinMode(WIO_LIGHT, INPUT);

    /* Setup user controls */
    pinMode(WIO_5S_PRESS, INPUT_PULLUP);
    pinMode(WIO_KEY_A, INPUT_PULLUP);
    pinMode(WIO_KEY_B, INPUT_PULLUP);
    pinMode(WIO_KEY_C, INPUT_PULLUP);
    
    /* Setup buzzer */
    pinMode(WIO_BUZZER, OUTPUT);

    /* Setup LCD */
    tft.begin();
    tft.setRotation(3);
    lcd_draw_background();
    lcd_draw_light_value("");
    lcd_draw_alarm(lcd_message_undefined);
    itoa(DEFAULT_LIGHT_THRESHOLD, light_value_string, 10);
    lcd_draw_light_threshold_value(light_value_string);

    /* Entering the Azure RTOS ThreadX kernel. This ultimately calls tx_application_define(). */
    tx_kernel_enter();
} 

/* 
 * Because tx_kernel_enter() is called in setup(), loop() will never be called. However, the Arduino
 * compiler still expects to see loop() defined so it cannot be removed.
 */
void loop()
{
}

/* Thread entry functions */

/* 
 * This thread reads the light sensor and updates the LCD. If the light threshold is reached, a 
 * buzzer will sound and the LCD is updated.
 */
void light_sensor_controller_entry(ULONG thread_input)
{
    (VOID)thread_input;

    UINT status;

    while(1)
    {
        /* Read light sensor. */
        int32_t light = analogRead(WIO_LIGHT);

        /* Update LCD. */
        lcd_message message = {lcd_message_light_value, light};
        tx_queue_send(&lcd_queue, &message, TX_WAIT_FOREVER);

        /* Check light threshold. */
        if (light >= light_threshold)
        {
            Serial.println("Light sensor: threshold reached.");

            /* Turn on buzzer. */
            analogWrite(WIO_BUZZER, 128);

            /* Update LCD. */
            lcd_message message = {lcd_message_light_threshold_reached, 0};
            tx_queue_send(&lcd_queue, &message, TX_WAIT_FOREVER);

            Serial.println("Light sensor: buzzer turned on.");
        }

        tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);
    }
}

/*
 * This thread checks if the 5-way switch is pressed, turns off the buzzer, and updates the LCD.
 */
void switch_press_controller_entry(ULONG thread_input)
{
    (VOID)thread_input;

    while(1)
    {
        /* Check if 5-way switch is pressed. */
        if (digitalRead(WIO_5S_PRESS) == LOW) 
        {
            Serial.println("5-way switch: pressed.");

            /* Turn off buzzer. */
            analogWrite(WIO_BUZZER, 0);
            
            /* Update LCD. */
            lcd_message message = {lcd_message_buzzer_silenced, 0};
            tx_queue_send(&lcd_queue, &message, TX_WAIT_FOREVER);

            Serial.println("5-way switch: buzzer silenced.");
        }

        tx_thread_sleep(15);
    }
}

/*
 * This thread checks if Button A is pressed, increases the light threshold, and updates the LCD.
 */
void button_a_controller_entry(ULONG thread_input)
{
    (VOID)thread_input;

    UINT status;

    while(1)
    {
        /* Check if Button A pressed. */
        if (digitalRead(WIO_KEY_A) == LOW)
        {
            Serial.println("Button A: Pressed.");

            Serial.println("Button A: About to get mutex");
            tx_mutex_get(&light_threshold_mutex, TX_WAIT_FOREVER);
            Serial.println("Button A: Got mutex");

            /* Increase light threshold. */
            light_threshold = light_threshold + 5;

            /* Used to demonstrate effectiveness of mutex. See readme for more information. */
            // tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 3);

            /* Update LCD. */
            lcd_message message = {lcd_message_light_threshold_increased, light_threshold};
            tx_queue_send(&lcd_queue, &message, TX_WAIT_FOREVER);
            
            Serial.print("Button A: Light threshold increased to ");
            Serial.println(light_threshold);

            Serial.println("Button A: About to put mutex");
            tx_mutex_put(&light_threshold_mutex);
            Serial.println("Button A: Put mutex");
        }

        tx_thread_sleep(15);
    }
}

/*
 * This thread checks if Button B is pressed, decreases the light threshold, and updates the LCD.
 */
void button_b_controller_entry(ULONG thread_input)
{
    (VOID)thread_input;

    UINT status;

    while(1)
    {
         /* Check if Button B pressed. */
        if (digitalRead(WIO_KEY_B) == LOW)
        {
            Serial.println("Button B: Pressed.");

            Serial.println("Button B: About to get mutex");
            tx_mutex_get(&light_threshold_mutex, TX_WAIT_FOREVER);
            Serial.println("Button B: Got mutex");

            /* Decrease light threshold. */
            light_threshold = light_threshold - 5;

            /* Update LCD. */
            lcd_message message = {lcd_message_light_threshold_decreased, light_threshold};
            tx_queue_send(&lcd_queue, &message, TX_WAIT_FOREVER);
            
            Serial.print("Button B: Light threshold decreased to ");
            Serial.println(light_threshold);     

            Serial.println("Button B: About to put mutex");
            tx_mutex_put(&light_threshold_mutex); 
            Serial.println("Button B: Put mutex");
        }

        tx_thread_sleep(15);
    }
}

/*
 * This thread checks if Button C is pressed, resets the light threshold, and updates the LCD.
 */
void button_c_controller_entry(ULONG thread_input)
{
    (VOID)thread_input;

    UINT status;

    while(1)
    {
        /* Check if Button C pressed. */
        if (digitalRead(WIO_KEY_C) == LOW)
        {
            Serial.println("Button C: Pressed.");

            Serial.println("Button C: About to get mutex");
            tx_mutex_get(&light_threshold_mutex, TX_WAIT_FOREVER);
            Serial.println("Button C: Gt mutex");

            /* Reset light threshold. */
            light_threshold = DEFAULT_LIGHT_THRESHOLD;

            /* Update LCD. */
            lcd_message message = {lcd_message_light_threshold_reset, light_threshold};
            tx_queue_send(&lcd_queue, &message, TX_WAIT_FOREVER);
            
            Serial.print("Button C: Light threshold reset to ");
            Serial.println(light_threshold);  

            Serial.println("Button C: About to put mutex");
            tx_mutex_put(&light_threshold_mutex);  
            Serial.println("Button C: Put mutex");      
        }

        tx_thread_sleep(15);
    }
}

/*
 * This thread checks if any messages have been received in the lcd_queue. If there is a message,
 * it updates the LCD accordingly.
 */
void lcd_controller_entry(ULONG thread_input)
{
    (VOID)thread_input;

    lcd_message message = {lcd_message_undefined, 0};

    while(1)
    {   
        tx_queue_receive(&lcd_queue, &message, TX_WAIT_FOREVER);

        switch(message.type)
        {
            case lcd_message_light_value:
                itoa(message.value, light_value_string, 10);
                //Serial.print("LCD: light value: ");
                //Serial.println(message.value);

                lcd_draw_light_value(light_value_string);
                break;

            case lcd_message_light_threshold_reset:
                itoa(message.value, light_value_string, 10);
                Serial.print("LCD: light threshold reset to ");
                Serial.println(message.value);

                lcd_draw_light_threshold_value(light_value_string);
                break;
       
            case lcd_message_light_threshold_increased:
                itoa(message.value, light_value_string, 10);
                Serial.print("LCD: light threshold increased to ");
                Serial.println(message.value);

                lcd_draw_light_threshold_value(light_value_string);
                break;

            case lcd_message_light_threshold_decreased:
                itoa(message.value, light_value_string, 10);
                Serial.print("LCD: light threshold decreased to ");
                Serial.println(message.value);

                lcd_draw_light_threshold_value(light_value_string);
                break;
        
            case lcd_message_light_threshold_reached:
                Serial.print("LCD: light threshold reached");
                lcd_draw_alarm(lcd_message_light_threshold_reached);
                break;
        
            case lcd_message_buzzer_silenced:
                Serial.println("LCD: shhhh!!! buzzer silenced");
                lcd_draw_alarm(lcd_message_buzzer_silenced);
                break;

            default:
                Serial.println("UNDEFINED");
        }
    }
}

/* 
 * Azure RTOS for ThreadX environment function (required). All threads, message queue, and mutex are
 * created here.
 */
void tx_application_define(void *first_unused_memory)
{
    (VOID)first_unused_memory;

    tx_thread_create(&light_sensor_controller_thread, "Light Sensor Controller Thread",
        light_sensor_controller_entry, 0, light_sensor_controller_stack, THREAD_STACK_SIZE,
        0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&switch_press_controller_thread, "Switch Press Controller Thread",
        switch_press_controller_entry, 0, switch_press_controller_stack, THREAD_STACK_SIZE,
        8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&button_a_controller_thread, "Button A Controller Thread",
        button_a_controller_entry, 0, button_a_controller_stack, THREAD_STACK_SIZE, 
        8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&button_b_controller_thread, "Button B Controller Thread",
        button_b_controller_entry, 0, button_b_controller_stack, THREAD_STACK_SIZE, 
        8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&button_c_controller_thread, "Button C Controller Thread",
        button_c_controller_entry, 0, button_c_controller_stack, THREAD_STACK_SIZE, 
        6, 6, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_thread_create(&lcd_controller_thread, "LCD Controller Thread",
        lcd_controller_entry, 0, lcd_controller_stack, THREAD_STACK_SIZE, 
        4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_queue_create(&lcd_queue, "LCD Queue", sizeof(lcd_message) / 4, lcd_queue_stack, QUEUE_STACK_SIZE);
    tx_mutex_create(&light_threshold_mutex, "Light Threshold Mutex", TX_NO_INHERIT); 
}

/* LCD drawing functions */
void lcd_draw_background()
{
    tft.fillScreen(TFT_NAVY);

    img.setColorDepth(8);
    img.createSprite(300, 220);
    img.fillSprite(TFT_BLACK);

    img.fillRoundRect(0, 0, 300, 220, 15, TFT_WHITE);

    img.setTextColor(TFT_NAVY);
    img.setTextSize(2);
    img.drawString("Light: ", 20, 20);
    img.drawString("Light Threshold: ", 20, 140);
    
    img.pushSprite(10, 10, TFT_BLACK);

    img.deleteSprite();
}

void lcd_draw_light_value(char* value)
{
    img.setColorDepth(8);
    img.createSprite(80, 60);
    img.fillSprite(TFT_WHITE);

    img.setTextColor(TFT_NAVY);
    img.setTextSize(3);
    img.drawString(String(value), 0, 0);
    
    img.pushSprite(55, 70);

    img.deleteSprite();
}

void lcd_draw_alarm(lcd_message_type message_type)
{
    img.setColorDepth(8);
    img.createSprite(140, 95);
 
    img.setTextSize(3);

    if (message_type == lcd_message_undefined)
    {
        img.fillSprite(TFT_WHITE);
        img.drawString("", 0, 0);
    }
    else if (message_type == lcd_message_light_threshold_reached)
    {
        img.fillSprite(TFT_WHITE);
        img.fillRoundRect(0, 0, 140, 95, 20, TFT_YELLOW);
        img.setTextColor(TFT_ORANGE);
        img.drawString("Good", 32, 10);
        img.drawString("Morning", 7, 35);
        img.drawString("Sun!", 37, 60);
    }
    else if (message_type == lcd_message_buzzer_silenced)
    {
        img.fillSprite(TFT_WHITE);
        img.fillRoundRect(0, 0, 140, 95, 20, TFT_NAVY);
        img.setTextColor(TFT_WHITE);
        img.drawString("Shhhh!", 20, 40);
    }
    else
    {
        img.drawString("", 0, 0);
        Serial.println("Not a valid message type for alarm.");
    }
    
    img.pushSprite(150, 25);

    img.deleteSprite();
}

void lcd_draw_light_threshold_value(char* value)
{
    img.setColorDepth(8);
    img.createSprite(80, 40);
    img.fillSprite(TFT_WHITE);

    img.setTextColor(TFT_NAVY);
    img.setTextSize(3);
    img.drawString(String(value), 0, 0);
    
    img.pushSprite(120, 185);

    img.deleteSprite();
}
