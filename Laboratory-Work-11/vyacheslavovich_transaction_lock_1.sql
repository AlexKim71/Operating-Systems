-- Транзакція 1
BEGIN;
-- Операція блокування (Shared Lock на рядок з a_id = 1)
LOCK TABLE airplane IN SHARE MODE;
-- Операція читання всіх стовпчиків першого рядку таблиці (припускаємо, що перший рядок має a_id = 1)
SELECT * FROM airplane WHERE a_id = 1;
-- Операція зміни значення другого стовпчика (model) у першому рядку
UPDATE airplane SET model = 'Boeing-747-v2' WHERE a_id = 1;
-- Повторна операція читання всіх стовпчиків першого рядку таблиці
SELECT * FROM airplane WHERE a_id = 1;
-- Операція фіксації всіх виконаних операцій
COMMIT;

-- Транзакція 2
BEGIN;
-- Операція блокування (Shared Lock на рядок з a_id = 2)
LOCK TABLE airplane IN SHARE MODE;
-- Операція читання всіх стовпчиків першого рядку таблиці (припускаємо, що другий рядок має a_id = 2)
SELECT * FROM airplane WHERE a_id = 2;
-- Операція зміни значення другого стовпчика (model) у першому рядку
UPDATE airplane SET model = 'Airbus-A380-v2' WHERE a_id = 2;
-- Повторна операція читання всіх стовпчиків першого рядку таблиці
SELECT * FROM airplane WHERE a_id = 2;
-- Операція фіксації всіх виконаних операцій
COMMIT;