# Relational Database System
A MySQL-like relational database system in C++17.  
The database system is built upon the **MVC(Model-View-Controller)** application design pattern. The “top-level” controller of my program will be an
Application class. Moreover, I use the **chain-of-responsibility** design pattern to handle processing of user provided commands.
## Coding Standards
1. Argument names start with lower-case “a” (aValue)
2. Local variable names start with “the”; (theInt)
3. Object member names start with lower case, not prefix required
4. Class names are “camel-case” (SomeClass)
5. Methods should have a comment block explaining what they do
## Handle Input
### Scanning -> Tokenizing -> Parsing
**Scanning**: The process of doing character by character analysis of user input, in order to help logically organize input into byte-sized chunks (called **Tokens**).    
**Tokenizing**: The process of analyzing character groupings produced by the scanning logic and identifying the “parts of grammar and syntax" that these groupings represent.  
**Parsing**: Group a collection of tokens into meaningful “sentences”(called **statements**) that make sense in the grammar of the language  
we're processing.  
![image](https://github.com/TYJ99/relational-database/blob/main/images%20used%20in%20readme/Handle%20input.JPG)![image](https://github.com/TYJ99/relational-database/blob/main/images%20used%20in%20readme/Handle%20input%202.JPG)
![image](https://github.com/TYJ99/relational-database/blob/main/images%20used%20in%20readme/291848349_1091827508086061_193881343118258287_n.jpg)  
![image](https://github.com/TYJ99/relational-database/blob/main/images%20used%20in%20readme/292314609_2302107486620080_3005346238732548585_n.jpg)
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
Implement the logic needed to perform a `LEFT JOIN`, or a `RIGHT JOIN`.
```
> select * from Authors;
+----+------------+-----------+
| id | first_name | last_name |
+----+------------+-----------+
|  1 | Stephen    | King      |
|  2 | JK         | Rowling   |
|  3 | Truong     | Nguyen    |
+----+------------+-----------+
3 rows in set (0.00 sec)

> select * from Books;
+----+-------------------------------------------+-----------+
| id | title                                     | author_id |
+----+-------------------------------------------+-----------+
|  1 | Harry Potter and the Sorcerer's Stone     |         2 |
|  2 | Harry Potter and the Philosopher's Stone  |         2 |
|  3 | Harry Potter and the Prisoner of Azkaban  |         2 |
|  4 | Harry Potter and the Chamber of Secrets   |         2 |
|  5 | Harry Potter and the Goblet of Fire       |         2 |
|  6 | Harry Potter and the Order of the Phoenix |         2 |
|  7 | Harry Potter and the Half-Blood Prince    |         2 |
|  8 | Carrie                                    |         1 |
|  9 | The Dark Tower                            |         1 |
| 10 | The Green Mile                            |         1 |
| 11 | Wavelets and Filter Banks                 |         0 |
+----+-------------------------------------------+-----------+
11 rows in set (0.00 sec)
```
1. left join
```
> select last_name, title from Authors left join Books on Authors.id=Books.author_id;
+-----------+-------------------------------------------+
| last_name | title                                     |
+-----------+-------------------------------------------+
| Rowling   | Harry Potter and the Sorcerer's Stone     |
| Rowling   | Harry Potter and the Philosopher's Stone  |
| Rowling   | Harry Potter and the Prisoner of Azkaban  |
| Rowling   | Harry Potter and the Chamber of Secrets   |
| Rowling   | Harry Potter and the Goblet of Fire       |
| Rowling   | Harry Potter and the Order of the Phoenix |
| Rowling   | Harry Potter and the Half-Blood Prince    |
| King      | Carrie                                    |
| King      | The Dark Tower                            |
| King      | The Green Mile                            |
| Nguyen    | NULL                                      |
+-----------+-------------------------------------------+
11 rows in set (0.00 sec)
```
2. right join
```
> select last_name, title from Authors right join Books on Authors.id=Books.author_id;
+-----------+-------------------------------------------+
| last_name | title                                     |
+-----------+-------------------------------------------+
| King      | Carrie                                    |
| King      | The Dark Tower                            |
| King      | The Green Mile                            |
| Rowling   | Harry Potter and the Sorcerer's Stone     |
| Rowling   | Harry Potter and the Philosopher's Stone  |
| Rowling   | Harry Potter and the Prisoner of Azkaban  |
| Rowling   | Harry Potter and the Chamber of Secrets   |
| Rowling   | Harry Potter and the Goblet of Fire       |
| Rowling   | Harry Potter and the Order of the Phoenix |
| Rowling   | Harry Potter and the Half-Blood Prince    |
| NULL      | Wavelets and Filter Banks                 |
+-----------+-------------------------------------------+
11 rows in set (0.00 sec)
```
## Task #9
Building an index to improve performance.  
We are going to add a primary-key index to tables that we create.  
1. Add support for the SHOW INDEXES command, which shows all the indexes defined for a given database.  
```
> show indexes
+-----------------+-----------------+
| table           | field(s)        | 
+-----------------+-----------------+
| users           | id              |  
+-----------------+-----------------+
1 rows in set (nnnn secs)
```
2. Add support for the SHOW INDEX command, that shows all the key/value pairs found in an index (shown below)
```
> SHOW index id FROM Users; 
+-----------------+-----------------+
| key             | block#          | 
+-----------------+-----------------+
| 1               | 35              |  
+-----------------+-----------------+
| 2               | 36              |  
+-----------------+-----------------+
| 3               | 47              |  
+-----------------+-----------------+
3 rows in set (nnnn secs)
```
## Task #10
Add a cache to improve overall system performance.  
`Least recently used cache(LRU Cache)`  
The database system is able to run with the cache enabled or disabled.    
When enabled, I will load blocks into my cache, so that they may be retrieve more quickly on a subsequent request. The caching policy algorithm should be designed to keep "most-recently-used" blocks, and discard the "least recently used". If block requests exceed the capacity of your cache, you may have to discard recently used blocks.
## Task #11
Implement two variations of the `alter table {table name} ...` command. There are two variations that I dealt with:
```
ALTER TABLE Books ADD  pub_year int;  -- adding a new column
ALTER TABLE Books DROP subtitle;  -- dropping an existing column
```
