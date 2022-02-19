from typing import Counter
from pandas import read_csv
import psycopg2
import time
import numpy as np
import datetime as dt

# Converting the np.int datatype to python int type
from psycopg2.extensions import register_adapter, AsIs
def addapt_numpy_int64(numpy_int64):
    return AsIs(numpy_int64)
register_adapter(np.int64, addapt_numpy_int64)


#connect to an existing database
conn = psycopg2.connect(
	dbname="d9vue3b4a4ucqd",
	user="wcuqojikeffpul",
	password="a3f3ec48375c0a41e69f0eb67e433fad21d78f215c07f5f38581bc308e790d06",
	host="ec2-34-205-46-149.compute-1.amazonaws.com",
	port=5432
	)

cur = conn.cursor()
# An execute command that creates a new table
#cur.execute("CREATE TABLE incubator(id serial PRIMARY KEY, value timestamp, humidity numeric, temperature numeric);")


insert_sql = """INSERT INTO incubator(value, humidity, temperature) VALUES(%s, %s, %s)"""

counter = 0
while counter < 200:
	for ind in data.index:
		cur.execute(insert_sql, [dt.datetime.now(), np.sin(data['humidity'][ind]), np.cos(data['temperature'][ind])])
		conn.commit()
		time.sleep(5)
		print([dt.datetime.now(), np.sin(data['humidity'][ind]), np.cos(data['temperature'][ind])])

	counter=counter+1	


cur.close()
conn.close()

