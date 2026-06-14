# ESP32C3 触摸蓝牙功放控制器

使用ESP32C3 + 2个TTP223触摸模块 控制大吴BT-16蓝牙功放板。

## 功能

- **单击**：暂停/播放
- **双击**：下一首
- **三击**：锁屏
- **长按1秒**：音量+/-（模块1音量+，模块2音量-）
- **长按2秒**：开关机（仅模块2）
- **长按3秒**：锁屏
- **锁屏状态下快速按两下**：解锁

## 硬件

- ESP32-C3-MINI
- TTP223触摸模块 x2
- 大吴BT-16蓝牙功放板
- 磁吸充电模块

## 接线

```
ESP32-C3    TTP223      BT-16
--------    ------      -----
GPIO4  ◄─── TP1
GPIO5  ◄─── TP2
GPIO0  ────         ► PLAY_PAUSE
GPIO1  ────         ► NEXT
GPIO2  ────         ► VOL_UP
GPIO3  ────         ► VOL_DOWN
GPIO21 ────         ► POWER
3.3V   ──── VCC
GND    ──── GND     GND
```

## 开发

```bash
pio run      # 编译
pio run -t upload  # 上传
pio device monitor  # 串口监视
```
