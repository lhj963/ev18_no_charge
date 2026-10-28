# PR #2 回归影响评审（只读，无源码改动）

对照：`main` `0ee2826`（Stop tracking Keil output, Source Insight, and release zips）与 [PR #2](https://github.com/lhj963/ev18_no_charge/pull/2) 头 `7e54aeb`（`cursor/audit-critical-high-fixes-61f1`）。

PR 分支从 `a4de756` 拉出。`a4de756..main` 只停止跟踪 Keil / Source Insight / 发布 zip，`Source/` 与 `main` 一致。下面的行为差就是 `7e54aeb` 这一笔提交（17 个源文件，+1085 / −455）。

约束：SM2 业务未使用；原有业务不能被打断。分类只用三档：**Safe** / **Compatible-with-caveat** / **Breaking**。

## 总评

**Breaking（范围限定在平台远程 wget）。** 其余日常链路在下面写明的前提下仍走原函数。

按「不得破坏原业务」：默认 `ENABLE_PLATFORM_REMOTE_WGET=0` 会停掉平台触发的 TBOX 升级包和三电升级包下载。这条链路写在正式平台解析里（链路 0 的 `0xA0` / `0xA5` / `0x92`，国六/HJ 的 `0x95`/`0x52`），不是调试宏包起来的死代码。MPU 已经下载完成后再通知 MCU 刷 CAN 的路径不受这个宏影响。

登录、实时上报、锁车/绑定、已登录后的远程启动/车门、DLC=8 的 CAN 信号、芯片在位时的 SM2，在「对端 BCC 与终端发包算法一致」时仍按原逻辑执行。仓库里三段现成 `##` 样例帧的 BCC 与新校验一致。

| 项 | 分类 |
| --- | --- |
| 1. BCC 必须正确 | Compatible-with-caveat |
| 2. 平台 wget 默认关闭 | Breaking |
| 3. `0xA7` 需要登录标志 | Compatible-with-caveat |
| 4. 解析长度 | Compatible-with-caveat |
| 5. SM2 不在位返回 0 | Safe |
| 6. CAN 真实 DLC | Compatible-with-caveat |
| 7. 报警测试掩码默认 0 | Compatible-with-caveat |
| 8. OTA 偏移 / ISO-TP | Compatible-with-caveat |

## 1. BCC 必须正确 — Compatible-with-caveat

`gb32960_ack_plat`、`sddf_ack_plat`、`dst_ack_plat`、`zhongche_analyze` 都改走 `gb32960_locate_frame`。校验是 `XOR_Check(帧+2, GB32960_CONTENT_OFFSET + 数据单元长度 - 2)`，和 `_xDataBag` / 登录封包里的 `XOR_Check(pbuf+2, len-2)` 是同一段：从命令字节到数据单元末字节，不含起始符 `##`，不含 BCC 本身。这就是 GB/T 32960 的 BCC。

旧代码把 BCC 读出来只打印，比较结果不参与分发。长度够的帧一律进入 `gb32960_deal` / `sddf_deal` / `dst_gb32960_deal` / `zhongche_deal`。

新代码：

- 算出的 BCC 与帧内字节相同：仍调用原来的 `deal`。命令字、应答标志、VIN、加密方式、数据单元指针都按原偏移取出。
- BCC 不同：打日志后跳过该帧，不调用 `deal`，也不回业务成功。同一缓冲里后面 BCC 正确的帧继续解析。
- 长度不完整：和旧代码一样停在当前缓冲，不把半帧当成一帧。

仓库内三帧按新算法复核，全部吻合（数据单元长度字段与缓冲长度也吻合）：

| 样例 | 位置 | 数据单元长度 | 帧长 | BCC |
| --- | --- | --- | --- | --- |
| `test_sd_mcu_ota` 的 `m[]` | `usercmd.c`，喂给 `gb32960_ack_plat` | 174 | 199 | `0xD4` 匹配 |
| 同命令的 `m1[]` | 同上 | 174 | 199 | `0x6C` 匹配 |
| `senddata` 的实时样例 | `usercmd.c` | 250 | 275 | `0xC2` 匹配 |

因此：以前就按国标/按本终端发包算法组帧的平台，登录应答、实时应答、参数、锁车、`0xA0`/`0xA5`/`0xA7`/`0x92` 仍会进原处理函数。以前靠「BCC 错误也被执行」才能工作的对端，现在整帧被丢弃。

登录应答若被丢掉，`set_gb32960_login_ok_flag(linkid, 1)` 不会发生。`collect_one_32960_pack` 在标志为 0 时直接返回，实时与补发都不再上报。错误 BCC 的影响面是整条会话，不只是丢一帧垃圾。

链路 4 仍对同一缓冲先调 `gb32960_ack_plat` 再调 `dst_ack_plat`。两边现在都验 BCC，正确帧仍会被两边各处理一次，这和改前相同。

安全上这是收紧。兼容上只有「对端 BCC 本来就对」的平台保持原业务。

## 2. 平台 wget 默认关闭 — Breaking

`Config.h` 与 `func.h` 都把 `ENABLE_PLATFORM_REMOTE_WGET` 默认定义为 0。为 0 时 `platform_remote_wget_allowed()` 直接返回 0，白名单不会执行。

被拦住的只有下面几处 `S_MSG_EXECUTE_DRV_CMD`（命令形态都是 `ATEX=<linuxcmd:wget -O /data/...>`）：

| 入口 | 命令 | 下载文件 | 原业务 |
| --- | --- | --- | --- |
| `ota_ctrl`，`0xA5` 且子命令 `0x52` | 链路 0 | `/data/box.bin` | TBOX 远程升级包 |
| `ota_ctrl`，`0xA0` 且子命令 `0xC4` | 链路 0 | `sandian_mcu.zip` / `sandian_bms.zip` / `sandian_vcu.zip`（源地址 `0xEE` / `0xC8` / `0xEF`） | 三电包下载 |
| `gb32960_deal` 的 `0x92` / `0xC8` | 链路 0 | `/data/box.bin`（ftp 用户口令或 http） | TBOX 升级；成功时还会把 `mcu_start_update_flag` 置 1，主循环停止 `collect_data_proc` |
| `anlyze_17691_server_data` 的 `0x95` / `0x52` | 链路 2、链路 5 | `/data/box.bin` | 国六/HJ TSP OTA |

三电的后续关系：平台 `0xA0` 让 MPU 去 wget；MPU 下载完用 `user_mpu_msg_arrive` 的 msgid 1 / 2 / 4 通知 MCU，MCU 才调用 `sandian_mcu_start_ota_step` / `sandian_vcu1_start_ota_step` / `sandian_bms1_start_ota_step1`。wget 不执行，这三路通知不会来，CAN 刷写不会开始。树内没有第二处拼这些 zip 或 `box.bin` 的 wget。

仍会执行、且与改前相同的部分：

- `0xA0`/`0xA5` 在 wget 判断之前已经用 `_xDataBag` 回了成功应答（`ack[12]=1`）。平台会看到命令已被接受，文件实际不会落地。
- `0xA0` 在 wget 之前仍会 `sandian_set_update_type/way/lingjian_id`。仪表侧升级对象会被置上，CAN 刷写要等 MPU 的下载完成消息。
- `0x92` 在 wget 被拒绝时不会置 `mcu_start_update_flag`，实时采集不会因为一次被拒绝的升级命令停掉。
- 短信到达仍把短信正文交给 `S_MSG_EXECUTE_DRV_CMD`（`sms_arrive`）。定时重启仍发 `ATWST`。这两处不经过新宏。
- MPU 已经发来 msgid 1/2/4 时，CAN OTA 状态机照旧启动。

宏改为 1 之后，白名单只接受：

`ATEX=<linuxcmd:wget -O /data/{box.bin,sandian_mcu.zip,sandian_bms.zip,sandian_vcu.zip} {http|ftp}://…>`

URL 字符集是字母数字和 `. - _ / : ? = % ~ @`，长度 8..240，且以 `>` 结尾。`usercmd.c` 里那条 `ftp://ftp_cwjh_by_mfu:…@ev.cwjh.cn:30021/gb32960/mcu/….zip` 的字符都在集合内，打开宏后这类 URL 可以通过。含 `&`、空格或其它字符的 URL 即使打开宏也会被拒绝。默认编译下这一条不发生，因为函数在宏为 0 时直接返回 0。

结论：日常登录和实时上报不依赖 wget。平台侧 TBOX 升级和三电包下载依赖 wget，默认编译下这两条原流程停止。

## 3. `0xA7` 需要登录标志 — Compatible-with-caveat

`0xA7` 只在 `gb32960_deal` 里进入 `driver_ctrl`。子命令 `0x61` 是远程启动（`set_want_startup_status`），`0x62` 是车门（`set_want_lockdoor_status`）。注释写成「TBOX OTA」，函数体是座舱控制。

新代码在读子命令之前增加 `get_gb32960_login_ok_flag(linkid)==0` 则直接返回。返回点在成功应答之前，所以未登录时：不改启动/车门状态，也不给平台回 `ack[12]=1`。

登录标志只在本链路收到命令 `0x01` 且应答标志 `0x01` 时置 1（`gb32960_deal` 的登录分支）。登出、断链（`pub.c` 里对标志清 0）之后为 0。链路 0 的平台数据都进 `gb32960_ack_plat(0, …)`，登录应答和后来的 `0xA7` 使用同一个 linkid。

因此：本链路登录应答已经被接受之后，`0x61`/`0x62` 的判断、CAN 条件和成功应答与改前相同。登录应答本身还要先过第 1 项的 BCC。

会停掉的情况：登录成功之前、登出或断链之后、以及标志不在这条 linkid 上时，平台下发的启动/车门被忽略。东风股份商研院路径（`dfgf_sy` 的 `set_want_startup_status` / `set_want_lockdoor_status`）不经过 `0xA7`，本次没有改。

## 4. 解析长度 — Compatible-with-caveat

单独一帧、声明长度与缓冲一致、校验与封包算法一致时，短帧（含数据单元长度为 0）仍被接受。新长度判断没有把「合法短帧」整类拒掉。变化在校验从无到有，以及假帧头后面的重同步。

**GB32960 / 中车 / 地上铁。** `GB32960_CONTENT_OFFSET` 为 24，`GB32960_MIN_PACK_LEN` 为 25。数据单元长度为 0 时整帧 25 字节，`need = 24 + 0 + 1`。缓冲里刚好有这 25 字节且 BCC 正确，`gb32960_locate_frame` 返回 1。旧代码用 `内容起点 + 内容长度 >= 缓冲尾` 判断 BCC 是否越界，BCC 落在最后一个字节时同样放行。中车的 `MIN_PACK_LEN`、`CONTENT_OFFSET` 与国标常量相同。不完整帧两边都停止继续扫当前缓冲。

**双 CPU `tccp_unpack`。** 声明包长与「帧头之后剩余字节」比较。旧代码比的是 `跳过字节数 + packlen > templen`，和「packlen > 剩余」相同。`TCCP_GET_PACK_LEN = MIN + 内容长度`，内容长度来自 16 位字段，包长不会小于 `TCCP_PACK_MIN_LEN`。内容长度为 0 的最小帧，缓冲刚好容纳且异或正确时仍回调。

**后门 `bp_unpack` / `bp_pack`。** 旧代码用整段剩余（含帧头前的垃圾）和 `packlen` 比，帧头不在缓冲起点时可能把越过末尾的声明长度当成放得下。新代码用帧头之后的剩余。刚好填满的帧仍接受；声明长度越过末尾才拒绝。`packlen < BP_PACK_MIN_LEN` 对这个宏同样不会出现。`bp_checkpack` 里原来调用的 `xor_check` 不在本文件（另一文件里的同名函数是 `static`），现改为 `bp_xor_check`，范围与 `bp_pack` 一致：从消息 ID 到校验前一字节。内容长度为正且不超过输出缓冲时，`bp_pack` 写出的长度字段和载荷与改前相同；负数长度会被收成 0。

**石家庄 `sjz_plat_unpack`。** 这是长度项里真正会多拒帧的地方。

- 旧代码校验是 `if (1)`，长度够就回调。新代码要求 `CommSumCheckCode(命令字节起, 整帧去掉头和校验)` 等于最后一字节。`CommSumCheckCode` 从 0 做异或，和 `XOR_Check` 对同一段的结果相同，也和 `cn6_pcak` 封包时写入的校验相同。按本模块封包算法组的帧仍通过。以前长度够但校验不对的帧现在被拒绝。
- 校验通过时游标跳过整帧。校验失败且后面还有字节时只跳 2 字节再找 `##`。旧代码无论成败都只跳 2 字节。
- 声明长度大于剩余缓冲时直接结束本缓冲。旧代码跳 2 字节后还能找到后面另一帧。缓冲以一帧合法短帧开头时不受影响；前面先出现一个长度装不下的 `##` 时，后面的合法帧到不了回调。
- 发送侧 VIN 固定占 `CN6_VIN_LEN`（17），不足补 0。旧封包按 `vin_len` 推进。`vin_len==17` 时线上布局不变。树内没有 `sjz_plat_init` 的调用点。`hd_zy` 在帧长短于固定头时直接返回，避免短 VIN 把转义扫描减成下溢；正常 17 字节 VIN 的帧长度大于帧头，转义仍执行。`hd_find_sub_str` 的循环上界改为子串必须完整落在缓冲内，完整的 `##` 仍能找到。

## 5. SM2 不在位返回 0 — Safe

只改了 `is_sm2_chip_ok()==0` 的早退返回值。芯片在位时仍 `wait_sm2_chip_sem`，仍按 `get_chip_type()` 走华大或华虹，返回值仍是芯片函数的返回值。

| 函数 | 芯片不在位，旧返回 | 新返回 | 芯片在位 |
| --- | --- | --- | --- |
| `sm2_chip_read_public_key` / `read_public_keyB` | 64，且不写输出缓冲 | 0 | 未改 |
| `sm2_chip_read_sign_data_with_z` | 64，且不写签名 | 0 | 未改 |
| `sm2_chip_write_public_private_keyB` | 1（看起来成功） | 0 | 未改 |
| `sm2_chip_private_key_decrypt_encryptB` | 解密 `len-96` 或加密 `len+96`，不做运算 | 0 | 未改 |

备案和数据流签名的失败条件本来就在：

- `sjz_plat_beian_pack`：`templen <= 0` 时打印「备案签名失败」并返回 0，不组备案包。
- `_sjz_plat_stream_report`：`ret <= 0` 时打印「信息签名失败」并返回 0。

旧的「芯片不在也返回 64」会绕过这两个判断，把未写入的缓冲当成 64 字节签名发出去。现在不在位时走已经存在的失败分支，备案包和带签名的数据流都不会报成功。这是约定的失败关闭。

树内对这五个封装的调用只有石家庄备案和石家庄数据流签名（另有一处已注释的公钥读取）。GB32960 登录、实时、锁车、车门不调用它们。SM2 未使用时，这条改动不进入原业务。

## 6. CAN 真实 DLC — Compatible-with-caveat

`can0/1/2_hook` 和 `_send_to_can_bus` 的回显改为传入真实 DLC（大于 8 则夹到 8）。`can_data_in_queue` 把 DLC 放进记录字节 `[4]` 的高 4 位，通道仍在低 2 位，扩展帧标志仍在 bit2。`store_can_msg` 用这个 DLC 填 `rxmsg.DLC`，并只拷贝这么多数据字节。`rxmsg` 在栈上按 `{0}` 清零，短帧尾部是 0。

树内没有业务解析读 `rxmsg.DLC` 做分支。`gb32960_analyze`、`uds_analysis`、`san_dian_ota_analyze`、`dfgf_sy_analyze`、`uds_server_proc`、`analyze_can_vin` 都直接使用 `Data[]`。`store_can_msg` 取通道时仍是 `[4] & 0x03` 和 bit2，DLC 的高 4 位不会改通道号。周期报文 DLC=8 时，8 个数据字节与改前一致，32960 信号不变。

和「始终按 8 字节」不同的地方：

- 打开 CAN 录像时，`S_MSG_STORE_CAN_DATA` 的 17 字节记录里，字节 `[4]` 在 DLC=8 时会带上 `0x80`。本地解析掩掉了高 4 位；按旧布局把整个字节当通道号的外部工具会读错。这不影响 32960 上报。
- DLC 小于 8 的帧，尾字节变为 0。旧钩子固定拷 8 字节，尾部是控制器数据寄存器里 DLC 以外的内容。若某条真实信号落在短帧尾部，解析结果会变成 0。本车周期帧按 8 字节组帧时看不出差别。
- `usercmd` 和 `main.c` 里仍有调用方显式传入长度 8，那些路径保持旧行为。发送侧状态帧在入队前把 `DLC` 写成 8，回显仍是 8 字节。

## 7. 报警测试掩码默认 0 — Compatible-with-caveat

`sos_test_flag` 从 1 改为 0。这是产品行为变化，报文格式不变。

`gb32960_can_get_sos_flag` 在标志为 1 时返回 `sos_test_flag_val`，为 0 时返回实时的 `stGB32960Param.sos_flag`。标志为 1 时，`gb32960_can_set_sos_flag_by_bit` 清位不会清测试掩码；分析函数里要连续 4 秒（`GetRunTime` 单位是秒）看不到该位，才清 `sos_test_flag_val`。实时数据单元和地上铁自定义包都把 `gb32960_can_get_sos_flag()` 写成 4 字节报警标志。EV18 / EM26 / D78 的最高报警等级也用这个返回值计算。

标志默认 1 时，线上报警标志和报警等级在 CAN 位清除后仍保持约 4 秒。默认 0 后，位清除的同一轮分析里上报就跟着清除。某一位正被 CAN 置住时，两条路径都会把该位置 1，上报内容相同。全 0 时也相同。差别只在「报警刚刚消失」的那几秒。

`test_sl_sos:` 仍可把标志设回 1，并按位制造测试报警；参数不小于 32 时仍关闭测试并清掩码。

## 8. OTA 偏移与 ISO-TP — Compatible-with-caveat

快乐路径（流控 BS=0、STmin=0、每次读回的字节数等于本块 `cur_tx_len`）仍然按 0、N、2N 取文件，连续帧的序号和数据字节与旧循环一致。

偏移：旧代码在发出 `S_MSG_GET_FILE_DATA` 时就把 `drv_offset` / `app_offset` 加上 `transfer_max_len` 或 `cur_tx_len`，数据还没进缓冲。新代码只在 `sandian_data_arrive` / `sandian_mcu_data_arrive` / `sandian_vcu1_data_arrive` 里用 `ota_note_file_offset` 加上实际拷进缓冲的长度，且只向前走。满块时下一次请求的偏移与旧的请求序列相同。传输结束看的是 `*_tx_len >= *_len`，不是文件偏移本身。

ISO-TP 连续帧收到 `0x30` 后改走 `sandian_isotp_send_cfs`：

- 首帧连续帧序号仍是 `0x21`，随后 `(idx+1) % 16`，数据从 `buf[4]` 起每帧 7 字节。与汇川、VCU1、通用 `can_bus_transfer_file_proc` 的旧循环一致。
- BS=0 时不按块截断，一帧流控里把本块剩余连续帧发完，再请求下一块文件。这是原来的快乐路径。
- BS 大于 0 时发够块大小就停，等下一帧 `0x30`。`data_arrive` 标志要等本块发完才清，后续流控还能进同一分支。旧代码忽略 BS，一次把块发完。
- STmin 为 0 不休眠；`0xF1..0xF9` 休眠 1 ms；其余按毫秒值，上限按 127 ms 处理。
- 最后一帧不足 7 字节时补 0。旧代码会把缓冲里多出来的字节原样发出。按长度取数的 ECU 不使用这些填充字节。

VCU1 旧代码每条连续帧固定 `os_sleep(2)`，并且每 16 帧再 `os_sleep(5)`。汇川和通用路径里对应休眠是注释掉的。新代码在 STmin=0 时 VCU1 也不再留这 2 ms。BS=0 且 ECU 能接受背靠背连续帧时，VCU1 仍能传完；依赖这 2 ms 间隔的控制器会在时序上偏离原来的快乐路径。

进度：块大小之和为 0 时返回 0，避免原来的除零。块大小非 0 时仍是 `(drv_offset + app_offset) * 100 / total_len`。偏移改成「已写入字节」后，百分比会比旧的「提前加偏移」晚大约一块，传完时分子回到已传输字节数。

`transfer_max_len` 仍被夹到缓冲容量（通用路径最大 2048，汇川/VCU1 最大 512），与 `buf` 大小一致，满块拷贝不会少计。

## 原功能影响

| 原功能 | 分类 | 现在怎样 |
| --- | --- | --- |
| GB32960 登录（链路 0 收到 `0x01`/`0x01`） | Compatible-with-caveat | BCC 与终端算法一致时仍置登录标志。样例帧 BCC 正确。错误 BCC 的登录应答不会置标志，实时数据也不再上报 |
| 实时 / 补发 / 座舱上报 | Compatible-with-caveat | 登录标志为 1 且报文装进现有 1536 缓冲时，封包内容与改前相同。数据单元为 `outbuflen` 留出 1 字节 BCC；原来数据区写满 1512 字节时 `_xDataBag` 会因放不下 BCC 返回 0，现在最多写 1511 字节并真正发出。EV18 常规报文远小于该上限 |
| 平台锁车、绑定、版本、参数查询（`0x82`/`0x80` 等） | Compatible-with-caveat | BCC 正确则仍进 `gb32960_deal` 原分支 |
| 远程启动、车门（`0xA7` / `0x61` / `0x62`） | Compatible-with-caveat | 本链路已登录后动作和成功应答不变。未登录、已登出、断链后命令被丢弃且无成功应答。东风商研院座舱控制未改 |
| 平台 TBOX 升级（`0xA5`/`0x52`，`0x92`/`0xC8`，国六 `0x95`/`0x52`） | Breaking | 默认不执行 wget，`box.bin` 不会下载。`0x92` 被拒绝时不会置 `mcu_start_update_flag`，采集循环不会停 |
| 平台三电包下载（`0xA0`/`0xC4`）及随后的 CAN 刷写 | Breaking | 默认不 wget zip，MPU 的下载完成消息不会来，CAN OTA 不会启动。命令的成功应答仍会先发出 |
| MPU 已通知「下载完成」（msgid 1/2/4）之后的 CAN 刷写 | Compatible-with-caveat | 不经过 wget 宏。BS=0、STmin=0、满块读取时偏移和连续帧与原来一致。VCU1 不再每帧固定等 2 ms。BS>0 改为按块等待下一帧流控 |
| 国六/HJ 常规数据流封包 | Safe | 1536 缓冲装得下现有包数时，各包字节与改前相同。装不下时少打后续包，不再写过 `usOutBufSize` |
| 石家庄备案、数据流签名 | Safe | 芯片在位：签名调用与改前相同。芯片不在位：返回 0，走原来的「签名失败」分支，不发伪签名。本树 GB32960 主业务不调用 SM2 |
| 报警标志与最高报警等级 | Compatible-with-caveat | 报警保持期间上报相同。CAN 位清除后不再多保持约 4 秒。`test_sl_sos:` 仍可打开测试掩码 |
| CAN 周期信号（DLC=8） | Safe | 8 字节数据与通道解析不变 |
| CAN 录像 17 字节记录 | Compatible-with-caveat | 字节 `[4]` 高 4 位变为 DLC。DLC=8 时该字节多 `0x80`。32960 业务解析不用这个原始布局 |
| 双 CPU / 后门协议中刚好一帧的短包 | Safe | 长度与异或正确则仍回调。后门校验改为本文件的 `bp_xor_check` |
| 短信下发驱动命令、定时 `ATWST` | Safe | 未经过平台 wget 白名单 |

## 结论（给合入判断）

合入后，国标 BCC 正确的登录、实时、锁车，以及登录成功之后的车门/启动，保持原路径。芯片不在位的备案按既定方式失败，芯片在位无变化。DLC=8 的 CAN 业务解析无变化。报警在消失后的约 4 秒与原来不同。三电 CAN 刷写在文件已经在本地、且流控 BS=0 时可以走完，VCU1 帧间隔变了。

平台远程 wget 默认关闭，TBOX 升级包和三电升级包的平台下载这条原业务不再执行。若这条链路仍要保留，需要在发布编译里把 `ENABLE_PLATFORM_REMOTE_WGET` 置 1，并确认现网 URL 落在白名单字符集内。本评审未改任何源码。
