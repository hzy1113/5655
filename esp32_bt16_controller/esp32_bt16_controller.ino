/*
 * ESP32C3 触摸蓝牙功放控制器
 * 使用2个TTP223触摸模块控制大吴BT-16蓝牙功放板
 */

// ==================== 引脚定义 ====================
#define TOUCH_PIN_1    4   // 触摸模块1 - 播放控制
#define TOUCH_PIN_2    5   // 触摸模块2 - 音量控制

#define AMP_PIN_1      0   // BT-16按钮1 (播放/暂停/下一首)
#define AMP_PIN_2      1   // BT-16按钮2 (音量+/音量-)

// ==================== 时间参数 ====================
#define TAP_TIMEOUT          400     // 两次触摸判定间隔
#define LONG_PRESS_TIME_1    1000    // 长按1s - 音量+/下一首
#define LONG_PRESS_TIME_2    2000    // 长按2s - 锁屏

// ==================== 状态定义 ====================
enum class AmplifierState {
    OFF,
    ON,
    PLAYING,
    PAUSED
};

// ==================== 全局变量 ====================
AmplifierState ampState = AmplifierState::OFF;

struct TouchModule {
    bool pressed = false;
    unsigned long pressTime = 0;
    unsigned long lastTapTime = 0;
    uint8_t tapCount = 0;
    bool longPress1Triggered = false;
    bool longPress2Triggered = false;
};

TouchModule modules[2];
bool locked = false;

// ==================== 功放控制 ====================
void ampBegin() {
    pinMode(AMP_PIN_1, OUTPUT);
    pinMode(AMP_PIN_2, OUTPUT);
    digitalWrite(AMP_PIN_1, HIGH);
    digitalWrite(AMP_PIN_2, HIGH);
}

void ampSendPulse(uint8_t pin, uint16_t duration_ms = 200) {
    digitalWrite(pin, LOW);
    delay(duration_ms);
    digitalWrite(pin, HIGH);
}

void ampButton1() {
    // BT-16按钮1：播放/暂停/下一首
    ampSendPulse(AMP_PIN_1);
    ampState = (ampState == AmplifierState::PLAYING) ?
              AmplifierState::PAUSED : AmplifierState::PLAYING;
}

void ampButton2() {
    // BT-16按钮2：音量+
    ampSendPulse(AMP_PIN_2);
}

// ==================== 触摸处理 ====================
void touchBegin() {
    pinMode(TOUCH_PIN_1, INPUT);
    pinMode(TOUCH_PIN_2, INPUT);
}

void processTap(uint8_t index) {
    unsigned long now = millis();
    TouchModule& mod = modules[index];

    if (now - mod.lastTapTime < TAP_TIMEOUT) {
        mod.tapCount++;
    } else {
        mod.tapCount = 1;
    }
    mod.lastTapTime = now;

    if (locked) {
        // 锁屏时双击解锁
        if (mod.tapCount >= 2) {
            locked = false;
            Serial.println("[SYSTEM] Unlocked");
            mod.tapCount = 0;
        }
        return;
    }

    // 正常工作模式
    if (index == 0) {
        // 模块1: 单击=播放/暂停, 双击=下一首
        switch (mod.tapCount) {
            case 1:
                ampButton1();
                break;
            case 2:
                ampButton1();
                delay(250);
                ampButton1();
                mod.tapCount = 0;
                break;
        }
        if (mod.tapCount >= 2) {
            mod.tapCount = 0;
        }
    } else {
        // 模块2: 单击=音量+
        if (mod.tapCount == 1) {
            ampButton2();
        }
        mod.tapCount = 0;
    }
}

void handleModule(uint8_t index) {
    const uint8_t pin = (index == 0) ? TOUCH_PIN_1 : TOUCH_PIN_2;
    TouchModule& mod = modules[index];
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
            if (locked) {
                locked = false;
                Serial.println("[SYSTEM] Unlocked");
            } else {
                locked = true;
                Serial.println("[SYSTEM] Locked");
            }
        }
        else if (duration >= LONG_PRESS_TIME_1 && !mod.longPress1Triggered) {
            mod.longPress1Triggered = true;
            if (index == 0) {
                // 模块1长按1秒：下一首
                ampButton1();
            } else {
                // 模块2长按1秒：音量-
                ampButton2();
            }
        }
    }
}

void touchUpdate() {
    if (locked) {
        handleModule(0);  // 锁屏时只有模块1用于解锁
        return;
    }

    handleModule(0);  // 模块1 - 播放/下一首
    handleModule(1);  // 模块2 - 音量+/音量-
}

// ==================== 主程序 ====================
void setup() {
    Serial.begin(115200);
    Serial.println("========== ESP32C3 BT-16 Controller ==========");

    ampBegin();
    touchBegin();

    Serial.println("[BOOT] Ready");
}

void loop() {
    touchUpdate();
    delay(10);
}
