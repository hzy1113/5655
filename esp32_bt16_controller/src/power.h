#ifndef POWER_H
#define POWER_H

#include <Arduino.h>
#include "esp_sleep.h"
#include "config.h"

class PowerManager {
public:
    PowerManager();

    // 进入深度睡眠（最低功耗）
    void enterDeepSleep();

    // 浅睡眠（用于短暂休眠）
    void lightSleep(uint32_t ms);

    // 睡眠唤醒配置
    void configureWakeup();

    // 获取唤醒原因
    esp_sleep_wakeup_cause_t getWakeupReason() const;

    // 判断是否是首次启动
    bool isFirstBoot() const;

private:
    bool m_wakeupEnabled;
};

#endif // POWER_H
