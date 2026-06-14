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

    // 触摸引脚状态
    bool m_touchStates[6];
    bool m_touchChanged[6];
    unsigned long m_touchPressTime[6];

    // 双击检测
    unsigned long m_lastTapTime[6];
    uint8_t m_tapCount[6];

    // 长按检测
    bool m_longPressTriggered[6];
    bool m_longPress2Triggered[6];

    // 锁屏状态
    bool m_locked;
    unsigned long m_lockUnlockTime;

    // 引脚映射
    const uint8_t m_touchPins[6] = {
        TOUCH_PIN_PLAY_PAUSE,
        TOUCH_PIN_NEXT,
        TOUCH_PIN_VOL_UP,
        TOUCH_PIN_VOL_DOWN,
        TOUCH_PIN_POWER,
        TOUCH_PIN_LOCK
    };

    void handleTouchEvent(uint8_t index);
    void processTap(uint8_t index);
    void processLongPress(uint8_t index);
};

#endif // TOUCH_H
