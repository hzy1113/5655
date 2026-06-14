#include "power.h"

PowerManager::PowerManager()
    : m_wakeupEnabled(false)
{
}

void PowerManager::configureWakeup() {
    // 配置EXT0唤醒（根据触摸模块电平变化唤醒）
    // 注意：ESP32C3支持GPIO唤醒
    esp_err_t err = esp_sleep_enable_ext0_wakeup(
        (gpio_num_t)TOUCH_PIN_LOCK,  // 使用LOCK引脚作为唤醒源
        1  // 高电平唤醒
    );

    if (err == ESP_OK) {
        m_wakeupEnabled = true;
        Serial.println("[POWER] Wakeup configured: EXT0 on LOCK pin");
    } else {
        Serial.println("[POWER] Failed to configure wakeup");
    }
}

void PowerManager::enterDeepSleep() {
    Serial.println("[POWER] Entering deep sleep mode...");

    // 配置唤醒源
    configureWakeup();

    // 进入深度睡眠
    esp_deep_sleep_start();
}

void PowerManager::lightSleep(uint32_t ms) {
    Serial.printf("[POWER] Light sleep for %ums\n", ms);
    esp_sleep_enable_timer_wakeup(ms * 1000);  // 微秒
    esp_light_sleep_start();
}

esp_sleep_wakeup_cause_t PowerManager::getWakeupReason() const {
    return esp_sleep_get_wakeup_cause();
}

bool PowerManager::isFirstBoot() const {
    return getWakeupReason() == ESP_SLEEP_WAKEUP_UNDEFINED;
}
