#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>
#include "config.h"
#include "amplifier.h"

class TouchHandler {
public:
    TouchHandler(AmplifierControl* amp);

    void begin();
    void update();

    void lock();
    void unlock();
    bool isLocked() const { return m_locked; }

private:
    AmplifierControl* m_amplifier;

    struct TouchModule {
        bool pressed = false;
        unsigned long pressTime = 0;
        unsigned long lastTapTime = 0;
        uint8_t tapCount = 0;
        bool longPress1Triggered = false;
        bool longPress2Triggered = false;
    };

    TouchModule m_modules[2];
    bool m_locked;

    void handleModule(uint8_t index);
    void processTap(uint8_t index);
};

#endif // TOUCH_H
