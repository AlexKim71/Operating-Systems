# Makefile для модульної програми з'єднання з PostgreSQL

# Визначаємо компілятор
CC = gcc

# Шляхи до header-файлів
INCLUDE = -I/usr/include/postgresql

# Шляхи до бібліотек
# -L/usr/lib/x86_64-linux-gnu може бути потрібен, якщо libpq знаходиться там,
# інакше достатньо лише -lpq. Залишаю його, як у вашому прикладі, якщо він потрібен.
LIB = -L/usr/lib/x86_64-linux-gnu -lpq

# Визначаємо об'єктні файли для нашої програми
# Замість bill.o, connect_bill.o тощо, використовуємо наші файли:
OBJ = connect_plane.o plane.o

# Визначаємо назву нашого виконуваного файлу
# Згідно з попередніми домовленостями, ми назвали його plane_app
TARGET = plane_app

# --- Мети ---

# Мета за замовчуванням: збирає виконуваний файл
all: $(TARGET)

# Мета створення executable-файлу з назвою «plane_app»
# Залежить від наших об'єктних файлів
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(INCLUDE) $(LIB)

# Мета створення object-файлу з назвою «connect_plane.o»
connect_plane.o: connect_plane.c plane.h
	$(CC) -Wall -c -o connect_plane.o connect_plane.c $(INCLUDE)

# Мета створення object-файлу з назвою «plane.o»
plane.o: plane.c plane.h
	$(CC) -Wall -c -o plane.o plane.c $(INCLUDE)

# Мета "install": копіює виконуваний файл
# Адаптував до вашого користувача (alex)
install:
	cp ./$(TARGET) /home/alex/bin/

# Мета "clean": видаляє згенеровані файли
clean:
	rm -f *.o
	rm -f $(TARGET)

.PHONY: all install clean
