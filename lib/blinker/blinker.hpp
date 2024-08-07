// header file for blinker class in blinker.cpp

#pragma once
class Blinker {
public:
    Blinker(int pin): pin(pin) {};
    void Init();
    void tick();
    /**
     * @param period - period of blinking in ms (default 1000). 0 means solid on.
    */
    void setBlinkPeriod(int period) {
        blinkPeriod = period;
    }
    void setBlinkCount(int count) {
        blinkCount = count;
    }
    void setBlinkLength(int length) {
        blinkLength = length;
    }
private:
    int blinkPeriod = 1000;
    int blinkLength = 100;
    int blinkCount = 1;

    int pin;
    enum State {
        IN_BETWEEN_BLINKS,
        IN_BLINK
    } state = IN_BETWEEN_BLINKS;
    int lastBlink = 0;
    int lastPeriod = 0;
    int blinks = 0;
    bool isOn = false;
};