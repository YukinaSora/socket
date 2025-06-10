# 编译器
CC = gcc

# 编译选项
CFLAGS = -I./include -D_TIME_BITS=64

# 链接选项
LDFLAGS = -L./lib -lsqlite3 -lmicrohttpd -ldl -Wl,-rpath -pthread

# 源文件和目标文件
SRCS = src/main.c src/client.c src/utils.c src/view.c src/msvc.c src/gcc.c
OBJS = $(SRCS:src/%.c=build/gcc/%.o)

# 可执行文件
TARGET = Client

# 规则：默认目标
all: $(TARGET)

# 规则：生成可执行文件
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# 规则：生成目标文件
build/gcc/%.o: src/%.c | build/gcc
	$(CC) $(CFLAGS) -c $< -o $@

# 规则：创建 build/gcc 目录
build/gcc:
	mkdir -p build/gcc

# 规则：清理
clean:
	rm -f build/gcc/*.o $(TARGET)
	rmdir build/gcc

clear: | clean

# 伪目标
.PHONY: all clean clear build/gcc