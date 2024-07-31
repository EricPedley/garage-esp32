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
    int timeMs = millis();
    if(state==State::IN_BETWEEN_BLINKS) {
        if(timeMs - lastPeriod > blinkPeriod) {
            state = IN_BLINK;
            lastPeriod = timeMs;
            blinks = blinkCount;
        }
    }

    // these both can run in the same function call, and it's on purpose.
    if (state==State::IN_BLINK) {
        if(timeMs - lastBlink > blinkLength) {
            digitalWrite(pin, !isOn);
            isOn = !isOn;
            lastBlink = timeMs;
            if(!isOn) {
                blinks--;
            }
            if(blinks == 0) {
                state = IN_BETWEEN_BLINKS;
            }
        }
    }
}