### Relational Database System
A relational database system like MySQL in C++17

## Task #1
Implement the "help" command, "version" command and "quit" command in the `Application` (command-processor). When a user types the "help" command with no arguments, "version" command and "quit" command, my system would respond with the message: "Help system ready!", "Version 0.1" and "DB::141 is shutting down" to shut down the database system.

## Task #2
"Database Level" commands.(case insensitive)  
1.  CREATE DATABASE {db-name}  
```
CREATE DATABASE testing1;
Query OK, 1 row affected (0.01 secs)  
```
2.  DROP DATABASE {db-name}  
```
DROP DATABASE {db-name}
Query OK, 0 rows affected (0.00) 
```
3.  SHOW DATABASES  
```
> show databases;
+--------------------+
| Database           |
+--------------------+
| foo                |
| bar                |
| wiltz              |
+--------------------+
3 rows in set (0.02 sec)
```
4.  USE {db-name}  
```
> use testing;
Database changed
```
## Task #3
table-related" commands.(case insensitive) 
1. create table `table-name`  
```
> CREATE TABLE tasks (
  id INT AUTO_INCREMENT PRIMARY KEY,
  title VARCHAR(100) NOT NULL,
  price FLOAT DEFAULT 0.0,
  due_date TIMESTAMP, //2020-04-15 10:11:12
  status BOOLEAN DEFAULT FALSE,
)
Query OK, 1 row affected (0.002 sec)
```
2. drop table `table-name`  
```
> drop table groups;
Query OK, 0 rows affected (0.02 sec)
```
3. describe `table-name`   
```
> DESCRIBE tasks;
+-----------+--------------+------+-----+---------+-----------------------------+
| Field     | Type         | Null | Key | Default | Extra                       |
+-----------+--------------+------+-----+---------+-----------------------------+
| id        | integer      | NO   | YES | NULL    | auto_increment primary key  |
| title     | varchar(100) | NO   |     | NULL    |                             |
| price     | float        | YES  |     | 0.0     |                             |
| due_date  | date         | YES  |     | NULL    |                             |
| status    | boolean      | YES  |     | FALSE   |                             |
+-----------+--------------+------+-----+---------+-----------------------------+
5 rows in set (0.000043 sec.)
```
4. show tables  
```
> show tables;
+----------------------+
| Tables_in_mydb       |
+----------------------+
| groups               |
| users                |
+----------------------+
2 rows in set (0.000025 sec.)
```
## Task #4
Implement the `Insert into` command that will allow us to add records to our database.  
```
INSERT INTO users 
  ('first_name', 'last_name', 'email') 
VALUES 
  ('David','He', 'dyhe@ucsd.edu'),
  ('Sai', 'Komatineni', 'saikomatineni@engucsd.edu');
```
## Task #5  
Implement code to handle "select" commands like, `SELECT * from Users`.  When run, this commmand will allow us to retrieve records from our database associated with a given table.(Users can select from multiple tables)
```
SELECT * from Accounts;
SELECT first_name, last_name from Users;
```
```
> SELECT * from Users;
+--------------------+--------------+
| id  | first_name   | last_name    |
+-----+--------------+--------------+
| 1   | david        | he           |
| 3   | rick         | gessner      |
+-----+--------------+--------------+
3 rows in set (0.00231 sec)
```
## Task #6  
apply filters to our search queries.  
Improve the `select` command to allow us to retrieve records from our database that meet specified search criteria. We'll also ORDER our data, and limit the number of rows we return.  
1. Select...WHERE ...
```
SELECT * from Users where first_name="Anirudh";
```
2. Select...ORDER BY `fieldname`... 
```
SELECT last_name, email from Users ORDER BY age;
```
3. Select...LIMIT row_count.../LIMIT offset, row_count...     
```
SELECT last_name, email from Users LIMIT 5;
```
```
SELECT last_name, email from Users LIMIT 2, 5;
```
For example, 
```
> SELECT id, first_name, last_name, zipcode from Users where zipcode>92120 order by zipcode LIMIT 2
+--------------------+--------------+--------------+
| id  | first_name   | last_name    | Zipcode      |
+-----+--------------+--------------+--------------+
| 3   | Anirudh      | Swaninthan   | 92126        |
| 1   | Pu           | Cheng        | 92127        |
+-----+--------------+--------------+--------------+
2 rows in set (0.00123 sec)
```
## Task #7  
Implement the `update`, `delete`, and `drop` commands that will allow us to chanage records in our database. 
1. Update {table} set...
```
UPDATE Users SET "zipcode" = 92127 WHERE zipcode>92100;
```
2. DELETE FROM {table} WHERE...
```
DELETE from Users where zipcode>92124;
Query Ok. 1 rows affected (0.000023 sec)
```
3. DROP table {table name}
```
drop table Users;
Query Ok. 5 rows affected (0.000023 sec)
```
## Task #8   
