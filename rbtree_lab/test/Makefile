# test/Makefile
.PHONY: test clean

# 컴파일 플래그
CPPFLAGS += -DSENTINEL -I ../src -I ../lib
CFLAGS   += -Wall -g

# 소스 목록
SRCS := test_unity_rbtree.c ../src/rbtree.c ../lib/unity.c
OBJS := $(SRCS:.c=.o)
TARGET := test_unity_rbtree

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OBJS) -o $(TARGET)

# ✅ 테스트 실행 + 메모리 누수 검사
test: $(TARGET)
	@echo "🚀 Running Unity tests..."
	./$(TARGET)
	@echo ""
	@echo "🔍 Running Valgrind memory leak check..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)