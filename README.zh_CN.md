<p align="right">
  简体中文 · <a href="README.md">English</a>
</p>

# 六爻固件(Liu Yao)

面向 FoloToy AI Passport(ESP32-C3,240×320 屏,三键)的全离线文王纳甲六爻
占卜应用。心中默念所问之事,在设备上摇铜钱六次(或手动录入爻值),即可得到
完整纳甲卦盘与四页传统解读——不联网、不上云、无 AI 服务。

本分支(`liuyao`)是基于 AI Passport 基线开发的衍生应用。上游平台介绍见
[docs/README.zh_CN.md](docs/README.zh_CN.md);基线硬测演示与本应用无关。

## 功能一览

- **开机即入应用**:太极封面,随后是引导式流程——问事分类 →(感情视角)→
  起卦时间 → 起卦方式 → 摇卦六爻 → 卦盘 → 四页解读。
- **九类问事**按传统规则选取用神;感情类可选男问/女问视角。
- **两种起卦方式**:设备摇卦(硬件随机数,传统 6..9 铜钱分布)或手动录入
  线下摇出的铜钱结果。
- **真实干支历法**:日柱(晚子时日界进位)、月柱(精确到分钟的节气边界,
  2020–2040)、年柱(立春换年)。上次输入的日期断电记忆。
- **完整纳甲卦盘**:宫与卦序、世应、纳甲干支与六亲六神、旬空、月破、日冲、
  伏神飞神、动爻化进化退、六冲六合。
- **四页传统解读**全部由设备端确定性规则生成:卦象总览、用神旺衰与应期、
  动爻分析,以及带综合倾向判词与同向特征句的结论页。

## 按键操作

| 按键 | 动作 |
| --- | --- |
| 上/下(短按) | 移动选中 · 调整日期字段 · 滚动解读 · 切换录入爻值 |
| OK(短按) | 确认 · 摇一爻 · 解读下一页 |
| OK(长按) | 返回上一级(结果页长按回封面) |

## 构建、测试与烧录

```bash
./tools/validate.sh --static    # 仓库检查 + 主机测试
./tools/validate.sh --firmware  # ESP-IDF 构建 + 合并镜像校验
```

完整门禁需要已激活的 ESP-IDF 5.5.3 环境。校验通过的合并镜像从偏移 `0x0`
烧入;烧录与存储数据策略见
[固件分区文档](docs/development/engineering/firmware-layout.zh_CN.md)。
中文界面文字使用应用字库子集(由 `tools/gen_liuyao_fonts.sh` 生成)——
改动界面文案后须重新生成字库。

## 设计要点(板级约束)

- 本板无 PSRAM,LVGL 内存池很小。禁止在本板上做 `transform_scale` 动画:
  LVGL 9 会把缩放对象渲染进一块 ARGB8888 离屏层,本板分配不出来,分配失败
  后渲染线程无退避空转,导致整机输入冻结。选中反馈改用阴影脉冲。
- 页面切换为瞬时切页。整屏透明度淡入在单缓冲下首帧会透出缓冲区陈旧内容,
  表现为闪屏。
- LVGL 非线程安全:LVGL 任务之外的一切对象访问都必须持有
  `bsp_lvgl_lock()`。

## 文档

- 应用完整档案与验证记录:
  [docs/reference/mutalisk999/liuyao/README.zh_CN.md](docs/reference/mutalisk999/liuyao/README.zh_CN.md)
- AI 助手入口:[AGENTS.zh_CN.md](AGENTS.zh_CN.md)
- 硬件事实:[components/bsp/include/bsp_pins.h](components/bsp/include/bsp_pins.h)
  与[硬件设计指南](docs/hardware-design/AI_HARDWARE_DEVELOPMENT_GUIDE.zh_CN.md)

构建成功不等于硬件验证。交付时须分开报告 `Build`、`Host tests`、
`Device tests` 与未验证项,烧录前须获得确认——完整交付规则见
[AGENTS.zh_CN.md](AGENTS.zh_CN.md)。
