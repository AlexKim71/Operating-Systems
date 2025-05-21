-- Транзакція 1 (намагається заблокувати рядок з a_id = 1, потім a_id = 2)
BEGIN;
LOCK TABLE airplane IN SHARE MODE;
SELECT * FROM airplane WHERE a_id = 1;
LOCK TABLE airplane IN EXCLUSIVE MODE;
UPDATE airplane SET model = 'Deadlock-1' WHERE a_id = 1;
SELECT * FROM airplane WHERE a_id = 1;
COMMIT;

-- Транзакція 2 (намагається заблокувати рядок з a_id = 2, потім a_id = 1)
BEGIN;
LOCK TABLE airplane IN SHARE MODE;
SELECT * FROM airplane WHERE a_id = 2;
LOCK TABLE airplane IN EXCLUSIVE MODE;
UPDATE airplane SET model = 'Deadlock-2' WHERE a_id = 2;
SELECT * FROM airplane WHERE a_id = 2;
COMMIT;