#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include <Arduino.h>
#include "config.h"

class AmplifierControl {
public:
    AmplifierControl();

    void begin();

    // 控制BT-16
    void button1();      // 按钮1 - 短按播放/暂停，长按下一首
    void button2();      // 按钮2 - 短按音量+，长按音量-
    void powerToggle();  // 开关机

    AmplifierState getState() const { return m_state; }
    void setState(AmplifierState state) { m_state = state; }

private:
    AmplifierState m_state;
    void sendPulse(uint8_t pin, uint16_t duration_ms = 200);
};

#endif // AMPLIFIER_H
