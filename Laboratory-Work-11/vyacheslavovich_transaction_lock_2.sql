-- Транзакція 1
BEGIN;
-- Операція блокування (Shared Lock на рядок з a_id = 1)
LOCK TABLE airplane IN SHARE MODE;
-- Операція читання всіх стовпчиків першого рядку таблиці (припускаємо, що перший рядок має a_id = 1)
SELECT * FROM airplane WHERE a_id = 1;
-- Операція блокування (Exclusive Lock на рядок з a_id = 1)
LOCK TABLE airplane IN EXCLUSIVE MODE;
-- Операція зміни значення другого стовпчика (model) у першому рядку
UPDATE airplane SET model = 'Boeing-747-lock2' WHERE a_id = 1;
-- Повторна операція читання всіх стовпчиків першого рядку таблиці
SELECT * FROM airplane WHERE a_id = 1;
-- Операція фіксації всіх виконаних операцій (всі блокування знімаються)
COMMIT;

-- Транзакція 2
BEGIN;
-- Операція блокування (Shared Lock на рядок з a_id = 2)
LOCK TABLE airplane IN SHARE MODE;
-- Операція читання всіх стовпчиків першого рядку таблиці (припускаємо, що другий рядок має a_id = 2)
SELECT * FROM airplane WHERE a_id = 2;
-- Операція блокування (Exclusive Lock на рядок з a_id = 2)
LOCK TABLE airplane IN EXCLUSIVE MODE;
-- Операція зміни значення другого стовпчика (model) у першому рядку
UPDATE airplane SET model = 'Airbus-A380-lock2' WHERE a_id = 2;
-- Повторна операція читання всіх стовпчиків першого рядка таблиці
SELECT * FROM airplane WHERE a_id = 2;
-- Операція фіксації всіх виконаних операцій (всі блокування знімаються)
COMMIT;