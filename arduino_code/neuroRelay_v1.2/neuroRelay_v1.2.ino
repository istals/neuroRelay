#include <ctype.h>

String BUFFER;

#define NONE 0
#define RELAY_1  13
#define RELAY_2  12
#define RELAY_3  11
#define RELAY_4  10
#define RELAY_5  9
#define RELAY_6  8
#define RELAY_7  7
#define RELAY_8  6
#define RELAY_9  5
#define RELAY_10  4
#define RELAY_11  3
#define RELAY_12  2

static unsigned long RELAY_DELAY = 200;
static unsigned long SOLENOID_WARM_UP_DELAY = 500;



static unsigned long relay_off_time;
int active_relay = NONE;
int last_active_relay = NONE;

bool solenoids_warmed = false;


void setup() 
{
    Serial.begin(9600);

    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT); 
    pinMode(RELAY_3, OUTPUT); 
    pinMode(RELAY_4, OUTPUT); 
    pinMode(RELAY_5, OUTPUT); 
    pinMode(RELAY_6, OUTPUT); 
    pinMode(RELAY_7, OUTPUT);
    pinMode(RELAY_8, OUTPUT); 
    pinMode(RELAY_9, OUTPUT); 
    pinMode(RELAY_10, OUTPUT); 
    pinMode(RELAY_11, OUTPUT); 
    pinMode(RELAY_12, OUTPUT);

    reset_relays();
    warm_up_solenoids();
}

void loop() 
{
    if ( solenoids_warmed )
    {
        check_relay_timeout();
        // read incoming messages
    
        String message = read_serial();
        Serial.println(message);
        
        if ( active_relay == NONE )
        {
            process_message(message);
        }
    }
}

void warm_up_solenoids()
{
    digitalWrite(RELAY_1, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_1, LOW);

    digitalWrite(RELAY_2, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_2, LOW);

    digitalWrite(RELAY_3, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_3, LOW);

    digitalWrite(RELAY_4, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_4, LOW);

    digitalWrite(RELAY_5, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_5, LOW);

    digitalWrite(RELAY_6, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_6, LOW);

    digitalWrite(RELAY_7, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_7, LOW);

    digitalWrite(RELAY_8, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_8, LOW);

    digitalWrite(RELAY_9, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_9, LOW);

    digitalWrite(RELAY_10, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_10, LOW);

    digitalWrite(RELAY_11, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_11, LOW);

    digitalWrite(RELAY_12, HIGH);
    delay(SOLENOID_WARM_UP_DELAY);
    digitalWrite(RELAY_12, LOW);
    
    solenoids_warmed = true;
}

void process_message(String message)
{
    if ( message.length() > 0 )
    {
        Serial.println(message);
        if ( is_valid_number(message) ) 
        {
            int attention = message.toInt();
            turn_on_relay(attention);   
        }
    }
}

String read_serial() {
    String message = "";
    while ( Serial.available() > 0) 
    {
        check_relay_timeout();
        char ch = Serial.read();
        if ( ch == '\n' )
        {
            message = BUFFER;
            BUFFER = "";
        } 
        else 
        {
            if ( sizeof(ch) > 0)
            {
                BUFFER.concat(ch);
            }
        }
        yield();
    }

    return message;
}

boolean is_valid_number( String str ){
    for( byte i=0; i < str.length(); i++)
    {
        if( isdigit( str.charAt(i)) ) return true;
    }
    return false;
}

void reset_relays()
{
    digitalWrite(RELAY_1, LOW);
    digitalWrite(RELAY_2, LOW); 
    digitalWrite(RELAY_3, LOW); 
    digitalWrite(RELAY_4, LOW); 
    digitalWrite(RELAY_5, LOW); 
    digitalWrite(RELAY_6, LOW); 
    digitalWrite(RELAY_7, LOW);
    digitalWrite(RELAY_8, LOW); 
    digitalWrite(RELAY_9, LOW); 
    digitalWrite(RELAY_10, LOW); 
    digitalWrite(RELAY_11, LOW); 
    digitalWrite(RELAY_12, LOW);
}

void check_relay_timeout()
{
    if ( active_relay != NONE )
    {
        if ( millis() > relay_off_time ) 
        {
            reset_relays();
            
            active_relay = NONE;
        }
    }
}

void turn_on_relay(int attention)
{
    if ( attention >= 1 && attention < 11 && last_active_relay != RELAY_1 ) {
        digitalWrite(RELAY_1, HIGH);
        active_relay = RELAY_1;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 11 && attention < 21 && last_active_relay != RELAY_2 ) {
        digitalWrite(RELAY_2, HIGH);
        active_relay = RELAY_2;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 21 && attention < 31 && last_active_relay != RELAY_3 ) {
        digitalWrite(RELAY_3, HIGH);
        active_relay = RELAY_3;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 31 && attention < 41 && last_active_relay != RELAY_4 ) {
        digitalWrite(RELAY_4, HIGH);
        active_relay = RELAY_4;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 41 && attention < 51 && last_active_relay != RELAY_5 ) {
        digitalWrite(RELAY_5, HIGH);
        active_relay = RELAY_5;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 51 && attention < 61 && last_active_relay != RELAY_6 ) {
        digitalWrite(RELAY_6, HIGH);
        active_relay = RELAY_6;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 61 && attention < 71 && last_active_relay != RELAY_7 ) {
        digitalWrite(RELAY_7, HIGH);
        active_relay = RELAY_7;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 71 && attention < 81 && last_active_relay != RELAY_8 ) {
        digitalWrite(RELAY_8, HIGH);
        active_relay = RELAY_8;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 81 && attention < 91 && last_active_relay != RELAY_9 ) {
        digitalWrite(RELAY_9, HIGH);
        active_relay = RELAY_9;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 91 && attention < 101 && last_active_relay != RELAY_10 ) {
        digitalWrite(RELAY_10, HIGH);
        active_relay = RELAY_10;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 101 && attention < 111 && last_active_relay != RELAY_11 ) {
        digitalWrite(RELAY_11, HIGH);
        active_relay = RELAY_11;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }

    if ( attention >= 111 && attention < 121 && last_active_relay != RELAY_12 ) {
        digitalWrite(RELAY_12, HIGH);
        active_relay = RELAY_12;
        last_active_relay = active_relay;
        relay_off_time = millis() + RELAY_DELAY;
    }
}



