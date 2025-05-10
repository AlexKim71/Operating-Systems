#!/bin/bash

# Проверка наличия параметров
if [ $# -lt 1 ]; then
    echo "Для команды нужно указать параметр"
    exit 1
fi

param=\$1

# Если второй параметр info, выводим описание
if [ "\$2" == "info" ]; then
    case $param in
        MemTotal)
            echo "MemTotal: Общий объем оперативной памяти"
            ;;
        MemFree)
            echo "MemFree: Объем свободной памяти"
            ;;
        MemAvailable)
            echo "MemAvailable: Доступный объем памяти"
            ;;
        Buffers)
            echo "Buffers: Память, используемая для буферизации"
            ;;
        Cached)
            echo "Cached: Память, используемая для кэширования"
            ;;
        SwapCached)
            echo "SwapCached: Кэшированная память подкачки"
            ;;
        Active)
            echo "Active: Активная память"
            ;;
        SwapTotal)
            echo "SwapTotal: Общий объем подкачки"
            ;;
        SwapFree)
            echo "SwapFree: Свободный объем подкачки"
            ;;
        VmallocTotal)
            echo "VmallocTotal: Общий объем виртуальной памяти"
            ;;
        vendor_id)
            echo "vendor_id: Идентификатор производителя CPU"
            ;;
        cpu family)
            echo "cpu family: Семейство CPU"
            ;;
        model)
            echo "model: Модель CPU"
            ;;
        model name)
            echo "model name: Название модели CPU"
            ;;
        cpu MHz)
            echo "cpu MHz: Тактовая частота CPU"
            ;;
        cache size)
            echo "cache size: Размер кэша CPU"
            ;;
        cpu cores)
            echo "cpu cores: Количество ядер CPU"
            ;;
        Dirty)
            echo "Dirty: Память, ожидающая записи на диск"
            ;;
        bogomips)
            echo "bogomips: Производительность CPU в BogoMIPS"
            ;;
        PageTables)
            echo "PageTables: Память, используемая для таблиц страниц"
            ;;
        *)
            echo "Неизвестный параметр"
            ;;
    esac
    exit 0
fi

# Поиск параметра в файлах Procfs
case $param in
    MemTotal|MemFree|MemAvailable|Buffers|Cached|SwapCached|Active|SwapTotal|SwapFree|VmallocTotal)
        value=$(grep "^$param:" /proc/meminfo | awk '{print \$2}')
        ;;
    vendor_id)
        value=$(grep "^vendor_id" /proc/cpuinfo | awk -F': ' '{print \$2}')
        ;;
    cpu family|model|model name|cpu MHz|cache size|cpu cores)
        value=$(grep "^$param" /proc/cpuinfo | awk -F': ' '{print \$2}')
        ;;
    Dirty)
        value=$(grep "^Dirty:" /proc/meminfo | awk '{print \$2}')
        ;;
    bogomips)
        value=$(grep "^bogomips" /proc/cpuinfo | awk -F': ' '{print \$2}')
        ;;
    PageTables)
        value=$(grep "^PageTables:" /proc/meminfo | awk '{print \$2}')
        ;;
    *)
        echo "Параметр не найден"
        exit 1
        ;;
esac

# Вывод значения параметра
echo "$value"