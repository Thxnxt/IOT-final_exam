from RPLCD.i2c import CharLCD
import paho.mqtt.client as mqtt
import json
import time

# 1. แก้ไข Address จอ LCD
lcd = CharLCD(
    i2c_expander='PCF8574',
    address=0x27,
    port=1,
    cols=16,
    rows=2,
    charmap='A00',
    auto_linebreaks=True,
    backlight_enabled=True
)

# ฟังก์ชัน callback เมื่อได้รับข้อความจาก MQTT
def on_message(client, userdata, msg):
    lcd.clear()
    try:
        # "แกะ" ข้อมูล JSON ที่ส่งมาจาก Arduino
        data = json.loads(msg.payload.decode())
        temp = data['temp']
        humid = data['humid']

        lcd.write_string(f"Temp: {temp:.2f} C")
        lcd.crlf()
        lcd.write_string(f"Humid: {humid:.2f} %")

        print(f"Received: Temp={temp:.2f}, Humid={humid:.2f}")

    except Exception as e:
        lcd.write_string("Data error")
        print("Error:", e)

# 2. แก้ไข MQTT
broker = "mqtt-dashboard.com"
topic = "66070084/final_test"

client = mqtt.Client()
client.on_message = on_message

print("Connecting to MQTT broker...")
client.connect(broker, 1883, 60)
client.subscribe(topic)

lcd.write_string("Waiting for data...")
client.loop_forever()