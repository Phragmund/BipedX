# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -Wextra -std=c99

# 链接选项
LDFLAGS = -L"../Product/SDL2-devel-2.32.0-mingw/SDL2-2.32.0/i686-w64-mingw32/lib" -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32

# 包含路径
CFLAGS += -I"../Product/SDL2-devel-2.32.0-mingw/SDL2-2.32.0/i686-w64-mingw32/include"

# 源文件
SRCS = simulation.c

# 目标文件
OBJS = $(SRCS:.c=.o)

# 可执行文件
TARGET = simulation

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# 目标文件规则
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理规则
clean:
	rm -f $(OBJS) $(TARGET)

# 运行规则
run: $(TARGET)
	./$(TARGET)