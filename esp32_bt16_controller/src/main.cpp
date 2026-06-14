#include <Arduino.h>
#include "config.h"
#include "amplifier.h"
#include "touch.h"

// 全局对象
AmplifierControl amplifier;
TouchHandler touchHandler(&amplifier);

// 触摸引脚中断辅助
volatile bool touchFlag = false;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("========== ESP32C3 BT-16 Controller ==========");
    Serial.println("[BOOT] Initializing...");

    // 初始化功放控制
    amplifier.begin();
    Serial.println("[BOOT] Amplifier control initialized");

    // 初始化触摸处理
    touchHandler.begin();
    Serial.println("[BOOT] Touch handler initialized");

    // 检查是否从深度睡眠唤醒
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
        Serial.println("[BOOT] Woke up from deep sleep (EXT0)");
        touchHandler.unlock();
    } else if (wakeup_reason != ESP_SLEEP_WAKEUP_UNDEFINED) {
        Serial.println("[BOOT] Woke up from deep sleep (other reason)");
    } else {
        Serial.println("[BOOT] Normal startup");
    }

    // 开启蓝牙前先确保功放开启
    amplifier.setState(AmplifierState::ON);

    Serial.println("[BOOT] System ready");
    Serial.println("==========================================");
}

void loop() {
    // 处理触摸事件
    touchHandler.update();

    // 检查是否需要进入深度睡眠（锁屏超过一定时间）
    if (touchHandler.isLocked()) {
        // 锁屏模式下，让CPU进入空闲状态
        delay(10);
    } else {
        // 正常工作模式
        delay(10);
    }
}

// 外部中断服务（可选，用于快速响应触摸）
void IRAM_ATTR touchISR() {
    touchFlag = true;
}
