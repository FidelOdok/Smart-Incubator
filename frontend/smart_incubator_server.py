from time import sleep
import paho.mqtt.client as paho
from paho import mqtt
from typing import Counter
import math
import random
from json import dumps

def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
client.username_pw_set("smartIncubator", "SmartIncubator@1234")
client.connect("0657fea3e3c14f5cb989d7bffd886dfd.s1.eu.hivemq.cloud", 8883)

client.on_publish = on_publish


while True:
    tempreature =  math.sin(random.random())*40
    humidity    = 50 + math.sin(random.random())*50
    inputVoltage    = 230 + math.sin(random.random())*10
    maxPower    = 300 + math.sin(random.random())*100
    heaterOntime   =  math.ceil(math.sin(random.randrange(1,1000)))

    msg = { "temperature":{ "value" :tempreature, "unit": "C"},
    "humidity":{ "value" :humidity, "unit": "H"},
    "inputVoltage":{ "value" :inputVoltage, "unit": "V"},
    "maxPower":{ "value" :maxPower, "unit": "W"},
    "heaterOntime":{ "value" :heaterOntime, "unit": " "}}

    
    msg = dumps(msg)
    
    client.publish("data/monitor/batteryStatus", payload=msg, qos=0)
    #client.loop_forever()
    sleep(5)