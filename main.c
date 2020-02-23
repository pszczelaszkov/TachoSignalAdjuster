#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>
//31.25 == 1ms@8Mhz
#define SPEED_KICKSTART_INTERVAL 3125 //10hz
#define RPM_KICKSTART_INTERVAL 6000 //<200RPM

uint16_t RPM[2];
uint16_t speed;//speed is 50/50 duty cycle
uint8_t RPM_i = 1;
uint8_t rpm_enabled,speed_enabled_watchdog;
uint8_t last_intport_state = 0xff;
uint16_t lastRPMstamp,lastSpeedStamp;

int main(void)
{
    RPM[0] = 60;//signal width fixed at ~2ms
    RPM[1] = RPM_KICKSTART_INTERVAL;
    speed = SPEED_KICKSTART_INTERVAL;

    DDRA = 0xf8;
    DDRB = 0xff;
    PORTA = 0x00;
    PORTB = 0xfe;

    TCCR1B = (1 << CS12);
    TIMSK1 = (1 << OCIE1A) | (1 << OCIE1B);
    PCMSK0 = (1 << PCINT0) | (1 << PCINT1);
    GIMSK = (1 << PCIE0);
    sei();
    while(1)
    {
        //servotronic control
        if(speed > 500 && RPM[1] < 160)//less than ~30km/h
            PORTB |= 0x04;
        else if(speed < 200)//more than ~50km/h
            PORTB &= ~0x04;
    };
    return 0;
}

ISR(TIM1_COMPA_vect)
{
    if(RPM[1] < RPM_KICKSTART_INTERVAL)
    {
        OCR1A = TCNT1 + RPM[RPM_i^=1];
        PORTA ^= 0x20;
    }
    else
    {
        RPM_i = 1;//if engine dead make sure to start from beginning
        PORTA |= 0x20;
    }
}

ISR(TIM1_COMPB_vect)
{
    if(speed_enabled_watchdog)
    {
        OCR1B = TCNT1 + speed;
        PORTA ^= 0x08;
        speed_enabled_watchdog--;
    }
}
ISR(PCINT0_vect)
{
    uint8_t changed_pins = last_intport_state ^ PINA;// detects change on pin
    last_intport_state = PINA;
    if((changed_pins & 0x01) && ((PINA & 0x01) == 0x01))//RPM pin change on rising edge
    {
        volatile uint16_t buffer = TCNT1;
        uint16_t result;
        if (buffer < lastRPMstamp)
            result = buffer + (0xffff-lastRPMstamp);
        else
            result = buffer - lastRPMstamp;
        lastRPMstamp = buffer;
        result = 2*result/3-RPM[0];//Target signal width is 2/3.
        //if (abs(RPM[1] - result) < 50)
            RPM[1] = result;
    }
    else if((changed_pins & 0x02) && ((PINA & 0x02) == 0x02))//Speed pin change on rising edge
    {
        speed_enabled_watchdog = 10;
        volatile uint16_t buffer = TCNT1;
        uint16_t result;
        if (buffer < lastSpeedStamp)
            result = buffer + (0xffff-lastSpeedStamp);
        else
            result = buffer - lastSpeedStamp;
        lastSpeedStamp = buffer;
        speed = 7*result/16;//Target signal width is 7/16;
    }
}
