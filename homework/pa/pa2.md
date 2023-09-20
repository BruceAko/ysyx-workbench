# pa1

## 必答题

### 程序是个状态机

**画出在YEMU上执行的加法程序的状态机**

我们定义一个状态为 (PC, R[0], R[1], M[7], halt)，则状态机为：

(0, x, x, 0, 0) -> (0, 33, x, 0, 0) -> (1, 33, x, 0, 0) -> (1, 33, 33, 0, 0) -> (2, 33, 33, 0, 0) -> (2, 16, 33, 0, 0) -> (3, 16, 33, 0, 0) -> (3, 49, 33, 0, 0) -> (4, 49, 33, 0, 0) -> (4, 49, 33, 49, 0) -> (5, 49, 33, 49, 0) -> (5, 49, 33, 49, 1)

**通过RTFSC理解YEMU如何执行一条指令**

YEMU 执行指令的流程为：

- 取指：将M[PC]处的内容拿出来
- 译码：根据指令的各个字段判断指令到底想让计算机做什么
- 执行：位于 switch 代码段中，YEMU 支持的指令非常简单，只有寄存器赋值，加法，load/store 四种。
- 更新 PC

**思考一下, 以上两者有什么联系?**

两者的联系在于：程序的两个状态之间的变化和指令的功能是一一对应的。

### RTFSC

**请整理一条指令在NEMU中的执行过程。**

fetch_decode_exec_updatepc() 函数分成三个部分：fetch_decode() 负责取指译码，s->EHelper(s) 利用函数指针调用了译码后指令应该执行的函数。cpu->pc=s->dnpc 将 pc 指向下一条指令。

取指

fetch_decode() 调用了 isa_fetch_decode()。isa_fetch_decode 会调用 instr_fetch() 把指令从内存中取出来。instr_fetch(*pc, len) 的更深内容和物理内存读写有关，此处不深究。特别的一点是它取出指令后会根据 len 更新 pc 的值（指向下一条指令）。

译码

isa_fetch_decode() 将 instr_fetch() 的返回结果存储到 s->instr.val 中。s 是一个 Decode 结构体类型，其中的 instr 是 ISADecodeInfo 类型。

这里有一个有趣的代码细节是 nemu 为了抽象化不同 ISA 的差异，在顶层的代码中使用的是相同的类型名称，这些名称在底层才会分出区别，例如 ISADecodeInfo 类型的定义是

typedef concat(__GUEST_ISA__, _ISADecodeInfo) ISADecodeInfo;

concat 函数的功能是将两个字符串拼接起来，从而实现了根据选择的 ISA 的不同定义不同的 ISADecodeInfo，将 ISA 的差异对上层抽象了。

在本实验中 instr 自然是 riscv32_ISADecodeInfo 类型。该类型的定义可以在 isa-def.h 中查到，这是一个 union，给 instr.val 赋值之后，我们可以通过调用 union 中的各个变量来轻松获得指令的各个部分。

nemu 调用了 table_main 函数来进行指令类型的确认，所有的 table 函数可以被看做一个巨大的 switch-case。这里面有两种类型的宏：

def_INSTR_IDTAB("??????? ????? ????? ??? ????? xxxxx xx", I/U/S/J/B/R, xxx);
def_INSTR_TAB("??????? ????? ????? ??? ????? xxxxx xx", xxx)

第一种和第二种的区别在于第一种还会按照传送的指令类型对指令的寄存器和立即数进行解析。这种宏会返回一个执行函数的编号。类型确认的过程无非是一些位运算，这里主要再看寄存器和立即数的解析：

为了进一步实现指令译码和操作数译码的解耦，nemu 还定义了译码操作数辅助函数，专门用于对立即数或寄存器进行译码。s 中有三个 Operand 类型的变量。Operand 类型的变量中有一个 union，分别是指向寄存器的指针，带符号立即数和无符号立即数，代表了寄存器类型/有符号立即数/无符号立即数。针对立即数的译码操作数辅助函数很简单，只要将值赋给 imm 即可。针对寄存器的则略微复杂一些：为了实现 riscv 中零号寄存器永远为0、不可写的特性，译码操作数辅助函数会对操作的读/写，和目标进行判断，以确保不会修改零号寄存器的内容 (is_write 参数只在寄存器中才会有作用）。

所有的译码辅助函数都是可以通过调用译码操作数辅助函数来高效完成。这部分和手册可以形成高度的对应。

执行

在 cpu-exec 中执行的代码很简单：就是调用 s 中存好的执行函数。形如 exec_xxx 的执行函数都是通过更加基本的 rtl 指令来实现功能。rtl 指令大多是调用了基于 c 语言的表达式运算来实现功能，少数的跳转指令稍微复杂一些。

更新 pc

将 s->dnpc 赋给 s，这里要注意 dnpc 和 snpc 的区别：snpc 仅仅指内存中该条指令的下一条指令（pc+4）位置，而 dnpc 保存了下一条真正要执行的指令的位置。在一些跳转指令中 snpc 和 dnpc 不一定相同。

### 程序如何运行

**请你以打字小游戏为例, 结合"程序在计算机上运行"的两个视角, 来剖析打字小游戏究竟是如何在计算机上运行的. 具体地, 当你按下一个字母并命中的时候, 整个计算机系统(NEMU, ISA, AM, 运行时环境, 程序) 是如何协同工作, 从而让打字小游戏实现出"命中"的游戏效果?**

在主循环之间，打字小游戏做了一些绘制屏幕方面的准备工作。这些工作在 vedio_init 中，具体为

- 读取窗口的宽和高。程序读取窗口大小通过读取 AM 提供的 AM_GPU_CONFIG 抽象寄存器实现。AM_GPU_CONFIG 抽象寄存器则通过访问 nemu 提供的 I/O 端口来获得这些参数。
- 将整个画布涂成紫色的。程序向画布输出颜色通过向 AM 提供的 AM_GPU_FBDRAW 抽象寄存器写入内容来实现。抽象寄存器会接收到开始绘制的位置的坐标，绘制内容数量，绘制的内容的 buffer，和一个是否立即同步的布尔变量。抽象寄存器的行为很简单，就是将对应的数据写到 nemu 的 I/O 端口中。nemu 的硬件会每隔一段时间检查是否有同步信号，如果有就将缓冲区的内容输出到屏幕上。
- 对各种字母的颜色做好设定。
- 
打字小游戏的主循环是一个 while(1) {} ，在循环中会做这样一些事情：

game_logic_update：每次循环中都会根据和上次更新的时间差进行若干次游戏逻辑更新。这里程序调用时间函数会使用 AM 提供的抽象寄存器 AM_TIMER_UPTIME 来读取游戏已经开始的时间。AM中读取这个抽象寄存器的逻辑是访问 nemu 的时钟 I/O 端口。

游戏逻辑更新中会定期生成一个新的字母，并且将每个字母根据其信息更新位置。关于字母，每个字母有如下一些参数：

ch：表示这个字母是啥，可以取 A-Z。
(x,y)：表示字母当前的位置。x 值是生成字母的时候随机的，y 值则会根据当前的时间不断更新。
v：表示字母单位时间移动的速度。正常情况下速度是正数，字母会往下落。当字母被击中的时候速度会变成负数，从而实现字母向上升的效果。如果速度为0，表示当前字母 miss 了。
t：这是一个计时器，用于当一个字母 miss 的时候，延迟 FPS 的时间后才会消失。

键盘读取：除非读取到 AM_KEY_NONE 表示当前没有按键，否则程序会一直在键盘读取的 while 循环中。这里程序收集键盘按键会使用 AM 提供的抽象寄存器 AM_INPUT_KEYBRD。AM_INPUT_KEYBRD 会访问 nemu 的键盘 I/O 端口，并根据得到的数据生成 keydown 和 keycode 两个参数。

键盘读取分为以下两种情况：

按下了 escape 键：调用 halt(0)，退出。
按下了是字母的键：调用 check_hit 函数检查是否确实击中了字母。check_hit 的行为比较简单：如果没有匹配到任何一个字母则更新 wrong，否则会将击中的字母的速度改成一个负值，从而一面实现字母被击中的标记，一面实现字母向上升的效果。

render ：这个函数负责更新屏幕。该函数会将字母原本所处位置的屏幕内容抹去（变成紫色），然后根据字母新的位置以及字母当前的状态（下落白色，miss红色，击中绿色）选择相应的 texture 作为 buffer 输出到对应位置。

### 编译与链接

**在nemu/src/engine/interpreter/rtl-basic.h中, 你会看到由static inline开头定义的各种RTL指令函数. 选择其中一个函数, 分别尝试去掉static, 去掉inline或去掉两者, 然后重新进行编译, 你可能会看到发生错误. 请分别解释为什么这些错误会发生/不发生? 你有办法证明你的想法吗?**

如果去掉inline，编译时会报错：xxx defined but not used。这是因为 rtl.h 文件 include 了 rtl-basic.h，但并没有使用这个函数，因此触发了 -Werror 编译选项。

如果去掉static，编译时不会报错。这是因为这里的函数被定义成了内联函数，函数内容被直接塞进了调用者的函数体中。如果我们用 objdump -d 命令去检查编译得到的汇编程序，我们会发现 rtl_xx 等一系列函数是不在其中的。

如果同时去掉 static 和 inline，编译时会报错：multiple definition of xxx。此时再查看汇编程序可以发现 rtl_xx 在其中。因为 rtl_xx 同时出现在了 cpu-exec.o hostcall.o 和 decode.o 中，所以链接时会出错。这里一个值得关注的细节是：我们编写的函数 exec_addi 调用了 rtl_addi ，但在汇编代码中并没有这一条调用。可以看到 rtl_addi 的内容被直接贴进了 exec_addi 的函数体中。这应该是编译器针对 addi 这种极其简短的函数调用做出的一种优化。

### 编译与链接2

**在nemu/include/common.h中添加一行volatile static int dummy; 然后重新编译NEMU. 请问重新编译后的NEMU含有多少个dummy变量的实体? 你是如何得到这个结果的?**

通过在 Makefile 中加入一些功能，使得在 make 可以将预编译的 *.i 文件同时输出到 /build 中，然后在 /build 中使用 grep "volatile static int dummy" -r | wc -l 命令统计个数，得到的答案是 33 个。

但这个做法其实有一些问题，因为高级程序中定义的“变量”并不一定是实体，比如连续定义两个相同名字的未初始化全局变量，在符号表中只会出现一个。因此正确的方法应该是查看符号表。使用命令 readelf --symbols $NEMU_HOME/build/riscv32-nemu-interpreter | grep "dummy" | wc -l 命令，得到的结果是 33 个。

**添加上题中的代码后, 再在nemu/include/debug.h中添加一行volatile static int dummy; 然后重新编译NEMU. 请问此时的NEMU含有多少个dummy变量的实体? 与上题中dummy变量实体数目进行比较, 并解释本题的结果.**

此时 nemu 中仍然含有 33 个 dummy 变量的实体。这是因为 common.h 中 include 了 dubug.h ，且这是唯一一个 include debug.h 的地方，未初始化的相同名字的全局变量在符号表中只有一个实体。

**修改添加的代码, 为两处dummy变量进行初始化:volatile static int dummy = 0; 然后重新编译NEMU. 你发现了什么问题? 为什么之前没有出现这样的问题? (回答完本题后可以删除添加的代码.)**

对变量进行初始化之后再编译，会报错：redefinition of dummy。这是因为赋了初值的全局变量会被认为是一个强符号。C 语言中不允许有两个相同的强符号被定义。而没有赋初值的变量是弱符号，语法上是可以重复定义的。

### 了解 Makefile

请描述你在am-kernels/kernels/hello/目录下敲入make ARCH=$ISA-nemu 后, make程序如何组织.c和.h文件, 最终生成可执行文件am-kernels/kernels/hello/build/hello-$ISA-nemu.elf. (这个问题包括两个方面:Makefile的工作方式和编译链接的过程.) 关于Makefile工作方式的提示:

- Makefile中使用了变量, 包含文件等特性
- Makefile运用并重写了一些implicit rules
- 在man make中搜索-n选项, 也许会对你有帮助
- RTFM

hello 中的 Makefile 内容比较简单：将 NAME 和 SRC 设置好，然后将 AM 中的 Makefile 全部贴进来。（虽然 AM 的 Makefile 相当复杂）

事实上 SRC 有非常多：AM 中几乎每个文件夹下都有 Makefile，把这个文件夹下的 .c 文件搜刮进 SRC 中。

按照正序，Makfile 大概做了如下一些大的事情：

在 Makefile 中有

$(DST_DIR)/%.o: %.c
	@mkdir -p $(dir $@) && echo + CC $<
	@$(CC) -std=gnu11 $(CFLAGS) -c -o $@ $(realpath $<)

它会中所有的.c文件编译成.o文件放进目录DST_DIR中。这里DST_DIR是 AM 下的 build 目录。

在 Makefile 中有

$(LIBS): %:
	@$(MAKE) -s -C $(AM_HOME)/$* archive

它会把我们自己写的库函数（如 klib）打包成 archive。

在 Makefile 中有

$(IMAGE).elf: $(OBJS) am $(LIBS)
	@echo + LD "->" $(IMAGE_REL).elf
	@$(LD) $(LDFLAGS) -o $(IMAGE).elf --start-group $(LINKAGE) --end-group

它会将所有的 .o 文件全部链接起来生成一个 .elf 文件。这里的 $(OBJS) 的生成方法不难：将 SRC 中所有的 .c 换成了 .o，加上相对应的路径前缀即可。

在 nemu.mk 中有

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

它使用 OBJCOPY 命令将 .elf 文件中的一些节做了修改，然后粘贴进了 .bin 文件中。

在 nemu.mk 中有

run: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin

它相当于在 $NEMU_HOME 目录下使用了命令 make ISA=riscv32 run ARGS=... IMG=...，这样就实现了将镜像加载到 nemu 上运行。

## 其他问答

### 立即数背后的故事

**mips32和riscv32的指令长度只有32位, 因此它们不能像x86那样, 把C代码中的32位常数直接编码到一条指令中. 思考一下, mips32和riscv32应该如何解决这个问题?**

可以设置两种加载立即数的指令：一种向高位加载，一种向低位加载。riscv32 中的 lui 指令可以实现向高位加载立即数的功能。mips指令集由于没有接触过，暂时留坑。

### 为什么不需要 rtl_muls_lo

**我们没有定义用于获取有符号数乘法结果低32位的RTL基本指令rtl_muls_lo, 你知道为什么吗?**

rtl_muls_lo 所需实现的功能与 rtl_mulu_lo 完全相同，本着不要让多个函数做同一件事情的原则，我们不需要 rtl_muls_lo。但有符号和无符号在高位的表现是不同的，因此两种 hi 我们都需要。

### 为什么执行了未实现指令会出现上述报错信息

**RTFSC, 理解执行未实现指令的时候, NEMU具体会怎么做.**

在执行了未实现的指令时，译码的过程中无法匹配到任何一种 pattern，最终会返回一个 EXEC_ID_inv 宏，即不合法指令对应的执行函数的编号。这里的宏利用元编程和 enum 的方法写出了一种鲁棒性很强的代码，即使不断添加新指令也可以使得 EXEC_ID_inv 代表的编号是正确的。

拿到 EXEC_ID_inv 后，执行函数赋为 g[idx]。在执行 g[idx] 时，执行的是 exec_inv，exec_inv() 函数调用了 rtl_hostcall 函数，并传入了 HOSTCALL_INV 参数（与正常退出的 HOSTCALL_EXIT 区分）。rtl_hostcall 函数接收到 HOSTCALL_INV 参数时，便会打印上述的报错信息。

### 为什么要有AM？

**操作系统也有自己的运行时环境. AM和操作系统提供的运行时环境有什么不同呢? 为什么会有这些不同?**

AM 中的运行时环境更多是对硬件功能的直接抽象：比如可以访问外设，可以读写内存等。而操作系统中的运行时环境抽象层次更高，比如系统调用等等，是对 AM 运行时环境的进一步封装。之所以不同是因为他们所处的抽象层不同，服务的对象也不同。操作系统向应用程序暴露 API，自然不需要让应用程序知道硬件细节。AM 的运行时环境可以让操作系统更好地利用已有的硬件功能，可以做到架构和OS的解耦。

### 指令名对照

**AT&T格式反汇编结果中的少量指令, 与手册中列出的指令名称不符, 如x86的cltd, mips32和riscv32则有不少伪指令(pseudo instruction). 除了STFW之外, 你有办法在手册中找到对应的指令吗? 如果有的话, 为什么这个办法是有效的呢?**

直接将这条伪指令对应的机器代码二进制串放进手册里搜索。因为伪指令实际上也是通过普通指令完成功能的，所以可以搜索到对应的普通指令，从而确定伪指令的行为。

### stdarg是如何实现的?

**stdarg.h中包含一些获取函数调用参数的宏, 它们可以看做是调用约定中关于参数传递方式的抽象. 不同ISA的ABI规范会定义不同的函数参数传递方式, 如果让你来实现这些宏, 你会如何实现?**

通过宏 __VA_ARGS__，可以认为它背后的实现方式应该是一个链表，链表中的每个元素包含了参数的名称和参数指向的地址。

### 关于 riscv 的 jalr 指令

在实现 jalr 指令的时候犯了一些错误，在此特地记录。jalr 指令的行为在手册中的定义如下：

t=pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t

我起初认为这样的写法非常麻烦：为什么不把 pc+4 的值直接赋给 x[rd] ，而要搞出这么一个中间变量 t 呢？事实上这是因为指令中 rs1 和 rd 可能是一样的，如果在第一条指令就直接修改了 x[rd]，就可能也修改了 x[rs1]，从而第二条指令的结果就不对了。因此中间变量 t 的设置绝非是画蛇添足。

ISA的手册经过了很多人的打磨，是非常简练的。每一条看似无用的语句一定都是充分考虑到了一些特殊情况无法删除才呈现在手册中的。因此最简单的保证正确的实现方法就是：手册说什么你就写什么。
