import dht
import urequests
import netwo rk
import socket
from time import sleep
import machine

SENSOR_NAME = "TestSensor1" #Sensor Name
SLEEP_TIME = 15
SENSOR = dht.DHT22(machine.Pin(28))
LED = machine.Pin("LED", machine.Pin.OUT)
IP = "http://192.168.2.11:8080"
RUNNING = True

ssid = "" #WIFI HostName
password = "" #WIFI Password

humidity = 0.0
temperature = 0.0

def InitSensor():
    try:
        ip = connect()
    except KeyboardInterrupt:
        resetSensor()
    blinkLED(1)
    try:
        SendInitRequest()
    except Exception as e:
            print(e)
            print("Error sending the init request")
    
    blinkLED(2)
    MainLoop()
    
    
def MainLoop():
    while RUNNING:
        if humidity is not None and temperature is not None:
            print("Temp={0:0.1f}*C  Humidity={1:0.1f}%".format(temperature, humidity))
            try:
                sendDataToServer()
                
            except Exception as e:
                print(e)
                print("Error sending the post request")
        else:
            print("Failed to retrieve data from humidity sensor")
        machine.lightsleep(SLEEP_TIME*1000)
        
def blinkLED(numOfBlinks):
    for i in range(numOfBlinks):
        LED.on()
        sleep(0.1)
        LED.off()
        sleep(0.1)
   
def connect():
    #Connect to WLAN
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    while wlan.isconnected() == False:
        sleep(0.2)
    ip = wlan.ifconfig()[0]
    print(f'Connected on {ip}')
    return ip

def SendInitRequest():
    rqst = urequests.post(IP + '/sensor/init', json=
        {
            "name": SENSOR_NAME,
            "address": None,
            "log": None
        })
    rqst.close()
    blinkLED(4)

def resetSensor():
    machine.reset()
    
def getSensorData():
    SENSOR.measure()
    humidity = SENSOR.humidity() 
    temperature = SENSOR.temperature()
    
def sendDataToServer():
    rqst = urequests.post('http://192.168.2.11:8080/sensor/'+ SENSOR_NAME, json=
                {
                    "name": SENSOR_NAME,
                    "temperature": temperature,
                    "humidity": humidity,
                    "time": None
                }
            )
    rqst.close()
    blinkLED(2)

def stopSensor():
    RUNNING=False
    
InitSensor()