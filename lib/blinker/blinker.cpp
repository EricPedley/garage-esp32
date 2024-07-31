// class that has a tick() method that gets called once per loop,
// and methods to set the period and length of the blink, and how many
// consecutive blinks per period.
#include <Arduino.h>
#include "blinker.hpp"


void Blinker::Init() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Blinker::tick() {
    if(state==State::IN_BETWEEN_BLINKS) {
        if(millis() - lastBlink > blinkPeriod) {
            state = IN_BLINK;
            lastBlink = millis();
            blinks = blinkCount;
        }
    } else if (state==State::IN_BLINK) {
        if(millis() - lastBlink > blinkLength) {
            int isOn = digitalRead(pin);
            digitalWrite(pin, !isOn);
            lastBlink = millis();
            if(isOn) {
                blinks--;
            }
            if(blinks == 0) {
                state = IN_BETWEEN_BLINKS;
            }
        }
    }
}