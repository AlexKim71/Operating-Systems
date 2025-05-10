#!/bin/bash

n=5  # Номер варианта
prefix="gavrylvo_"  # Початок імені об’єкта

for ((i=1; i<=(30+n); i++)); do
    suffix=$(openssl rand -hex 3)  # Генератор псевдовипадкових чисел
    filename="${prefix}${i}_${suffix}"
    
    case $((i % 4)) in
        0)
            # Создание группы
            groupadd "$filename"
            ;;
        1)
            # Создание пользователя
            useradd "$filename"
            ;;
        2)
            # Создание каталога
            mkdir "$filename"
            ;;
        3)
            # Создание файла
            touch "$filename"
            ;;
    esac
done

echo "Объекты созданы"