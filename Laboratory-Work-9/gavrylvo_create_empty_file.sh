#!/bin/bash

read -p "Введите название файла: " filename

# Проверка длины и содержимого имени
if [ ${#filename} -gt 6 ]; then
    echo "Ошибка: Длина имени файла превышает 6 символов"
    exit 1
fi

if [[ "$filename" =~ [^a-zA-Z0-9_] ]]; then
    echo "Ошибка: Имя файла содержит недопустимые символы"
    exit 1
fi

# Проверка существования файла
if [ -e "$filename" ]; then
    echo "Ошибка: Файл уже существует"
    exit 1
fi

# Создание пустого файла
touch "$filename"
echo "Файл '$filename' создан"