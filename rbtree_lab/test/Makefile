.PHONY: test test-fast clean

CPPFLAGS += -DSENTINEL -I ../src -I ../lib
CFLAGS   += -Wall -g
# (선택) Valgrind에서 스택 추적 더 보기 좋게
# CFLAGS   += -O0 -fno-omit-frame-pointer

SRCS   := test_unity_rbtree.c ../src/rbtree.c ../lib/unity.c
OBJS   := $(SRCS:.c=.o)
TARGET := test_unity_rbtree

VALGRIND ?= valgrind --leak-check=full --show-leak-kinds=all

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(TARGET)

# 빠른 테스트(Valgrind X)
test-fast: $(TARGET)
	@echo "🚀 Running Unity tests..."
	./$(TARGET)

# 기본: 테스트 + 메모리 누수 검사
test: $(TARGET)
	@echo "🚀 Running Unity tests..."
	./$(TARGET)
	@echo ""
	@echo "🔍 Running Valgrind memory leak check..."
	$(VALGRIND) ./$(TARGET)

clean:
	$(RM) $(TARGET) $(OBJS)