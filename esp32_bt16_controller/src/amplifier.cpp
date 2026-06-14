#include "amplifier.h"

AmplifierControl::AmplifierControl()
    : m_state(AmplifierState::OFF)
    , m_lastControlTime(0)
{
}

void AmplifierControl::begin() {
    // 设置所有控制引脚为输出
    pinMode(AMP_PIN_PLAY_PAUSE, OUTPUT);
    pinMode(AMP_PIN_NEXT, OUTPUT);
    pinMode(AMP_PIN_VOL_UP, OUTPUT);
    pinMode(AMP_PIN_VOL_DOWN, OUTPUT);
    pinMode(AMP_PIN_POWER, OUTPUT);

    // 初始化为高电平（未触发）
    digitalWrite(AMP_PIN_PLAY_PAUSE, HIGH);
    digitalWrite(AMP_PIN_NEXT, HIGH);
    digitalWrite(AMP_PIN_VOL_UP, HIGH);
    digitalWrite(AMP_PIN_VOL_DOWN, HIGH);
    digitalWrite(AMP_PIN_POWER, HIGH);
}

void AmplifierControl::sendPulse(uint8_t pin, uint16_t duration_ms) {
    // BT-16 功放板低电平触发
    digitalWrite(pin, LOW);
    delay(duration_ms);
    digitalWrite(pin, HIGH);
}

void AmplifierControl::playPause() {
    sendPulse(AMP_PIN_PLAY_PAUSE);
    m_lastControlTime = millis();

    if (m_state == AmplifierState::PLAYING) {
        m_state = AmplifierState::PAUSED;
    } else {
        m_state = AmplifierState::PLAYING;
    }
}

void AmplifierControl::nextTrack() {
    sendPulse(AMP_PIN_NEXT);
    m_lastControlTime = millis();
}

void AmplifierControl::volumeUp() {
    sendPulse(AMP_PIN_VOL_UP);
    m_lastControlTime = millis();
}

void AmplifierControl::volumeDown() {
    sendPulse(AMP_PIN_VOL_DOWN);
    m_lastControlTime = millis();
}

void AmplifierControl::powerToggle() {
    sendPulse(AMP_PIN_POWER, 500);  // 开关机需要较长脉冲
    m_lastControlTime = millis();

    if (m_state == AmplifierState::OFF) {
        m_state = AmplifierState::ON;
    } else {
        m_state = AmplifierState::OFF;
    }
}
