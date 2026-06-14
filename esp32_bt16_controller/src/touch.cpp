#include "touch.h"

TouchHandler::TouchHandler(AmplifierControl* amp)
    : m_amplifier(amp)
    , m_locked(false)
    , m_lockUnlockTime(0)
{
}

void TouchHandler::begin() {
    pinMode(TOUCH_PIN_1, INPUT);
    pinMode(TOUCH_PIN_2, INPUT);
}

void TouchHandler::update() {
    if (m_locked) {
        handleModule(0);  // 模块1用于解锁
        return;
    }

    handleModule(0);  // 模块1 - 播放控制
    handleModule(1);  // 模块2 - 音量/开关
}

void TouchHandler::handleModule(uint8_t index) {
    const uint8_t pin = (index == 0) ? TOUCH_PIN_1 : TOUCH_PIN_2;
    TouchModule& mod = m_modules[index];
    unsigned long now = millis();

    bool state = digitalRead(pin) == HIGH;

    if (state && !mod.pressed) {
        // 触摸按下
        mod.pressed = true;
        mod.pressTime = now;
        mod.longPress1Triggered = false;
        mod.longPress2Triggered = false;
        mod.longPress3Triggered = false;

    } else if (!state && mod.pressed) {
        // 触摸释放
        mod.pressed = false;
        unsigned long duration = now - mod.pressTime;

        if (duration < 500) {
            // 短按 - 处理点击事件
            processTap(index);
        }
    }

    // 持续触摸 - 长按检测
    if (mod.pressed) {
        unsigned long duration = now - mod.pressTime;

        // 长按3秒 - 锁屏/解锁
        if (duration >= LONG_PRESS_TIME_3 && !mod.longPress3Triggered) {
            mod.longPress3Triggered = true;
            lock();
        }
        // 长按2秒 - 开关机 (仅模块2)
        else if (duration >= LONG_PRESS_TIME_2 && !mod.longPress2Triggered && index == 1) {
            mod.longPress2Triggered = true;
            m_amplifier->powerToggle();
        }
        // 长按1秒 - 音量+ (模块1) 或 音量- (模块2)
        else if (duration >= LONG_PRESS_TIME_1 && !mod.longPress1Triggered) {
            mod.longPress1Triggered = true;
            if (index == 0) {
                m_amplifier->volumeUp();
            } else {
                m_amplifier->volumeDown();
            }
        }
    }
}

void TouchHandler::processTap(uint8_t index) {
    unsigned long now = millis();
    TouchModule& mod = m_modules[index];

    // 判断点击次数
    if (now - mod.lastTapTime < TAP_TIMEOUT) {
        mod.tapCount++;
    } else {
        mod.tapCount = 1;
    }
    mod.lastTapTime = now;

    if (index == 0) {
        // 模块1: 单击=暂停/播放, 双击=下一首, 三击=锁屏
        switch (mod.tapCount) {
            case 1:
                m_amplifier->playPause();
                break;
            case 2:
                m_amplifier->nextTrack();
                break;
            case 3:
                lock();
                break;
        }
        if (mod.tapCount >= 3) {
            mod.tapCount = 0;
        }
    } else {
        // 模块2: 单击=暂停/播放, 双击=下一首
        switch (mod.tapCount) {
            case 1:
                m_amplifier->playPause();
                break;
            case 2:
                m_amplifier->nextTrack();
                break;
        }
        if (mod.tapCount >= 2) {
            mod.tapCount = 0;
        }
    }
}

void TouchHandler::lock() {
    m_locked = true;
    m_lockUnlockTime = millis();
    Serial.println("[SYSTEM] Locked - Entering deep sleep");
}

void TouchHandler::unlock() {
    m_locked = false;
    m_lockUnlockTime = millis();
    Serial.println("[SYSTEM] Unlocked - Wake from deep sleep");

    // 重置状态
    for (int i = 0; i < 2; i++) {
        m_modules[i].tapCount = 0;
        m_modules[i].lastTapTime = 0;
        m_modules[i].longPress1Triggered = false;
        m_modules[i].longPress2Triggered = false;
        m_modules[i].longPress3Triggered = false;
    }
}
