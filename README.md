# BYSD_N2021_MCU

车载终端 MCU 业务应用程序（无智能补电）。源码在 `Source/`，只含业务逻辑；驱动与操作系统不在本仓库。

## 功能

- GB/T 32960 数据采集与上报（`Source/apps/gb32960`）
- 平台协议（东风、地上铁、中车、石家庄等，`Source/apps` 下各平台目录）
- OBD：J1939、UDS、ISO 15765（`Source/apps/OBD`）
- OTA：三电及相关控制器（`Source/apps/san_dian_ota`）

## 驱动接口

业务通过 `Source/sysapi` 调用已烧录在片内的驱动 API。接口表基址为 `DRIVER_API_BASE_ADDR`，定义在 `Source/sysapi/sysapi.h`：

```c
#define DRIVER_API_BASE_ADDR 0x00004034
```

## 编译

用 Keil MDK 打开 `KeilMDK/KeilMDK.uvproj`（器件 Cortex-M3）。列表文件在 `KeilMDK/lst/`，编译产物在 `KeilMDK/output/`。这两处以及 Source Insight 工程库 `SI/` 不入库。

## 版本记录

变更说明见 [Source/readme.txt](Source/readme.txt)。

## 发布包

固件与安装包请从 GitHub Releases 获取（例如 `BYSD_N2021_MCU_*.zip`）。不要把发布压缩包放进 `doc/` 或提交到本仓库。
