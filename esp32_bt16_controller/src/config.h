#ifndef CONFIG_H
#define CONFIG_H

// ==================== 引脚定义 ====================
// TTP223 触摸模块引脚
#define TOUCH_PIN_1    4   // 触摸模块1 - 播放控制
#define TOUCH_PIN_2    5   // 触摸模块2 - 音量控制

// BT-16 功放板控制引脚 (低电平触发)
#define AMP_PIN_1      0   // BT-16按钮1 (播放/暂停/下一首)
#define AMP_PIN_2      1   // BT-16按钮2 (音量+/音量-)

// ==================== 时间参数 ====================
#define TAP_TIMEOUT          400     // 两次触摸判定间隔
#define LONG_PRESS_TIME_1    1000    // 长按1s - 音量+/音量-
#define LONG_PRESS_TIME_2    2000    // 长按2s - 锁屏

// ==================== 状态定义 ====================
enum class AmplifierState {
    OFF,
    ON,
    PLAYING,
    PAUSED
};

#endif // CONFIG_H
