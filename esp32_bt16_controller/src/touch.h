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

    // 模式切换
    void lock();
    void unlock();
    bool isLocked() const { return m_locked; }

private:
    AmplifierControl* m_amplifier;

    // 触摸模块状态
    struct TouchModule {
        bool pressed = false;
        unsigned long pressTime = 0;
        unsigned long lastTapTime = 0;
        uint8_t tapCount = 0;
        bool longPress1Triggered = false;  // 1s
        bool longPress2Triggered = false;  // 2s
        bool longPress3Triggered = false;  // 3s
    };

    TouchModule m_modules[2];  // 2个触摸模块

    // 锁屏状态
    bool m_locked;
    unsigned long m_lockUnlockTime;

    void handleModule(uint8_t index);
    void processTap(uint8_t index);
};

#endif // TOUCH_H
