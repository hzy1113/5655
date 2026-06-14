#include "amplifier.h"

AmplifierControl::AmplifierControl()
    : m_state(AmplifierState::OFF)
{
}

void AmplifierControl::begin() {
    pinMode(AMP_PIN_1, OUTPUT);
    pinMode(AMP_PIN_2, OUTPUT);
    digitalWrite(AMP_PIN_1, HIGH);
    digitalWrite(AMP_PIN_2, HIGH);
}

void AmplifierControl::sendPulse(uint8_t pin, uint16_t duration_ms) {
    digitalWrite(pin, LOW);
    delay(duration_ms);
    digitalWrite(pin, HIGH);
}

void AmplifierControl::button1() {
    // BT-16按钮1：短按播放/暂停，长按下一首
    // 这里发送单次脉冲触发播放/暂停
    sendPulse(AMP_PIN_1);
    m_state = (m_state == AmplifierState::PLAYING) ?
              AmplifierState::PAUSED : AmplifierState::PLAYING;
}

void AmplifierControl::button2() {
    // BT-16按钮2：短按音量+
    sendPulse(AMP_PIN_2);
}

void AmplifierControl::powerToggle() {
    // 开关机需要较长脉冲
    sendPulse(AMP_PIN_1, 500);
    m_state = (m_state == AmplifierState::OFF) ?
              AmplifierState::ON : AmplifierState::OFF;
}
