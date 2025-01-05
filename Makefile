# $@ 表示目标文件
# $< 表示第一个依赖项
# $^ 表示所有依赖项

# 自动检测 kernel、drivers、cpu 和 fs 目录下的所有 .c 源文件
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c fs/*.c sample-programs/*.c)

# 自动检测 kernel、drivers、cpu 和 fs 目录下的所有 .h 头文件
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h fs/*.h sample-programs/*.h)

# 将 .c 文件转换为 .o 文件，并手动添加 cpu/interrupt.o
OBJ_FILES = ${C_SOURCES:.c=.o cpu/interrupt.o}

# 默认目标，当没有明确指定目标时，执行这个规则
all: run

# 编译内核二进制文件
# 目标：kernel.bin
# 依赖项：boot/kernel_entry.o 和 ${OBJ_FILES}（所有 .o 文件）
kernel.bin: boot/kernel_entry.o ${OBJ_FILES}
	i686-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary
# 使用 i686-elf-ld 链接生成二进制文件
# -m elf_i386：指定目标为 32 位 ELF 文件
# -Ttext 0x1000：指定内核加载的起始地址为 0x1000
# --oformat binary：输出为纯二进制格式

# 创建操作系统镜像文件
# 目标：os-image.bin
# 依赖项：boot/mbr.bin 和 kernel.bin
os-image.bin: boot/mbr.bin kernel.bin
	cat $^ > $@
# 使用 cat 命令将引导扇区（boot/mbr.bin）和内核文件（kernel.bin）合并为操作系统镜像

# 运行操作系统
# 目标：run
# 依赖项：os-image.bin
run: os-image.bin
	qemu-img create my_disk.img 10M
# 创建一个大小为 10MB 的虚拟硬盘文件（my_disk.img）
	qemu-system-i386 -fda $< -hda my_disk.img
# 使用 QEMU 模拟器启动操作系统，其中：
# -fda 表示加载 os-image.bin 文件到软盘驱动器
# -hda 表示挂载虚拟硬盘 my_disk.img

# 使用 xxd 工具以十六进制显示 OS 镜像的内容
# 目标：echo
# 依赖项：os-image.bin
echo: os-image.bin
	xxd $<
# xxd：将二进制文件以十六进制格式显示，便于调试

# 创建一个用于调试的 ELF 文件
# 目标：kernel.elf
# 依赖项：boot/kernel_entry.o 和 ${OBJ_FILES}
kernel.elf: boot/kernel_entry.o ${OBJ_FILES}
	i686-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^
# 生成一个调试用的 ELF 文件，格式与 kernel.bin 相同，但保留了符号信息

# 启动操作系统的调试模式
# 目标：debug
# 依赖项：os-image.bin 和 kernel.elf
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -fda os-image.bin -d guest_errors,int &
# 使用 QEMU 启动 OS，并开启调试模式
# -s：开启 GDB 服务器，默认监听 1234 端口
# -S：在启动时暂停 CPU，等待 GDB 连接
	i386-elf-gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
# 使用 GDB 连接到 QEMU 模拟器，加载 kernel.elf 符号文件

# 编译 C 源文件为目标文件 (.o)
# %.o 表示任意目标文件，%.c 表示与目标文件对应的源文件
%.o: %.c ${HEADERS}
	i686-elf-gcc -g -m32 -ffreestanding -c $< -o $@
# -g：生成调试信息
# -m32：编译为 32 位代码
# -ffreestanding：表示代码为 freestanding 环境，标准库不可用
# -c：仅进行编译，生成目标文件

# 将汇编文件 (.asm) 编译为目标文件 (.o)
%.o: %.asm
	nasm $< -f elf -o $@
# 使用 NASM 汇编器将汇编代码编译为 ELF 格式的目标文件

# 将汇编文件 (.asm) 编译为二进制文件 (.bin)
%.bin: %.asm
	nasm $< -f bin -o $@
# 使用 NASM 汇编器将汇编代码直接编译为纯二进制文件

# 将二进制文件 (.bin) 反汇编为可读的汇编代码 (.dis)
%.dis: %.bin
	ndisasm -b 32 $< > $@
# 使用 ndisasm 工具反汇编二进制文件，生成 32 位的汇编代码

# 清理生成的所有文件
# 目标：clean
clean:
	$(RM) *.bin *.o *.dis *.elf
	$(RM) kernel/*.o
	$(RM) boot/*.o boot/*.bin
	$(RM) drivers/*.o
	$(RM) cpu/*.o
	$(RM) fs/*.o

	$(RM) sample-programs/*.o

	$(RM) my_disk.img
# 使用 $(RM) 删除所有临时文件和中间文件，包括 .bin、.o、.dis 和 .elf 文件
# 删除 my_disk.img 文件以节省空间
