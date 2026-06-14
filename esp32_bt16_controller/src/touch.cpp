#include "touch.h"

TouchHandler::TouchHandler(AmplifierControl* amp)
    : m_amplifier(amp)
    , m_locked(false)
    , m_lockUnlockTime(0)
{
    for (int i = 0; i < 6; i++) {
        m_touchStates[i] = false;
        m_touchChanged[i] = false;
        m_touchPressTime[i] = 0;
        m_lastTapTime[i] = 0;
        m_tapCount[i] = 0;
        m_longPressTriggered[i] = false;
        m_longPress2Triggered[i] = false;
    }
}

void TouchHandler::begin() {
    for (int i = 0; i < 6; i++) {
        pinMode(m_touchPins[i], INPUT);
    }
}

void TouchHandler::update() {
    if (m_locked) {
        // 锁屏模式下只响应LOCK键（解锁）
        bool lockState = digitalRead(TOUCH_PIN_LOCK) == HIGH;
        unsigned long now = millis();

        if (lockState && !m_touchStates[5]) {
            // LOCK键按下
            m_touchStates[5] = true;
            m_touchPressTime[5] = now;
        } else if (!lockState && m_touchStates[5]) {
            // LOCK键释放
            m_touchStates[5] = false;

            // 检测是否快速按两下
            if (now - m_lastTapTime[5] < TAP_TIMEOUT) {
                m_tapCount[5]++;
                if (m_tapCount[5] >= 2) {
                    unlock();
                    m_tapCount[5] = 0;
                }
            } else {
                m_tapCount[5] = 1;
            }
            m_lastTapTime[5] = now;
        }

        // 长按3秒也解锁
        if (m_touchStates[5] && (now - m_touchPressTime[5] >= LONG_PRESS_TIME_3)) {
            if (!m_longPressTriggered[5]) {
                m_longPressTriggered[5] = true;
                unlock();
            }
        } else {
            m_longPressTriggered[5] = false;
        }
        return;
    }

    // 正常工作模式
    for (int i = 0; i < 5; i++) {  // 0-4是功能键，不包括LOCK
        bool state = digitalRead(m_touchPins[i]) == HIGH;
        unsigned long now = millis();

        if (state && !m_touchStates[i]) {
            // 触摸按下
            m_touchStates[i] = true;
            m_touchPressTime[i] = now;
            m_longPressTriggered[i] = false;
            m_longPress2Triggered[i] = false;

        } else if (!state && m_touchStates[i]) {
            // 触摸释放
            m_touchStates[i] = false;

            // 判断是单击还是长按释放
            unsigned long pressDuration = now - m_touchPressTime[i];

            if (pressDuration < 500) {
                // 短按 - 处理点击事件
                processTap(i);
            }
            // 长按触发后释放不处理点击

        } else if (state && m_touchStates[i]) {
            // 持续触摸 - 检测长按
            unsigned long pressDuration = now - m_touchPressTime[i];

            // 长按2秒 - 开关机
            if (pressDuration >= LONG_PRESS_TIME_2 && !m_longPress2Triggered[i]) {
                m_longPress2Triggered[i] = true;
                if (i == 0 || i == 4) {  // PLAY_PAUSE 或 POWER
                    m_amplifier->powerToggle();
                }
            }
            // 长按1秒 - 音量+
            else if (pressDuration >= LONG_PRESS_TIME_1 && !m_longPressTriggered[i]) {
                m_longPressTriggered[i] = true;
                if (i == 2) {  // VOL_UP
                    m_amplifier->volumeUp();
                } else if (i == 3) {  // VOL_DOWN
                    m_amplifier->volumeDown();
                }
            }
        }
    }

    // 处理LOCK键（在非锁屏模式）
    bool lockState = digitalRead(TOUCH_PIN_LOCK) == HIGH;
    unsigned long now = millis();

    if (lockState && !m_touchStates[5]) {
        m_touchStates[5] = true;
        m_touchPressTime[5] = now;
        m_longPressTriggered[5] = false;
    } else if (!lockState && m_touchStates[5]) {
        m_touchStates[5] = false;

        unsigned long pressDuration = now - m_touchPressTime[5];

        if (pressDuration < 500) {
            // 短按 - 处理双击锁屏
            if (now - m_lastTapTime[5] < TAP_TIMEOUT) {
                m_tapCount[5]++;
                if (m_tapCount[5] >= 2) {
                    lock();
                    m_tapCount[5] = 0;
                }
            } else {
                m_tapCount[5] = 1;
            }
            m_lastTapTime[5] = now;
        }
    } else if (lockState && m_touchStates[5]) {
        // 长按3秒锁屏
        if (now - m_touchPressTime[5] >= LONG_PRESS_TIME_3 && !m_longPressTriggered[5]) {
            m_longPressTriggered[5] = true;
            lock();
        }
    }
}

void TouchHandler::processTap(uint8_t index) {
    unsigned long now = millis();

    // 双击检测
    if (now - m_lastTapTime[index] < TAP_TIMEOUT) {
        m_tapCount[index]++;
    } else {
        m_tapCount[index] = 1;
    }
    m_lastTapTime[index] = now;

    // 根据触摸次数处理
    switch (index) {
        case 0: // PLAY_PAUSE - 单击暂停/播放
            if (m_tapCount[index] == 1) {
                m_amplifier->playPause();
            }
            m_tapCount[index] = 0;
            break;

        case 1: // NEXT - 单击下一首
            if (m_tapCount[index] == 1) {
                m_amplifier->nextTrack();
            }
            m_tapCount[index] = 0;
            break;

        case 2: // VOL_UP - 单击音量+
        case 3: // VOL_DOWN - 单击音量-
            // 单击已在长按处理
            m_tapCount[index] = 0;
            break;

        default:
            m_tapCount[index] = 0;
            break;
    }
}

void TouchHandler::lock() {
    m_locked = true;
    m_lockUnlockTime = millis();
    Serial.println("[SYSTEM] Locked - Entering deep sleep mode");
}

void TouchHandler::unlock() {
    m_locked = false;
    m_lockUnlockTime = millis();
    Serial.println("[SYSTEM] Unlocked - Wake from deep sleep");

    // 重置所有触摸状态
    for (int i = 0; i < 6; i++) {
        m_tapCount[i] = 0;
        m_lastTapTime[i] = 0;
        m_longPressTriggered[i] = false;
        m_longPress2Triggered[i] = false;
    }
}
