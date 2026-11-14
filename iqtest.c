// ============================================================
// 老墨智商测试 IQTest.exe —— 纯 Win32 API 版（TinyCC 兼容）
// 编译：tcc -O2 -mwindows -o IQTest.exe iqtest.c
// 无运行库依赖、无壳，双击即用
// ============================================================
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// ---------------- 常量 ----------------
#define APP_TITLE L"老墨智商测试"
#define CLIENT_W  880
#define CLIENT_H  682

// 控件 ID
#define IDC_LBL_PROGRESS  1010
#define IDC_LBL_CAT       1011
#define IDC_LBL_TIMER     1012
#define IDC_LBL_QUESTION  1013
#define IDC_OPT_RADIO     1020   // 1020..1023
#define IDC_FIG_SEQ       1030
#define IDC_FIG_OPT       1031   // 1031..1034
#define IDC_BTN_PREV      1040
#define IDC_BTN_NEXT      1041
#define IDC_BTN_SUBMIT    1042
#define IDC_BTN_START     1001
#define IDC_RB_TIMED      1002
#define IDC_RB_UNTIMED    1003
#define IDC_RB_AGE        1100   // 1100..1103 四个年龄段
#define IDC_S_VERINFO     1104   // 当前版本信息行
#define IDC_S_MODELBL2    1105   // 第二步标签
#define IDC_CONFIRM_LBL   1200   // 交卷确认条
#define IDC_CONFIRM_YES   1201
#define IDC_CONFIRM_NO    1202
#define IDC_REVIEW_EDIT   1203   // 回顾页
#define IDC_REVIEW_BACK   1204
#define IDC_R_SCORE       1050
#define IDC_R_RANGE       1051
#define IDC_R_PCT         1052
#define IDC_R_TIMEUSED    1053
#define IDC_R_VERDICT     1054
#define IDC_R_GRADE       1055   // 测评等级（5 级）
#define IDC_R_CATLABEL    1060   // 1060..1064
#define IDC_R_CATBAR      1065   // 1065..1069
#define IDC_R_CATDETAIL   1070   // 1070..1074
#define IDC_R_REVIEW      1080
#define IDC_R_RESTART     1081
#define IDC_R_NOTE        1082
#define IDC_S_TITLE       1090
#define IDC_S_SUB         1091
#define IDC_S_INTRO       1092
#define IDC_S_MODELBL     1093
#define IDC_S_FOOT        1094

// ---------------- 图形与题目结构 ----------------
typedef enum { SK_CIRCLE, SK_SQUARE, SK_TRIANGLE, SK_DIAMOND, SK_PENTAGON, SK_HEXAGON, SK_ARROW } ShapeKind;

typedef struct {
    ShapeKind kind;
    int dots;       // 内部点数 0-9，-1 表示问号占位
    BOOL filled;
    int scalePct;   // 大小百分比，100 = 标准
    int rotation;   // 箭头旋转角度
} Shape;

typedef enum { CAT_NUM, CAT_LOGIC, CAT_VERBAL, CAT_SPATIAL, CAT_FIG } Cat;

typedef struct {
    const wchar_t *text;
    Cat cat;
    int weight;
    BOOL isFig;
    const wchar_t *options[4];
    int answer;
    Shape seq[5]; int seqLen;
    Shape figOpts[4];
} Question;

typedef struct { float x, y; } FPoint;

// 宏形式：静态题库初始化必须用常量表达式
#define SH(k,d,f,p,r) { k, d, f, p, r }
#define QMARK() { SK_CIRCLE, -1, FALSE, 100, 0 }

// ---------------- 题库（70 题：0..39 成人卷，40..69 少儿卷）----------------
static Question QUESTIONS[70] = {
    // ===== 数列推理 8 题 =====
    { L"请找出数列的规律，括号里应填什么？\n2、4、6、8、（ ）", CAT_NUM, 1, FALSE,
        { L"9", L"10", L"11", L"12" }, 1, { {0} }, 0, { {0} } },
    { L"1、3、6、10、15、（ ）", CAT_NUM, 2, FALSE,
        { L"18", L"20", L"21", L"24" }, 2, { {0} }, 0, { {0} } },
    { L"2、3、5、8、12、（ ）", CAT_NUM, 2, FALSE,
        { L"15", L"16", L"17", L"18" }, 2, { {0} }, 0, { {0} } },
    { L"1、4、9、16、25、（ ）", CAT_NUM, 2, FALSE,
        { L"30", L"32", L"35", L"36" }, 3, { {0} }, 0, { {0} } },
    { L"2、6、18、54、（ ）", CAT_NUM, 2, FALSE,
        { L"108", L"126", L"162", L"160" }, 2, { {0} }, 0, { {0} } },
    { L"1、1、2、3、5、8、（ ）", CAT_NUM, 2, FALSE,
        { L"11", L"12", L"13", L"14" }, 2, { {0} }, 0, { {0} } },
    { L"3、7、16、35、（ ）", CAT_NUM, 3, FALSE,
        { L"64", L"70", L"72", L"74" }, 3, { {0} }, 0, { {0} } },
    { L"4、9、25、49、（ ）", CAT_NUM, 3, FALSE,
        { L"64", L"81", L"100", L"121" }, 3, { {0} }, 0, { {0} } },
    // ===== 逻辑推理 10 题 =====
    { L"甲比乙高，丙比甲高。由此可以推出：", CAT_LOGIC, 1, FALSE,
        { L"乙比丙高", L"丙比乙高", L"甲和丙一样高", L"无法比较乙和丙" }, 1, { {0} }, 0, { {0} } },
    { L"所有的钢琴家都会识谱，小张会识谱。由此可以推出：", CAT_LOGIC, 2, FALSE,
        { L"小张是钢琴家", L"小张不是钢琴家", L"不能确定小张是不是钢琴家", L"会识谱的都是钢琴家" }, 2, { {0} }, 0, { {0} } },
    { L"如果明天下雨，运动会就延期。现在运动会没有延期。由此可以推出：", CAT_LOGIC, 2, FALSE,
        { L"明天没有下雨", L"明天一定下雨", L"运动会取消了", L"无法确定下没下雨" }, 0, { {0} }, 0, { {0} } },
    { L"排队买票时，小明前面有 3 个人，后面有 4 个人。这支队伍一共有多少人？", CAT_LOGIC, 1, FALSE,
        { L"6 人", L"7 人", L"8 人", L"9 人" }, 2, { {0} }, 0, { {0} } },
    { L"钟表 4 点整时，时针与分针的夹角是多少度？", CAT_LOGIC, 2, FALSE,
        { L"90 度", L"120 度", L"150 度", L"180 度" }, 1, { {0} }, 0, { {0} } },
    { L"一块豆腐（立方体形状）切 3 刀，最多能切成多少块？", CAT_LOGIC, 3, FALSE,
        { L"6 块", L"7 块", L"8 块", L"9 块" }, 2, { {0} }, 0, { {0} } },
    { L"聚会上一共 5 个人，每两人都握一次手，一共握了多少次手？", CAT_LOGIC, 2, FALSE,
        { L"8 次", L"10 次", L"12 次", L"20 次" }, 1, { {0} }, 0, { {0} } },
    { L"一根 1 米长的木棍，每天截去剩下的一半。第 3 天截完后，剩下的木棍有多长？", CAT_LOGIC, 2, FALSE,
        { L"1/4 米", L"1/6 米", L"1/8 米", L"1/16 米" }, 2, { {0} }, 0, { {0} } },
    { L"找规律：字母序列 A、C、F、J、O、（ ）", CAT_LOGIC, 2, FALSE,
        { L"R", L"S", L"T", L"U" }, 3, { {0} }, 0, { {0} } },
    { L"三个盒子上分别贴着「苹果」「橘子」「混合」的标签，已知所有标签都贴错了。允许你从一个盒子里摸出一个水果查看，应该从哪个盒子摸，才能确定全部标签？", CAT_LOGIC, 3, FALSE,
        { L"贴「苹果」的盒子", L"贴「橘子」的盒子", L"贴「混合」的盒子", L"随便哪个都行" }, 2, { {0} }, 0, { {0} } },
    // ===== 词汇类比 8 题 =====
    { L"医生 : 医院 相当于 教师 : （ ）", CAT_VERBAL, 1, FALSE,
        { L"课本", L"学校", L"学生", L"黑板" }, 1, { {0} }, 0, { {0} } },
    { L"热 : 冷 相当于 高 : （ ）", CAT_VERBAL, 1, FALSE,
        { L"大", L"远", L"低", L"重" }, 2, { {0} }, 0, { {0} } },
    { L"「犹豫」的反义词是：", CAT_VERBAL, 1, FALSE,
        { L"迟疑", L"果断", L"纠结", L"担心" }, 1, { {0} }, 0, { {0} } },
    { L"鸟 : 翼 相当于 鱼 : （ ）", CAT_VERBAL, 2, FALSE,
        { L"水", L"鳞", L"鳍", L"鳃" }, 2, { {0} }, 0, { {0} } },
    { L"与「同室操戈」意思最接近的成语是：", CAT_VERBAL, 2, FALSE,
        { L"同心协力", L"自相残杀", L"借刀杀人", L"事半功倍" }, 1, { {0} }, 0, { {0} } },
    { L"秒 : 分 相当于 克 : （ ）", CAT_VERBAL, 2, FALSE,
        { L"千克", L"两", L"吨", L"升" }, 0, { {0} }, 0, { {0} } },
    { L"「画蛇添足」这个成语的寓意是：", CAT_VERBAL, 1, FALSE,
        { L"精益求精", L"多此一举", L"锦上添花", L"弄假成真" }, 1, { {0} }, 0, { {0} } },
    { L"下面哪个词与其他三个不是同一类？", CAT_VERBAL, 1, FALSE,
        { L"苹果", L"香蕉", L"白菜", L"橘子" }, 2, { {0} }, 0, { {0} } },
    // ===== 空间思维 6 题 =====
    { L"把一根木头锯成 4 段，需要锯几次？", CAT_SPATIAL, 1, FALSE,
        { L"2 次", L"3 次", L"4 次", L"5 次" }, 1, { {0} }, 0, { {0} } },
    { L"一个骰子相对两个面的点数之和都是 7。如果 3 点朝上，那么与它相对的一面是几点？", CAT_SPATIAL, 1, FALSE,
        { L"2 点", L"3 点", L"4 点", L"5 点" }, 2, { {0} }, 0, { {0} } },
    { L"时钟正好指在 3:00，把它照在镜子里，镜中的时钟显示是几点？", CAT_SPATIAL, 2, FALSE,
        { L"6:00", L"9:00", L"12:00", L"3:00" }, 1, { {0} }, 0, { {0} } },
    { L"3 只猫 3 分钟能抓 3 只老鼠。照这样的速度，9 只猫抓 9 只老鼠需要几分钟？", CAT_SPATIAL, 2, FALSE,
        { L"3 分钟", L"6 分钟", L"9 分钟", L"27 分钟" }, 0, { {0} }, 0, { {0} } },
    { L"池塘里的睡莲覆盖面积每天扩大一倍，48 天正好盖满池塘。问盖住半个池塘时是第几天？", CAT_SPATIAL, 2, FALSE,
        { L"24 天", L"36 天", L"46 天", L"47 天" }, 3, { {0} }, 0, { {0} } },
    { L"从 1 楼走到 3 楼需要 20 秒。照这样的速度（每层用时相同），从 1 楼走到 5 楼需要多少秒？", CAT_SPATIAL, 2, FALSE,
        { L"30 秒", L"40 秒", L"50 秒", L"60 秒" }, 1, { {0} }, 0, { {0} } },
    // ===== 图形规律 8 题 =====
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 1, TRUE, { NULL, NULL, NULL, NULL }, 1,
        { SH(SK_CIRCLE,1,FALSE,100,0), SH(SK_CIRCLE,2,FALSE,100,0), SH(SK_CIRCLE,3,FALSE,100,0), QMARK() }, 4,
        { SH(SK_CIRCLE,5,FALSE,100,0), SH(SK_CIRCLE,4,FALSE,100,0), SH(SK_SQUARE,4,FALSE,100,0), SH(SK_CIRCLE,3,FALSE,100,0) } },
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 2, TRUE, { NULL, NULL, NULL, NULL }, 0,
        { SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_SQUARE,0,FALSE,100,0), SH(SK_TRIANGLE,0,FALSE,100,0), SH(SK_CIRCLE,0,FALSE,100,0), QMARK() }, 5,
        { SH(SK_SQUARE,0,FALSE,100,0), SH(SK_TRIANGLE,0,FALSE,100,0), SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_DIAMOND,0,FALSE,100,0) } },
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 2, TRUE, { NULL, NULL, NULL, NULL }, 2,
        { SH(SK_TRIANGLE,0,FALSE,100,0), SH(SK_SQUARE,0,FALSE,100,0), SH(SK_PENTAGON,0,FALSE,100,0), QMARK() }, 4,
        { SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_SQUARE,0,FALSE,100,0), SH(SK_HEXAGON,0,FALSE,100,0), SH(SK_PENTAGON,0,FALSE,100,0) } },
    { L"注意形状和填充两个规律，问号处应填入哪个图形？", CAT_FIG, 3, TRUE, { NULL, NULL, NULL, NULL }, 1,
        { SH(SK_CIRCLE,0,TRUE,100,0), SH(SK_SQUARE,0,FALSE,100,0), SH(SK_TRIANGLE,0,TRUE,100,0), SH(SK_CIRCLE,0,FALSE,100,0), QMARK() }, 5,
        { SH(SK_SQUARE,0,FALSE,100,0), SH(SK_SQUARE,0,TRUE,100,0), SH(SK_TRIANGLE,0,TRUE,100,0), SH(SK_DIAMOND,0,FALSE,100,0) } },
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 2, TRUE, { NULL, NULL, NULL, NULL }, 0,
        { SH(SK_CIRCLE,0,FALSE,45,0), SH(SK_CIRCLE,0,FALSE,68,0), SH(SK_CIRCLE,0,FALSE,90,0), QMARK() }, 4,
        { SH(SK_CIRCLE,0,FALSE,112,0), SH(SK_CIRCLE,0,FALSE,90,0), SH(SK_SQUARE,0,FALSE,112,0), SH(SK_CIRCLE,0,FALSE,45,0) } },
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 1, TRUE, { NULL, NULL, NULL, NULL }, 3,
        { SH(SK_CIRCLE,2,FALSE,100,0), SH(SK_CIRCLE,4,FALSE,100,0), SH(SK_CIRCLE,6,FALSE,100,0), QMARK() }, 4,
        { SH(SK_SQUARE,8,FALSE,100,0), SH(SK_CIRCLE,7,FALSE,100,0), SH(SK_CIRCLE,6,FALSE,100,0), SH(SK_CIRCLE,8,FALSE,100,0) } },
    { L"图形的边数和内部点数之间有联系，问号处应填入哪个图形？", CAT_FIG, 3, TRUE, { NULL, NULL, NULL, NULL }, 0,
        { SH(SK_TRIANGLE,3,FALSE,100,0), SH(SK_SQUARE,4,FALSE,100,0), QMARK() }, 3,
        { SH(SK_PENTAGON,5,FALSE,100,0), SH(SK_PENTAGON,4,FALSE,100,0), SH(SK_HEXAGON,6,FALSE,100,0), SH(SK_SQUARE,5,FALSE,100,0) } },
    { L"箭头每次转动一定的角度，问号处应填入哪个图形？", CAT_FIG, 2, TRUE, { NULL, NULL, NULL, NULL }, 1,
        { SH(SK_ARROW,0,TRUE,90,0), SH(SK_ARROW,0,TRUE,90,90), SH(SK_ARROW,0,TRUE,90,180), QMARK() }, 4,
        { SH(SK_ARROW,0,TRUE,90,270), SH(SK_ARROW,0,TRUE,90,90), SH(SK_ARROW,0,TRUE,90,0), SH(SK_ARROW,0,TRUE,90,180) } },
    // ===== 少儿版（8-12 岁）独有 30 题，索引 40..69 =====
    // --- 词汇类比 6 题（40..45）---
    { L"「大」的反义词是：", CAT_VERBAL, 1, FALSE,
        { L"小", L"高", L"长", L"多" }, 0, { {0} }, 0, { {0} } },
    { L"医生在医院工作，老师在（ ）工作。", CAT_VERBAL, 1, FALSE,
        { L"课本", L"学校", L"学生", L"黑板" }, 1, { {0} }, 0, { {0} } },
    { L"「高兴」的反义词是：", CAT_VERBAL, 1, FALSE,
        { L"生气", L"难过", L"开心", L"着急" }, 1, { {0} }, 0, { {0} } },
    { L"脚 : 鞋 相当于 手 : （ ）", CAT_VERBAL, 1, FALSE,
        { L"帽子", L"手套", L"袜子", L"戒指" }, 1, { {0} }, 0, { {0} } },
    { L"下面哪一个是交通工具？", CAT_VERBAL, 1, FALSE,
        { L"铅笔", L"自行车", L"椅子", L"盘子" }, 1, { {0} }, 0, { {0} } },
    { L"一年四季的顺序是：春、夏、秋、冬。夏天后面是哪个季节？", CAT_VERBAL, 1, FALSE,
        { L"春天", L"冬天", L"秋天", L"夏天" }, 2, { {0} }, 0, { {0} } },
    // --- 逻辑推理 6 题（46..51）---
    { L"树上有 5 只鸟，飞走了 2 只，又飞来了 1 只。现在树上有几只鸟？", CAT_LOGIC, 1, FALSE,
        { L"3 只", L"4 只", L"5 只", L"6 只" }, 1, { {0} }, 0, { {0} } },
    { L"小明比小红高，小红比小刚高。他们三个谁最高？", CAT_LOGIC, 1, FALSE,
        { L"小明", L"小红", L"小刚", L"一样高" }, 0, { {0} }, 0, { {0} } },
    { L"今天是星期二，后天是星期几？", CAT_LOGIC, 1, FALSE,
        { L"星期三", L"星期四", L"星期五", L"星期六" }, 1, { {0} }, 0, { {0} } },
    { L"排队时，你前面有 2 个人，后面有 3 个人。这支队伍一共有多少人？", CAT_LOGIC, 2, FALSE,
        { L"5 人", L"6 人", L"7 人", L"8 人" }, 1, { {0} }, 0, { {0} } },
    { L"妈妈买了 12 个苹果，全家每天吃 2 个。几天可以吃完？", CAT_LOGIC, 2, FALSE,
        { L"4 天", L"5 天", L"6 天", L"8 天" }, 2, { {0} }, 0, { {0} } },
    { L"妈妈今年比小红大 25 岁。5 年之后，妈妈比小红大几岁？", CAT_LOGIC, 2, FALSE,
        { L"20 岁", L"25 岁", L"30 岁", L"无法知道" }, 1, { {0} }, 0, { {0} } },
    // --- 数列推理 6 题（52..57）---
    { L"找规律填数：1、2、3、4、（ ）", CAT_NUM, 1, FALSE,
        { L"5", L"6", L"7", L"8" }, 0, { {0} }, 0, { {0} } },
    { L"2、4、6、8、（ ）", CAT_NUM, 1, FALSE,
        { L"9", L"10", L"11", L"12" }, 1, { {0} }, 0, { {0} } },
    { L"5、10、15、20、（ ）", CAT_NUM, 1, FALSE,
        { L"22", L"23", L"25", L"30" }, 2, { {0} }, 0, { {0} } },
    { L"10、9、8、7、（ ）", CAT_NUM, 1, FALSE,
        { L"5", L"6", L"8", L"4" }, 1, { {0} }, 0, { {0} } },
    { L"1、3、5、7、（ ）", CAT_NUM, 1, FALSE,
        { L"8", L"9", L"10", L"11" }, 1, { {0} }, 0, { {0} } },
    { L"1、2、4、8、（ ）", CAT_NUM, 2, FALSE,
        { L"12", L"14", L"16", L"18" }, 2, { {0} }, 0, { {0} } },
    // --- 空间思维 6 题（58..63）---
    { L"一根绳子剪 3 刀，能剪成几段？", CAT_SPATIAL, 1, FALSE,
        { L"3 段", L"4 段", L"5 段", L"6 段" }, 1, { {0} }, 0, { {0} } },
    { L"正方形有几条边？", CAT_SPATIAL, 1, FALSE,
        { L"3 条", L"4 条", L"5 条", L"6 条" }, 1, { {0} }, 0, { {0} } },
    { L"一个西瓜切 2 刀，最多能切成几块？", CAT_SPATIAL, 2, FALSE,
        { L"2 块", L"3 块", L"4 块", L"6 块" }, 2, { {0} }, 0, { {0} } },
    { L"一只小狗有 4 条腿。3 只小狗一共有多少条腿？", CAT_SPATIAL, 1, FALSE,
        { L"7 条", L"9 条", L"12 条", L"15 条" }, 2, { {0} }, 0, { {0} } },
    { L"从 1 楼走到 2 楼要走 1 层楼梯。从 1 楼走到 4 楼要走几层楼梯？", CAT_SPATIAL, 2, FALSE,
        { L"2 层", L"3 层", L"4 层", L"5 层" }, 1, { {0} }, 0, { {0} } },
    { L"你对着镜子举起自己的右手。镜子里的「你」举起的是哪只手？", CAT_SPATIAL, 2, FALSE,
        { L"右手", L"左手", L"两只手", L"都不举" }, 1, { {0} }, 0, { {0} } },
    // --- 图形规律 6 题（64..69）---
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 1, TRUE, { NULL, NULL, NULL, NULL }, 1,
        { SH(SK_CIRCLE,1,FALSE,100,0), SH(SK_CIRCLE,2,FALSE,100,0), SH(SK_CIRCLE,3,FALSE,100,0), QMARK() }, 4,
        { SH(SK_CIRCLE,5,FALSE,100,0), SH(SK_CIRCLE,4,FALSE,100,0), SH(SK_SQUARE,4,FALSE,100,0), SH(SK_CIRCLE,3,FALSE,100,0) } },
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 1, TRUE, { NULL, NULL, NULL, NULL }, 2,
        { SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_SQUARE,0,FALSE,100,0), SH(SK_TRIANGLE,0,FALSE,100,0), QMARK() }, 4,
        { SH(SK_SQUARE,0,FALSE,100,0), SH(SK_TRIANGLE,0,FALSE,100,0), SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_DIAMOND,0,FALSE,100,0) } },
    { L"图形越来越大，问号处应填入哪个图形？", CAT_FIG, 1, TRUE, { NULL, NULL, NULL, NULL }, 0,
        { SH(SK_CIRCLE,0,FALSE,45,0), SH(SK_CIRCLE,0,FALSE,68,0), QMARK() }, 3,
        { SH(SK_CIRCLE,0,FALSE,90,0), SH(SK_CIRCLE,0,FALSE,68,0), SH(SK_SQUARE,0,FALSE,90,0), SH(SK_CIRCLE,0,FALSE,45,0) } },
    { L"观察图形序列的规律，问号处应填入哪个图形？", CAT_FIG, 1, TRUE, { NULL, NULL, NULL, NULL }, 3,
        { SH(SK_CIRCLE,2,FALSE,100,0), SH(SK_CIRCLE,4,FALSE,100,0), SH(SK_CIRCLE,6,FALSE,100,0), QMARK() }, 4,
        { SH(SK_SQUARE,8,FALSE,100,0), SH(SK_CIRCLE,7,FALSE,100,0), SH(SK_CIRCLE,6,FALSE,100,0), SH(SK_CIRCLE,8,FALSE,100,0) } },
    { L"注意颜色的规律（黑色=实心），问号处应填入哪个图形？", CAT_FIG, 2, TRUE, { NULL, NULL, NULL, NULL }, 1,
        { SH(SK_CIRCLE,0,TRUE,100,0), SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_CIRCLE,0,TRUE,100,0), QMARK() }, 4,
        { SH(SK_CIRCLE,0,TRUE,100,0), SH(SK_CIRCLE,0,FALSE,100,0), SH(SK_SQUARE,0,TRUE,100,0), SH(SK_SQUARE,0,FALSE,100,0) } },
    { L"图形的边数越来越多，问号处应填入哪个图形？", CAT_FIG, 2, TRUE, { NULL, NULL, NULL, NULL }, 0,
        { SH(SK_TRIANGLE,0,FALSE,100,0), SH(SK_SQUARE,0,FALSE,100,0), QMARK() }, 3,
        { SH(SK_PENTAGON,0,FALSE,100,0), SH(SK_HEXAGON,0,FALSE,100,0), SH(SK_TRIANGLE,0,FALSE,100,0), SH(SK_SQUARE,0,FALSE,100,0) } },
};

// ---------------- 全局状态 ----------------
static int g_state = 0;            // 0=开始页 1=测验 2=结果
static int g_answers[70];
static int g_exam[70];             // 当前卷面的题目索引
static int g_current = 0;
static int g_count = 40;
static int g_version = 2;          // 0=少儿 1=青少年 2=成人 3=中老年
static double g_mean = 42.0, g_sd = 8.0;   // 定标参数
static int g_timeMin = 30;         // 限时分钟数
static BOOL g_limited = TRUE;
static BOOL g_noConfirm = FALSE;   // 隐藏测试开关：--test 跳过交卷确认
static int g_ansMode = 0;          // --ans=best 时=4，自动答一份高分卷
static int g_gotoN = -1;           // --goto=N 直接跳到第 N+1 题（截图用）
static DWORD g_startTick = 0;
static int g_remainingSec = 30 * 60;
static BOOL g_finished = FALSE;
static int g_iqFinal = 0;
static HFONT g_font, g_fontBold, g_fontBig, g_fontTitle, g_fontSmall, g_fontTimer;
static HWND g_hwnd;
static HWND g_quizCtrls[64];
static int g_catStats[5][3];   // [维度][0=答对题数 1=总题数 2=得分率%]
static const wchar_t *g_catNames[5] = { L"数列推理", L"逻辑推理", L"词汇类比", L"空间思维", L"图形规律" };

// ---------------- 前置声明 ----------------
#define CURQ (&QUESTIONS[g_exam[g_current]])
static void RenderQuestion(void);
static void DoSubmit(void);
static void ShowPage(int page);
static void PickFigureOption(int idx);
static void ConfirmSubmit(BOOL timedOut);
static void UpdateIntro(void);

// ---------------- 工具函数 ----------------
static HWND CreateLabel(HWND parent, int id, const wchar_t *text, int x, int y, int w, int h, HFONT f)
{
    HWND hw = CreateWindowW(L"STATIC", text, WS_CHILD | SS_LEFT,
        x, y, w, h, parent, (HMENU)(INT_PTR)id, NULL, NULL);
    SendMessageW(hw, WM_SETFONT, (WPARAM)f, TRUE);
    return hw;
}

static HWND CreateBtn(HWND parent, int id, const wchar_t *text, int x, int y, int w, int h, HFONT f, DWORD style)
{
    HWND hw = CreateWindowW(L"BUTTON", text, WS_CHILD | style,
        x, y, w, h, parent, (HMENU)(INT_PTR)id, NULL, NULL);
    SendMessageW(hw, WM_SETFONT, (WPARAM)f, TRUE);
    return hw;
}

// ---------------- 图形绘制 ----------------
static void AddPoly(ShapeKind kind, float r, float cx, float cy, float *rotDeg, FPoint *out, int *n)
{
    int sides = 0; float start = -90.0f;
    switch (kind) {
        case SK_TRIANGLE: sides = 3; break;
        case SK_DIAMOND:  sides = 4; break;
        case SK_PENTAGON: sides = 5; break;
        case SK_HEXAGON:  sides = 6; break;
        case SK_SQUARE:   sides = 4; start = -45.0f; break;
        default: return;
    }
    (void)rotDeg;
    for (int i = 0; i < sides; i++) {
        float ang = (start + i * 360.0f / sides) * 3.14159265f / 180.0f;
        out[*n].x = cx + r * (float)cos(ang);
        out[*n].y = cy + r * (float)sin(ang);
        (*n)++;
    }
}

static void ArrowPoints(float r, float cx, float cy, int rotDeg, FPoint *out, int *n)
{
    // 原始箭头指向右，7 个顶点，先算局部坐标再旋转
    float w = r * 0.62f;
    FPoint raw[7] = {
        { -r * 0.9f, -w }, { r * 0.25f, -w },
        { r * 0.25f, -r * 0.85f }, { r, 0 },
        { r * 0.25f, r * 0.85f }, { r * 0.25f, w },
        { -r * 0.9f, w }
    };
    float rad = rotDeg * 3.14159265f / 180.0f;
    float cs = (float)cos(rad), sn = (float)sin(rad);
    for (int i = 0; i < 7; i++) {
        out[i].x = cx + raw[i].x * cs - raw[i].y * sn;
        out[i].y = cy + raw[i].x * sn + raw[i].y * cs;
    }
    *n = 7;
}

static void DotLayout(int n, float spread, float cx, float cy, FPoint *out, int *cnt)
{
    *cnt = 0;
    if (n == 1) { out[(*cnt)].x = cx; out[(*cnt)].y = cy; (*cnt)++; return; }
    int rows = (n + 1) / 2;
    int placed = 0;
    for (int row = 0; row < rows && placed < n; row++) {
        float y = (rows == 1) ? cy : cy - spread + (2.0f * spread) * row / (rows - 1);
        int colsThis = (n - placed) >= 2 ? 2 : 1;
        for (int c = 0; c < colsThis; c++) {
            float x = (colsThis == 1) ? cx : (c == 0 ? cx - spread * 0.8f : cx + spread * 0.8f);
            out[*cnt].x = x; out[*cnt].y = y;
            (*cnt)++;
            placed++;
        }
    }
}

// 在矩形内画一个图形
static void DrawShape(HDC hdc, const Shape *s, RECT box)
{
    if (!s || s->dots == -1) {
        // 问号占位
        HFONT fBig = CreateFontW(-34, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT old = (HFONT)SelectObject(hdc, fBig);
        SetTextColor(hdc, RGB(90, 90, 160));
        SetBkMode(hdc, TRANSPARENT);
        RECT rb = box;
        DrawTextW(hdc, L"?", -1, &rb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, old);
        DeleteObject(fBig);
        return;
    }

    float cx = (box.left + box.right) / 2.0f;
    float cy = (box.top + box.bottom) / 2.0f;
    float r = ((box.right - box.left) < (box.bottom - box.top) ? (box.right - box.left) : (box.bottom - box.top))
        / 2.0f * 0.72f * (s->scalePct / 100.0f);

    COLORREF col = RGB(60, 60, 70);
    HPEN pen = CreatePen(PS_SOLID, 2, col);
    HBRUSH fill = CreateSolidBrush(col);
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    FPoint pts[16]; int n = 0;
    if (s->kind == SK_CIRCLE) {
        SelectObject(hdc, s->filled ? fill : GetStockObject(NULL_BRUSH));
        Ellipse(hdc, (int)(cx - r), (int)(cy - r), (int)(cx + r), (int)(cy + r));
        SelectObject(hdc, oldBrush);
    } else if (s->kind == SK_ARROW) {
        ArrowPoints(r, cx, cy, s->rotation, pts, &n);
        SelectObject(hdc, s->filled ? fill : GetStockObject(NULL_BRUSH));
        POINT ap[7];
        for (int i = 0; i < n; i++) { ap[i].x = (int)pts[i].x; ap[i].y = (int)pts[i].y; }
        Polygon(hdc, ap, n);
        SelectObject(hdc, oldBrush);
    } else {
        AddPoly(s->kind, r, cx, cy, NULL, pts, &n);
        SelectObject(hdc, s->filled ? fill : GetStockObject(NULL_BRUSH));
        POINT ap[8];
        for (int i = 0; i < n; i++) { ap[i].x = (int)pts[i].x; ap[i].y = (int)pts[i].y; }
        Polygon(hdc, ap, n);
        SelectObject(hdc, oldBrush);
    }

    // 内部点
    if (s->dots > 0 && s->kind != SK_ARROW) {
        FPoint dp[10]; int dn = 0;
        DotLayout(s->dots, r * 0.55f, cx, cy, dp, &dn);
        float dotR = r * 0.11f; if (dotR < 3) dotR = 3;
        HBRUSH dotBrush = CreateSolidBrush(col);
        HBRUSH ob = (HBRUSH)SelectObject(hdc, dotBrush);
        for (int i = 0; i < dn; i++)
            Ellipse(hdc, (int)(dp[i].x - dotR), (int)(dp[i].y - dotR), (int)(dp[i].x + dotR), (int)(dp[i].y + dotR));
        SelectObject(hdc, ob);
        DeleteObject(dotBrush);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    DeleteObject(fill);
}

// 圆角矩形路径
static void FillRoundRect(HDC hdc, RECT r, int radius, COLORREF bg, COLORREF border, int borderWidth)
{
    HPEN pen = CreatePen(PS_SOLID, borderWidth, border);
    HBRUSH br = CreateSolidBrush(bg);
    HPEN op = (HPEN)SelectObject(hdc, pen);
    HBRUSH ob = (HBRUSH)SelectObject(hdc, br);
    RoundRect(hdc, r.left, r.top, r.right, r.bottom, radius, radius);
    SelectObject(hdc, op);
    SelectObject(hdc, ob);
    DeleteObject(pen);
    DeleteObject(br);
}

// ---------------- 页面构建 ----------------
static void BuildStartPage(void)
{
    CreateLabel(g_hwnd, IDC_S_TITLE, L"老墨智商测试", 60, 34, 400, 46, g_fontTitle);
    CreateLabel(g_hwnd, IDC_S_SUB, L"分年龄测评 · 数列 / 逻辑 / 词汇 / 空间 / 图形 五个维度", 63, 88, 600, 24, g_font);

    HWND intro = CreateWindowW(L"EDIT", NULL,
        WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        60, 126, 760, 186, g_hwnd, (HMENU)(INT_PTR)IDC_S_INTRO, NULL, NULL);
    SendMessageW(intro, WM_SETFONT, (WPARAM)g_font, TRUE);

    CreateLabel(g_hwnd, IDC_S_MODELBL, L"第一步 · 选择年龄段：", 63, 324, 300, 24, g_fontBold);
    CreateBtn(g_hwnd, IDC_RB_AGE,     L"少儿版（8-12 岁）· 30 题", 80, 354, 300, 24, g_font, BS_AUTORADIOBUTTON | WS_GROUP);
    CreateBtn(g_hwnd, IDC_RB_AGE + 1, L"青少年版（13-17 岁）· 35 题", 400, 354, 320, 24, g_font, BS_AUTORADIOBUTTON);
    CreateBtn(g_hwnd, IDC_RB_AGE + 2, L"成人版（18-59 岁）· 40 题", 80, 384, 300, 24, g_font, BS_AUTORADIOBUTTON);
    CreateBtn(g_hwnd, IDC_RB_AGE + 3, L"中老年版（60 岁以上）· 40 题", 400, 384, 320, 24, g_font, BS_AUTORADIOBUTTON);
    CheckDlgButton(g_hwnd, IDC_RB_AGE + 2, BST_CHECKED);

    CreateLabel(g_hwnd, IDC_S_MODELBL2, L"第二步 · 选择模式：", 63, 424, 300, 24, g_fontBold);
    CreateBtn(g_hwnd, IDC_RB_TIMED, L"限时模式（更接近正式测验）", 80, 454, 400, 24, g_font, BS_AUTORADIOBUTTON | WS_GROUP);
    CreateBtn(g_hwnd, IDC_RB_UNTIMED, L"不限时模式（适合第一次接触这类题的人）", 80, 484, 420, 24, g_font, BS_AUTORADIOBUTTON);
    CheckDlgButton(g_hwnd, IDC_RB_TIMED, BST_CHECKED);

    CreateLabel(g_hwnd, IDC_S_VERINFO, L"", 63, 522, 700, 24, g_fontSmall);

    HWND start = CreateBtn(g_hwnd, IDC_BTN_START, L"开 始 测 试", 60, 552, 220, 46, g_fontBig, BS_PUSHBUTTON);

    CreateLabel(g_hwnd, IDC_S_FOOT, L"纯本地程序，不联网、不上传任何数据。适合 8 岁以上人群。", 63, 612, 600, 22, g_fontSmall);
    UpdateIntro();
}

static void BuildQuizPage(void)
{
    g_quizCtrls[0] = CreateLabel(g_hwnd, IDC_LBL_PROGRESS, L"", 26, 16, 220, 26, g_fontBold);
    g_quizCtrls[1] = CreateLabel(g_hwnd, IDC_LBL_CAT, L"", 300, 18, 260, 24, g_fontBold);
    g_quizCtrls[2] = CreateLabel(g_hwnd, IDC_LBL_TIMER, L"", 740, 16, 120, 24, g_fontTimer);
    g_quizCtrls[3] = CreateLabel(g_hwnd, IDC_LBL_QUESTION, L"", 26, 56, 830, 100, g_fontBig);
    // 图形序列（owner-draw 静态）
    g_quizCtrls[4] = CreateWindowW(L"STATIC", NULL, WS_CHILD | SS_OWNERDRAW,
        30, 166, 820, 150, g_hwnd, (HMENU)(INT_PTR)IDC_FIG_SEQ, NULL, NULL);
    // 4 个文字选项 radio（WS_GROUP 开新组，与开始页模式 radio 隔离；纵向排列各占一行）
    for (int i = 0; i < 4; i++) {
        g_quizCtrls[5 + i] = CreateBtn(g_hwnd, IDC_OPT_RADIO + i, L"", 44, 180 + i * 52, 800, 34, g_font,
            BS_AUTORADIOBUTTON | (i == 0 ? WS_GROUP : 0));
    }
    // 4 个图形选项 owner-draw button
    for (int i = 0; i < 4; i++) {
        g_quizCtrls[9 + i] = CreateWindowW(L"BUTTON", NULL,
            WS_CHILD | BS_OWNERDRAW | WS_TABSTOP,
            74 + i * 176, 344, 146, 146, g_hwnd, (HMENU)(INT_PTR)(IDC_FIG_OPT + i), NULL, NULL);
        SendMessageW(g_quizCtrls[9 + i], WM_SETFONT, (WPARAM)g_font, TRUE);
    }
    g_quizCtrls[13] = CreateBtn(g_hwnd, IDC_BTN_PREV, L"← 上一题", 40, 560, 110, 40, g_font, BS_PUSHBUTTON);
    g_quizCtrls[14] = CreateBtn(g_hwnd, IDC_BTN_NEXT, L"下一题 →", 165, 560, 110, 40, g_font, BS_PUSHBUTTON);
    g_quizCtrls[15] = CreateBtn(g_hwnd, IDC_BTN_SUBMIT, L"交 卷", 724, 560, 130, 40, g_fontBold, BS_PUSHBUTTON);
    // 交卷确认条（点击"交卷"后显示在页中，避免弹窗）
    CreateLabel(g_hwnd, IDC_CONFIRM_LBL, L"", 200, 500, 470, 24, g_fontBold);
    CreateBtn(g_hwnd, IDC_CONFIRM_YES, L"确定交卷", 240, 532, 130, 38, g_fontBold, BS_PUSHBUTTON);
    CreateBtn(g_hwnd, IDC_CONFIRM_NO, L"继续答题", 420, 532, 130, 38, g_font, BS_PUSHBUTTON);
}

static void BuildReviewPage(void)
{
    HWND ed = CreateWindowW(L"EDIT", NULL,
        WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        40, 40, 800, 500, g_hwnd, (HMENU)(INT_PTR)IDC_REVIEW_EDIT, NULL, NULL);
    SendMessageW(ed, WM_SETFONT, (WPARAM)g_font, TRUE);
    CreateBtn(g_hwnd, IDC_REVIEW_BACK, L"← 返回结果页", 40, 556, 150, 40, g_font, BS_PUSHBUTTON);
}

static void BuildResultPage(void)
{
    CreateLabel(g_hwnd, IDC_R_SCORE, L"", 60, 70, 260, 76, g_fontBig);
    CreateLabel(g_hwnd, IDC_R_GRADE, L"", 66, 152, 340, 34, g_fontBold);
    CreateLabel(g_hwnd, IDC_R_RANGE, L"", 330, 100, 500, 24, g_font);
    CreateLabel(g_hwnd, IDC_R_PCT, L"", 330, 132, 500, 24, g_font);
    CreateLabel(g_hwnd, IDC_R_TIMEUSED, L"", 330, 164, 500, 24, g_fontSmall);
    CreateLabel(g_hwnd, IDC_R_VERDICT, L"", 63, 236, 800, 110, g_font);
    // 五个维度的统计条在 WM_PAINT 里自绘（g_catStats）

    CreateBtn(g_hwnd, IDC_R_REVIEW, L"查看逐题回顾", 60, 576, 150, 40, g_font, BS_PUSHBUTTON);
    CreateBtn(g_hwnd, IDC_R_RESTART, L"重新测一次", 230, 576, 150, 40, g_font, BS_PUSHBUTTON);
    CreateLabel(g_hwnd, IDC_R_NOTE,
        L"本结果由 40 道原创题估算，仅供参考，不构成临床诊断。\n如需正式评估，请联系专业机构进行韦氏（WAIS-IV）或瑞文标准推理测验。",
        63, 634, 800, 44, g_fontSmall);
}

static void ShowPage(int page)
{
    g_state = page;
    // 开始页控件
    const int startIds[] = { IDC_S_TITLE, IDC_S_SUB, IDC_S_INTRO, IDC_S_MODELBL, IDC_RB_AGE,
        IDC_RB_AGE + 1, IDC_RB_AGE + 2, IDC_RB_AGE + 3, IDC_S_MODELBL2,
        IDC_RB_TIMED, IDC_RB_UNTIMED, IDC_BTN_START, IDC_S_FOOT, IDC_S_VERINFO };
    // 测验页控件
    const int quizIds[] = { IDC_LBL_PROGRESS, IDC_LBL_CAT, IDC_LBL_TIMER, IDC_LBL_QUESTION, IDC_FIG_SEQ,
        IDC_OPT_RADIO, IDC_OPT_RADIO + 1, IDC_OPT_RADIO + 2, IDC_OPT_RADIO + 3,
        IDC_FIG_OPT, IDC_FIG_OPT + 1, IDC_FIG_OPT + 2, IDC_FIG_OPT + 3,
        IDC_BTN_PREV, IDC_BTN_NEXT, IDC_BTN_SUBMIT };
    // 结果页控件（分类统计条由 WM_PAINT 自绘，无控件）
    const int resIds[] = { IDC_R_SCORE, IDC_R_GRADE, IDC_R_RANGE, IDC_R_PCT, IDC_R_TIMEUSED, IDC_R_VERDICT,
        IDC_R_REVIEW, IDC_R_RESTART, IDC_R_NOTE };
    // 回顾页控件
    const int revIds[] = { IDC_REVIEW_EDIT, IDC_REVIEW_BACK };

    int nStart = sizeof(startIds) / sizeof(int);
    int nQuiz = sizeof(quizIds) / sizeof(int);
    int nRes = sizeof(resIds) / sizeof(int);

    for (int i = 0; i < nStart; i++) {
        HWND hw = GetDlgItem(g_hwnd, startIds[i]);
        if (hw) ShowWindow(hw, page == 0 ? SW_SHOW : SW_HIDE);
    }
    for (int i = 0; i < nQuiz; i++) {
        HWND hw = GetDlgItem(g_hwnd, quizIds[i]);
        if (hw) ShowWindow(hw, page == 1 ? SW_SHOW : SW_HIDE);
    }
    for (int i = 0; i < nRes; i++) {
        HWND hw = GetDlgItem(g_hwnd, resIds[i]);
        if (hw) ShowWindow(hw, page == 2 ? SW_SHOW : SW_HIDE);
    }
    for (int i = 0; i < (int)(sizeof(revIds) / sizeof(int)); i++) {
        HWND hw = GetDlgItem(g_hwnd, revIds[i]);
        if (hw) ShowWindow(hw, page == 3 ? SW_SHOW : SW_HIDE);
    }
    // 确认条只在答题页可见且仅当确认流程激活
    if (page != 1) {
        ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_LBL), SW_HIDE);
        ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_YES), SW_HIDE);
        ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_NO), SW_HIDE);
    }
    InvalidateRect(g_hwnd, NULL, TRUE);
}

// ---------------- 测验流程 ----------------
static const wchar_t *CatName(Cat c)
{
    switch (c) {
        case CAT_NUM: return L"数列推理";
        case CAT_LOGIC: return L"逻辑推理";
        case CAT_VERBAL: return L"词汇类比";
        case CAT_SPATIAL: return L"空间思维";
        default: return L"图形规律";
    }
}

// 组装当前年龄段的卷面与定标参数
static void BuildVersion(int ver, int *timeMin)
{
    if (ver == 0) {            // 少儿版：少儿卷 40..69
        for (int i = 0; i < 30; i++) g_exam[i] = 40 + i;
        g_count = 30; g_mean = 23.0; g_sd = 6.5; *timeMin = 20;
    } else if (ver == 1) {     // 青少年版：成人卷去掉 5 道最难
        int k = 0;
        for (int i = 0; i < 40; i++) {
            if (i == 6 || i == 7 || i == 13 || i == 18 || i == 38) continue;
            g_exam[k++] = i;
        }
        g_count = k; g_mean = 33.0; g_sd = 7.0; *timeMin = 25;
    } else {                   // 成人版 / 中老年版：完整成人卷
        for (int i = 0; i < 40; i++) g_exam[i] = i;
        g_count = 40; g_mean = 42.0; g_sd = 8.0; *timeMin = 30;
    }
}

static const wchar_t *VersionName(int ver)
{
    switch (ver) {
        case 0: return L"少儿版（8-12 岁）";
        case 1: return L"青少年版（13-17 岁）";
        case 2: return L"成人版（18-59 岁）";
        default: return L"中老年版（60 岁以上）";
    }
}

// 根据当前选择的年龄段刷新说明文字
static void UpdateIntro(void)
{
    int ver = 2;
    for (int i = 0; i < 4; i++)
        if (IsDlgButtonChecked(g_hwnd, IDC_RB_AGE + i) == BST_CHECKED) ver = i;
    g_version = ver;

    int timeMin;
    BuildVersion(ver, &timeMin);

    wchar_t intro[1024];
    if (ver == 0) {
        _snwprintf(intro, 1024,
            L"【少儿版说明 · 共 %d 题，限时 %d 分钟】\r\n"
            L"· 题目都是贴近生活的：数数找规律、找不同类、生活小推理、看图形找规律。\r\n"
            L"· 拿不准也要蒙一个，空着按答错算。\r\n"
            L"· 分数是和同龄小朋友比较得出的，100 分代表同龄人的平均水平。\r\n"
            L"· 8 岁以下的小朋友不建议测试，题目会看不太懂。\r\n", g_count, timeMin);
    } else if (ver == 1) {
        _snwprintf(intro, 1024,
            L"【青少年版说明 · 共 %d 题，限时 %d 分钟】\r\n"
            L"· 题型与成人版相同，去掉了 5 道最难的题，难度适中。\r\n"
            L"· 测试时不要查资料、不要用计算器，否则分数不准。\r\n"
            L"· 分数是和同龄人比较得出的，100 分代表同龄人的平均水平。\r\n", g_count, timeMin);
    } else if (ver == 2) {
        _snwprintf(intro, 1024,
            L"【成人版说明 · 共 %d 题，限时 %d 分钟】\r\n"
            L"· 难度不同分值不同（1~3 分），包含文字题和图形推理题。\r\n"
            L"· 建议一次性完成，中途不要查资料、不要用计算器，否则分数不准。\r\n"
            L"· 分数是和其他成年人比较得出的，100 分代表成人平均水平。\r\n"
            L"· 正式的智力评估请到专业机构做韦氏（WAIS）或瑞文测验。\r\n", g_count, timeMin);
    } else {
        _snwprintf(intro, 1024,
            L"【中老年版说明 · 共 %d 题，建议不限时】\r\n"
            L"· 题目与成人版相同，但默认不限时：反应速度随年龄自然变化，\r\n"
            L"  限时反而测不准真实水平，建议选「不限时模式」从容作答。\r\n"
            L"· 分数是和同龄人比较得出的，100 分代表同龄人的平均水平。\r\n", g_count);
    }
    intro[1023] = 0;

    // 通用尾段
    wchar_t tail[512] =
        L"\r\n【计分方式】按难度加权计分后换算为标准智商分（平均值 100，标准差 15），"
        L"给出估分区间和百分位。结果仅供参考，不是临床诊断。";
    int len = lstrlenW(intro);
    if (len + lstrlenW(tail) < 1020) lstrcpyW(intro + len, tail);

    HWND hw = GetDlgItem(g_hwnd, IDC_S_INTRO);
    SetWindowTextW(hw, intro);

    wchar_t info[128];
    _snwprintf(info, 128, L"已选择：%s · %d 题 · 限时模式时长 %d 分钟", VersionName(ver), g_count, timeMin);
    info[127] = 0;
    SetDlgItemTextW(g_hwnd, IDC_S_VERINFO, info);
}

static void StartTest(void)
{
    g_limited = (IsDlgButtonChecked(g_hwnd, IDC_RB_TIMED) == BST_CHECKED);
    int ver = 2;
    for (int i = 0; i < 4; i++)
        if (IsDlgButtonChecked(g_hwnd, IDC_RB_AGE + i) == BST_CHECKED) ver = i;
    g_version = ver;

    int timeMin;
    BuildVersion(ver, &timeMin);

    for (int i = 0; i < g_count; i++) g_answers[i] = -1;
    if (g_noConfirm) {          // 隐藏测试模式：自动填答案，便于自动化验证/截图
        for (int i = 0; i < g_count; i++) {
            int correct = QUESTIONS[g_exam[i]].answer;
            if (g_ansMode == 4)
                g_answers[i] = (i % 5 == 4) ? (correct + 1) % 4 : correct;  // 80% 正确率
            else
                g_answers[i] = 0;
        }
    }
    if (g_gotoN >= 0 && g_gotoN < g_count) g_current = g_gotoN;
    else g_current = 0;
    g_finished = FALSE;
    g_startTick = GetTickCount();
    g_remainingSec = timeMin * 60;
    g_timeMin = timeMin;

    SetTimer(g_hwnd, 1, 1000, NULL);
    ShowPage(1);
    RenderQuestion();
}

static void RenderQuestion(void)
{
    const Question *q = CURQ;
    wchar_t buf[128];

    wsprintfW(buf, L"第 %d / %d 题", g_current + 1, g_count);
    SetDlgItemTextW(g_hwnd, IDC_LBL_PROGRESS, buf);

    wsprintfW(buf, L"【%s】· %d 分", CatName(q->cat), q->weight);
    SetDlgItemTextW(g_hwnd, IDC_LBL_CAT, buf);

    HWND hTimer = GetDlgItem(g_hwnd, IDC_LBL_TIMER);
    ShowWindow(hTimer, g_limited ? SW_SHOW : SW_HIDE);

    SetDlgItemTextW(g_hwnd, IDC_LBL_QUESTION, q->text);

    BOOL isFig = q->isFig;
    ShowWindow(GetDlgItem(g_hwnd, IDC_FIG_SEQ), isFig ? SW_SHOW : SW_HIDE);
    // 切题时收起交卷确认条
    ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_LBL), SW_HIDE);
    ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_YES), SW_HIDE);
    ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_NO), SW_HIDE);
    for (int i = 0; i < 4; i++) {
        HWND rb = GetDlgItem(g_hwnd, IDC_OPT_RADIO + i);
        HWND fo = GetDlgItem(g_hwnd, IDC_FIG_OPT + i);
        ShowWindow(rb, !isFig ? SW_SHOW : SW_HIDE);
        ShowWindow(fo, isFig ? SW_SHOW : SW_HIDE);
    }

    if (isFig) {
        for (int i = 0; i < 4; i++) InvalidateRect(GetDlgItem(g_hwnd, IDC_FIG_OPT + i), NULL, TRUE);
        InvalidateRect(GetDlgItem(g_hwnd, IDC_FIG_SEQ), NULL, TRUE);
    } else {
        for (int i = 0; i < 4; i++) {
            HWND rb = GetDlgItem(g_hwnd, IDC_OPT_RADIO + i);
            SetDlgItemTextW(g_hwnd, IDC_OPT_RADIO + i, q->options[i]);
            SendMessageW(rb, BM_SETCHECK, g_answers[g_current] == i ? BST_CHECKED : BST_UNCHECKED, 0);
        }
    }

    EnableWindow(GetDlgItem(g_hwnd, IDC_BTN_PREV), g_current > 0);
    EnableWindow(GetDlgItem(g_hwnd, IDC_BTN_NEXT), g_current < g_count - 1);
}

static void PickFigureOption(int idx)
{
    g_answers[g_current] = idx;
    for (int i = 0; i < 4; i++) {
        HWND fo = GetDlgItem(g_hwnd, IDC_FIG_OPT + i);
        InvalidateRect(fo, NULL, TRUE);
    }
}

static void GoPrev(void) { if (g_current > 0) { g_current--; RenderQuestion(); } }
static void GoNext(void)
{
    if (g_current < g_count - 1) { g_current++; RenderQuestion(); }
    else ConfirmSubmit(FALSE);
}

static void ConfirmSubmit(BOOL timedOut)
{
    if (g_finished) return;
    if (timedOut) {
        KillTimer(g_hwnd, 1);
        DoSubmit();
        return;
    }
    if (g_noConfirm) {
        KillTimer(g_hwnd, 1);
        DoSubmit();
        return;
    }
    // 内嵌确认条（不用弹窗）
    int unanswered = 0;
    for (int i = 0; i < g_count; i++) if (g_answers[i] < 0) unanswered++;
    wchar_t msg[128];
    if (unanswered == 0)
        wsprintfW(msg, L"全部答完，确定交卷吗？");
    else
        wsprintfW(msg, L"还有 %d 题没答（空着按错算），确定交卷吗？", unanswered);
    SetDlgItemTextW(g_hwnd, IDC_CONFIRM_LBL, msg);
    ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_LBL), SW_SHOW);
    ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_YES), SW_SHOW);
    ShowWindow(GetDlgItem(g_hwnd, IDC_CONFIRM_NO), SW_SHOW);
}

static double Phi(double z)
{
    double sign = z < 0 ? -1.0 : 1.0;
    double x = fabs(z) / sqrt(2.0);
    double t = 1.0 / (1.0 + 0.3275911 * x);
    double erf = 1.0 - (((((1.061405429 * t - 1.453152027) * t) + 1.421413741) * t - 0.284496736) * t + 0.254829592) * t * exp(-x * x);
    return 0.5 * (1.0 + sign * erf);
}

static const wchar_t *Verdict(int iq)
{
    if (iq >= 130) return L"非常优秀。在这套题上你的推理能力明显高出普通水平，数理和逻辑反应都很快。\n不过高分也意味着题目对你偏简单，真实水平可能比这个分数还要高。";
    if (iq >= 120) return L"优秀水平。你的抽象推理和逻辑分析能力明显强于大多数人，学习新知识通常会比较轻松。";
    if (iq >= 110) return L"中上水平。高于平均水平，处理复杂问题时有不错的分析能力，稍加练习数列和图形题还能再提。";
    if (iq >= 90)  return L"中等水平。这涵盖了大约一半的人，是最常见的区间。智力测试更多反映的是解题熟练度，不代表能力上限。";
    if (iq >= 80)  return L"中下水平。如果测试时比较仓促或状态不好，实际分数可能偏低，可以换个时间重测一次。";
    return L"分数偏低。这类测试受题目类型熟悉度影响很大，单次结果仅供参考；\n如确有评估需要，建议到专业机构做正规测评。";
}

// 五级评定（IQ 已按同龄定标，各年龄段通用）
static const wchar_t *GradeText(int iq)
{
    if (iq >= 120) return L"★★★★★  优秀";
    if (iq >= 110) return L"★★★★☆  良好";
    if (iq >= 90)  return L"★★★☆☆  中等";
    if (iq >= 80)  return L"★★☆☆☆  中下";
    return L"★☆☆☆☆  待提升";
}

static void DoSubmit(void)
{
    g_finished = TRUE;
    int raw = 0, maxRaw = 0;
    for (int i = 0; i < g_count; i++) {
        maxRaw += QUESTIONS[g_exam[i]].weight;
        if (g_answers[i] == QUESTIONS[g_exam[i]].answer) raw += QUESTIONS[g_exam[i]].weight;
    }

    // 按年龄段定标：同龄平均人 raw=mean 对应 IQ=100
    double iq = 100.0 + (raw - g_mean) * (15.0 / g_sd);
    if (iq < 55) iq = 55;
    if (iq > 160) iq = 160;
    g_iqFinal = (int)(iq + 0.5);

    double pct = Phi((iq - 100.0) / 15.0) * 100.0;
    if (pct < 0.1) pct = 0.1;
    if (pct > 99.9) pct = 99.9;

    wchar_t buf[256];
    wsprintfW(buf, L"%d", g_iqFinal);
    SetDlgItemTextW(g_hwnd, IDC_R_SCORE, buf);

    wsprintfW(buf, L"测评等级  %s", GradeText(g_iqFinal));
    SetDlgItemTextW(g_hwnd, IDC_R_GRADE, buf);

    int lo = g_iqFinal - 6; if (lo < 55) lo = 55;
    int hi = g_iqFinal + 6; if (hi > 160) hi = 160;
    wsprintfW(buf, L"估分区间  %d ~ %d", lo, hi);
    SetDlgItemTextW(g_hwnd, IDC_R_RANGE, buf);

    int pctX10 = (int)(pct * 10.0 + 0.5);
    wchar_t buf2[256];
    wsprintfW(buf2, L"大约超过了 %d.%d%% 的人（正态模型估算）", pctX10 / 10, pctX10 % 10);
    SetDlgItemTextW(g_hwnd, IDC_R_PCT, buf2);

    DWORD usedSec = (GetTickCount() - g_startTick) / 1000;
    if (g_limited) wsprintfW(buf, L"限时模式 · 用时约 %d 分钟", (int)(usedSec / 60));
    else wsprintfW(buf, L"不限时模式 · 用时约 %d 分钟", (int)(usedSec / 60));
    SetDlgItemTextW(g_hwnd, IDC_R_TIMEUSED, buf);

    SetDlgItemTextW(g_hwnd, IDC_R_VERDICT, Verdict(g_iqFinal));

    // 分维度统计
    const Cat cats[5] = { CAT_NUM, CAT_LOGIC, CAT_VERBAL, CAT_SPATIAL, CAT_FIG };
    for (int c = 0; c < 5; c++) {
        int got = 0, total = 0, correct = 0, cnt = 0;
        for (int i = 0; i < g_count; i++) {
            if (QUESTIONS[g_exam[i]].cat == cats[c]) {
                total += QUESTIONS[g_exam[i]].weight; cnt++;
                if (g_answers[i] == QUESTIONS[g_exam[i]].answer) { got += QUESTIONS[g_exam[i]].weight; correct++; }
            }
        }
        int rate = total == 0 ? 0 : (int)((double)got / total * 100.0 + 0.5);
        g_catStats[c][0] = correct;
        g_catStats[c][1] = cnt;
        g_catStats[c][2] = rate;
    }

    ShowPage(2);

    if (g_noConfirm) {          // 测试模式：落结果日志供独立核算
        int glv = g_iqFinal >= 120 ? 5 : g_iqFinal >= 110 ? 4 : g_iqFinal >= 90 ? 3 : g_iqFinal >= 80 ? 2 : 1;
        FILE *f = fopen("C:\\Users\\admin\\AppData\\Local\\Temp\\iqtest_result.txt", "w");
        if (f) {
            fprintf(f, "raw=%d maxRaw=%d mean=%.1f sd=%.1f iq=%d gradeN=%d ver=%d\n",
                raw, maxRaw, g_mean, g_sd, g_iqFinal, glv, g_version);
            fclose(f);
        }
    }
}

// ---------------- 逐题回顾（页内展示，不用弹窗）----------------
static const wchar_t *ShapeName(const Shape *s)
{
    static wchar_t buf[64];
    const wchar_t *kind = L"";
    switch (s->kind) {
        case SK_CIRCLE: kind = L"圆形"; break;
        case SK_SQUARE: kind = L"方形"; break;
        case SK_TRIANGLE: kind = L"三角形"; break;
        case SK_DIAMOND: kind = L"菱形"; break;
        case SK_PENTAGON: kind = L"五边形"; break;
        case SK_HEXAGON: kind = L"六边形"; break;
        case SK_ARROW: {
            int deg = s->rotation % 360;
            const wchar_t *dir = deg == 0 ? L"向右" : deg == 90 ? L"向下" : deg == 180 ? L"向左" : L"向上";
            wsprintfW(buf, L"箭头·%s", dir);
            return buf;
        }
    }
    if (s->dots > 0) {
        if (s->filled) wsprintfW(buf, L"%s·实心·%d点", kind, s->dots);
        else wsprintfW(buf, L"%s·%d点", kind, s->dots);
    } else {
        if (s->filled) wsprintfW(buf, L"%s·实心", kind);
        else if (s->scalePct >= 110) wsprintfW(buf, L"%s·加大", kind);
        else wsprintfW(buf, L"%s", kind);
    }
    return buf;
}

static void ShowReview(void)
{
    static wchar_t text[16384];
    int pos = 0;
    text[0] = 0;
    for (int i = 0; i < g_count; i++) {
        const Question *q = &QUESTIONS[g_exam[i]];
        BOOL ok = (g_answers[i] == q->answer);
        wchar_t line[512];
        wchar_t letter[2] = { (wchar_t)(L'A' + q->answer), 0 };
        const wchar_t *ans;
        if (q->isFig) {
            ans = ShapeName(&q->figOpts[q->answer]);
            wsprintfW(line, L"第%2d题 [%s %d分]  %s   正确答案 %s：%s\r\n",
                i + 1, CatName(q->cat), q->weight,
                ok ? L"√ 答对" : L"× 答错", letter, ans);
        } else {
            wsprintfW(line, L"第%2d题 [%s %d分]  %s   正确答案 %s：%s\r\n",
                i + 1, CatName(q->cat), q->weight,
                ok ? L"√ 答对" : L"× 答错", letter, q->options[q->answer]);
        }
        int len = lstrlenW(line);
        if (pos + len < 16383) { lstrcpyW(text + pos, line); pos += len; }
    }
    text[pos] = 0;
    SetDlgItemTextW(g_hwnd, IDC_REVIEW_EDIT, text);
    ShowPage(3);
}

// ---------------- WndProc ----------------
static void PaintClient(HDC hdc)
{
    // 背景
    RECT rc; GetClientRect(g_hwnd, &rc);
    HBRUSH bg = CreateSolidBrush(RGB(246, 247, 250));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    SetBkMode(hdc, TRANSPARENT);

    if (g_state == 2) {
        // 五个维度的统计条
        for (int c = 0; c < 5; c++) {
            int y = 364 + c * 40;
            // 名称
            RECT rn = { 60, y, 160, y + 22 };
            DrawTextW(hdc, g_catNames[c], -1, &rn, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            // 条底
            RECT track = { 175, y + 2, 535, y + 20 };
            HBRUSH tb = CreateSolidBrush(RGB(224, 227, 235));
            FillRect(hdc, &track, tb);
            DeleteObject(tb);
            // 填充
            int fillW = (int)((535 - 175) * (g_catStats[c][2] / 100.0));
            if (fillW > 0) {
                RECT fillR = { 175, y + 2, 175 + fillW, y + 20 };
                COLORREF col = g_catStats[c][2] >= 60 ? RGB(63, 150, 90)
                    : (g_catStats[c][2] >= 30 ? RGB(63, 106, 220) : RGB(214, 120, 60));
                HBRUSH fb = CreateSolidBrush(col);
                FillRect(hdc, &fillR, fb);
                DeleteObject(fb);
            }
            // 详情
            wchar_t buf[128];
            wsprintfW(buf, L"答对 %d/%d 题 · 得分率 %d%%", g_catStats[c][0], g_catStats[c][1], g_catStats[c][2]);
            RECT rd = { 560, y, 840, y + 22 };
            SetTextColor(hdc, RGB(110, 115, 130));
            DrawTextW(hdc, buf, -1, &rd, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
        case WM_CREATE: {
            g_hwnd = hwnd;
            g_font = CreateFontW(-15, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"微软雅黑");
            g_fontBold = CreateFontW(-15, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"微软雅黑");
            g_fontBig = CreateFontW(-17, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"微软雅黑");
            g_fontTitle = CreateFontW(-34, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"微软雅黑");
            g_fontSmall = CreateFontW(-12, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"微软雅黑");
            g_fontTimer = CreateFontW(-16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH, L"Consolas");

            BuildStartPage();
            BuildQuizPage();
            BuildResultPage();
            BuildReviewPage();
            ShowPage(0);
            return 0;
        }
        case WM_TIMER: {
            if (g_state != 1 || !g_limited || g_finished) break;
            g_remainingSec--;
            wchar_t buf[64];
            wsprintfW(buf, L"剩余 %02d:%02d", g_remainingSec / 60, g_remainingSec % 60);
            SetDlgItemTextW(g_hwnd, IDC_LBL_TIMER, buf);
            if (g_remainingSec <= 0) {
                // 时间到：直接自动交卷，不打断
                KillTimer(hwnd, 1);
                ConfirmSubmit(TRUE);
            }
            return 0;
        }
        case WM_COMMAND: {
            int id = LOWORD(wp);
            int code = HIWORD(wp);
            if (g_state == 1) {
                if (id >= IDC_OPT_RADIO && id < IDC_OPT_RADIO + 4 && code == BN_CLICKED) {
                    g_answers[g_current] = id - IDC_OPT_RADIO;
                }
                else if (id >= IDC_FIG_OPT && id < IDC_FIG_OPT + 4 && code == BN_CLICKED) {
                    PickFigureOption(id - IDC_FIG_OPT);
                }
                else if (id == IDC_BTN_PREV && code == BN_CLICKED) GoPrev();
                else if (id == IDC_BTN_NEXT && code == BN_CLICKED) GoNext();
                else if (id == IDC_BTN_SUBMIT && code == BN_CLICKED) ConfirmSubmit(FALSE);
                else if (id == IDC_CONFIRM_YES && code == BN_CLICKED) {
                    ShowWindow(GetDlgItem(hwnd, IDC_CONFIRM_LBL), SW_HIDE);
                    ShowWindow(GetDlgItem(hwnd, IDC_CONFIRM_YES), SW_HIDE);
                    ShowWindow(GetDlgItem(hwnd, IDC_CONFIRM_NO), SW_HIDE);
                    KillTimer(hwnd, 1);
                    DoSubmit();
                }
                else if (id == IDC_CONFIRM_NO && code == BN_CLICKED) {
                    ShowWindow(GetDlgItem(hwnd, IDC_CONFIRM_LBL), SW_HIDE);
                    ShowWindow(GetDlgItem(hwnd, IDC_CONFIRM_YES), SW_HIDE);
                    ShowWindow(GetDlgItem(hwnd, IDC_CONFIRM_NO), SW_HIDE);
                }
            } else if (g_state == 3) {
                if (id == IDC_REVIEW_BACK && code == BN_CLICKED) ShowPage(2);
            } else if (g_state == 0) {
                if (id == IDC_BTN_START && code == BN_CLICKED) StartTest();
                else if (id >= IDC_RB_AGE && id < IDC_RB_AGE + 4 && code == BN_CLICKED) {
                    // 选 60+ 版本时自动切到不限时
                    if (id == IDC_RB_AGE + 3)
                        CheckDlgButton(hwnd, IDC_RB_UNTIMED, BST_CHECKED);
                    UpdateIntro();
                }
            } else if (g_state == 2) {
                if (id == IDC_R_REVIEW && code == BN_CLICKED) ShowReview();
                else if (id == IDC_R_RESTART && code == BN_CLICKED) {
                    KillTimer(hwnd, 1);
                    ShowPage(0);
                }
            }
            return 0;
        }
        case WM_DRAWITEM: {
            DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lp;
            int id = (int)dis->CtlID;
            HDC hdc = dis->hDC;
            RECT rc = dis->rcItem;
            SetBkMode(hdc, TRANSPARENT);

            if (id == IDC_FIG_SEQ) {
                // 序列背景
                HBRUSH bg = CreateSolidBrush(RGB(246, 247, 250));
                FillRect(hdc, &rc, bg);
                DeleteObject(bg);
                const Question *q = CURQ;
                int n = q->seqLen;
                int cellW = 108, cellH = 108, gap = 24;
                int totalW = n * cellW + (n - 1) * gap;
                int startX = (rc.right - rc.left - totalW) / 2;
                if (startX < 10) startX = 10;
                int startY = (rc.bottom - rc.top - cellH) / 2;
                for (int i = 0; i < n; i++) {
                    RECT cell = { startX + i * (cellW + gap), startY, startX + i * (cellW + gap) + cellW, startY + cellH };
                    FillRoundRect(hdc, cell, 12, RGB(255, 255, 255), RGB(205, 205, 215), 1);
                    DrawShape(hdc, &q->seq[i], cell);
                }
                return TRUE;
            }
            if (id >= IDC_FIG_OPT && id < IDC_FIG_OPT + 4) {
                int idx = id - IDC_FIG_OPT;
                const Question *q = CURQ;
                BOOL sel = (g_answers[g_current] == idx);
                FillRoundRect(hdc, rc, 4,
                    sel ? RGB(238, 243, 255) : RGB(255, 255, 255),
                    sel ? RGB(63, 106, 220) : RGB(200, 200, 212),
                    sel ? 2 : 1);
                // 字母角标
                wchar_t letter[2] = { (wchar_t)(L'A' + idx), 0 };
                HFONT f = CreateFontW(-13, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
                HFONT of = (HFONT)SelectObject(hdc, f);
                SetTextColor(hdc, RGB(110, 110, 120));
                RECT lt = { rc.left + 6, rc.top + 4, rc.left + 30, rc.top + 24 };
                DrawTextW(hdc, letter, -1, &lt, DT_LEFT | DT_TOP);
                SelectObject(hdc, of);
                DeleteObject(f);
                // 图形
                RECT box = { rc.left + 12, rc.top + 26, rc.right - 12, rc.bottom - 6 };
                DrawShape(hdc, &q->figOpts[idx], box);
                return TRUE;
            }
            break;
        }
        case WM_ERASEBKGND:
            return 1;  // 防闪烁，背景在 WM_PAINT 里画
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintClient(hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------- WinMain ----------------
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pCmdLine, int nShow)
{
    (void)hPrev;
    if (pCmdLine && strstr(pCmdLine, "--test")) g_noConfirm = TRUE;
    if (pCmdLine && strstr(pCmdLine, "--ans=best")) g_ansMode = 4;
    if (pCmdLine && strstr(pCmdLine, "--goto=")) {
        const char *p = strstr(pCmdLine, "--goto=") + 7;
        g_gotoN = atoi(p);
    }

    // 高 DPI 适配
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        typedef BOOL (WINAPI *SPDAFn)(void);
        SPDAFn pSPDA = (SPDAFn)GetProcAddress(user32, "SetProcessDPIAware");
        if (pSPDA) pSPDA();
    }

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"LM_IQTest";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassW(&wc);

    RECT wr = { 0, 0, CLIENT_W, CLIENT_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), FALSE);
    int w = wr.right - wr.left, h = wr.bottom - wr.top;
    int sx = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int sy = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;

    HWND hwnd = CreateWindowW(L"LM_IQTest", APP_TITLE,
        WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
        sx, sy, w, h, NULL, NULL, hInst, NULL);

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}
