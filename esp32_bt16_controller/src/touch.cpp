#include "touch.h"

TouchHandler::TouchHandler(AmplifierControl* amp)
    : m_amplifier(amp)
    , m_locked(false)
{
}

void TouchHandler::begin() {
    pinMode(TOUCH_PIN_1, INPUT);
    pinMode(TOUCH_PIN_2, INPUT);
}

void TouchHandler::update() {
    if (m_locked) {
        // 锁屏时只有模块1可用于解锁（快速按两下）
        handleModule(0);
        return;
    }

    handleModule(0);  // 模块1 - 播放/下一首
    handleModule(1);  // 模块2 - 音量+/音量-
}

void TouchHandler::handleModule(uint8_t index) {
    const uint8_t pin = (index == 0) ? TOUCH_PIN_1 : TOUCH_PIN_2;
    TouchModule& mod = m_modules[index];
    unsigned long now = millis();

    bool state = digitalRead(pin) == HIGH;

    if (state && !mod.pressed) {
        mod.pressed = true;
        mod.pressTime = now;
        mod.longPress1Triggered = false;
        mod.longPress2Triggered = false;

    } else if (!state && mod.pressed) {
        mod.pressed = false;
        unsigned long duration = now - mod.pressTime;

        if (duration < 500) {
            processTap(index);
        }
    }

    // 长按检测
    if (mod.pressed) {
        unsigned long duration = now - mod.pressTime;

        if (duration >= LONG_PRESS_TIME_2 && !mod.longPress2Triggered) {
            mod.longPress2Triggered = true;
            if (m_locked) {
                unlock();
            } else {
                lock();
            }
        }
        else if (duration >= LONG_PRESS_TIME_1 && !mod.longPress1Triggered) {
            mod.longPress1Triggered = true;
            if (index == 0) {
                // 模块1长按1秒：下一首
                m_amplifier->button1();
            } else {
                // 模块2长按1秒：音量-
                m_amplifier->button2();
            }
        }
    }
}

void TouchHandler::processTap(uint8_t index) {
    unsigned long now = millis();
    TouchModule& mod = m_modules[index];

    if (now - mod.lastTapTime < TAP_TIMEOUT) {
        mod.tapCount++;
    } else {
        mod.tapCount = 1;
    }
    mod.lastTapTime = now;

    if (m_locked) {
        // 锁屏时双击解锁
        if (mod.tapCount >= 2) {
            unlock();
            mod.tapCount = 0;
        }
        return;
    }

    // 正常工作模式
    if (index == 0) {
        // 模块1: 单击=播放/暂停, 双击=下一首
        switch (mod.tapCount) {
            case 1:
                m_amplifier->button1();
                break;
            case 2:
                // 双击下一首 - 发送两次脉冲
                m_amplifier->button1();
                delay(250);
                m_amplifier->button1();
                mod.tapCount = 0;
                break;
        }
        if (mod.tapCount >= 2) {
            mod.tapCount = 0;
        }
    } else {
        // 模块2: 单击=音量+
        if (mod.tapCount == 1) {
            m_amplifier->button2();
        }
        mod.tapCount = 0;
    }
}

void TouchHandler::lock() {
    m_locked = true;
    Serial.println("[SYSTEM] Locked");
}

void TouchHandler::unlock() {
    m_locked = false;
    Serial.println("[SYSTEM] Unlocked");

    for (int i = 0; i < 2; i++) {
        m_modules[i].tapCount = 0;
        m_modules[i].lastTapTime = 0;
        m_modules[i].longPress1Triggered = false;
        m_modules[i].longPress2Triggered = false;
    }
}
