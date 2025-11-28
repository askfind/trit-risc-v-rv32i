# Имя целевого исполняемого файла
TARGET = emu

# Компилятор
CC = gcc

# Исходные файлы
SRCS = main.c elf_parser.c bit_trit_parser.c log.c ternary.c inst.c inst_trit.c cli.c

# Объектные файлы
OBJS = $(SRCS:.c=.o)

# Сборка основной программы
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

# Правило для компиляции .c файлов в .o файлы
%.o: %.c
	$(CC) -c $< -o $@

# Запуск программы
run: $(TARGET)
	./$(TARGET)

# Очистка
clean:
	rm -f $(TARGET) $(OBJS)

# Тесты
test:
	./test.sh

test_trit:
	./test_trit.sh

# Короткая команда для тестирования
t:
	./$(TARGET) -a -m -d "01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" 1

.PHONY: clean test test_trit run t