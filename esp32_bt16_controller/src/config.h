#ifndef CONFIG_H
#define CONFIG_H

// ==================== 引脚定义 ====================
// TTP223 触摸模块引脚
#define TOUCH_PIN_1    4   // 触摸模块1
#define TOUCH_PIN_2    5   // 触摸模块2

// BT-16 功放板控制引脚 (低电平触发)
#define AMP_PIN_PLAY_PAUSE   0   // 播放/暂停
#define AMP_PIN_NEXT         1   // 下一首
#define AMP_PIN_VOL_UP       2   // 音量+
#define AMP_PIN_VOL_DOWN     3   // 音量-
#define AMP_PIN_POWER        21  // 开关机

// ==================== 时间参数 ====================
// 触摸时间阈值 (ms)
#define TAP_TIMEOUT          400     // 两次触摸判定为双击的最大间隔
#define TRIPLE_TAP_TIMEOUT   600     // 三击判定间隔
#define LONG_PRESS_TIME_1     1000    // 长按1s - 音量+
#define LONG_PRESS_TIME_2     2000    // 长按2s - 开关机
#define LONG_PRESS_TIME_3     3000    // 长按3s - 锁屏/解锁

// ==================== 状态定义 ====================
enum class AmplifierState {
    OFF,
    ON,
    PLAYING,
    PAUSED
};

enum class SystemMode {
    NORMAL,    // 正常工作
    LOCKED     // 锁屏（深度睡眠）
};

#endif // CONFIG_H
