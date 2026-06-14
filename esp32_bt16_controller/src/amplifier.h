#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include <Arduino.h>
#include "config.h"

class AmplifierControl {
public:
    AmplifierControl();

    void begin();

    // 控制功放板
    void playPause();      // 播放/暂停
    void nextTrack();      // 下一首
    void volumeUp();       // 音量+
    void volumeDown();     // 音量-
    void powerToggle();    // 开关机

    // 状态查询
    AmplifierState getState() const { return m_state; }
    void setState(AmplifierState state) { m_state = state; }

private:
    AmplifierState m_state;
    unsigned long m_lastControlTime;

    void sendPulse(uint8_t pin, uint16_t duration_ms = 200);
};

#endif // AMPLIFIER_H
