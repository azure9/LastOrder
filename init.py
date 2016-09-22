import sqlite3


connection = sqlite3.connect('data.db')
connection.text_factory = 'gbk'
cursor = connection.cursor()

with open('init.sql') as sql_sque:
    for each_line in sql_sque:
        row = each_line.strip()
        cursor.execute(row)

connection.commit()
connection.close()