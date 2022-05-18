import time
import serial

s = serial.Serial('COM3', 9600, timeout = 1)

def read_till_none():
    running = True
    while running:
        new_line = s.readline()
        if not new_line:
            print(s.readline().decode("utf-8"))
            time.sleep(0.01)
        else:
            running = False

print("Waiting for startup")
time.sleep(4)

# Enumerate
b1 = 0xa5
b2 = 0x5a
b3 = 0x01
b4 = 0x01
b5 = 0x00
b6 = 0x00
b7 = 0x00
print(b3)
b8 = b1 + b2 + b3 + b4 + b5 + b6
b8 = b8 % 256
sendArray = bytearray([b1,b2,b3,b4,b5,b6,b7,b8])

print("About to enumerate")
s.write(sendArray)
read_till_none()
# Set actual light
b3 = 0x02
b4 = 1
b5 = 255
b8 = b1 + b2 + b3 + b4 + b5 + b6
b8 = b8 % 256
sendArray = bytearray([b1,b2,b3,b4,b5,b6,b7,b8])
s.write(sendArray)
read_till_none()
print("Actual On")
time.sleep(2)

# Clear actual light
b5 = 0
b8 = b1 + b2 + b3 + b4 + b5 + b6
b8 = b8 % 256
sendArray = bytearray([b1,b2,b3,b4,b5,b6,b7,b8])
s.write(sendArray)
read_till_none()
print("Actual Off")
time.sleep(0.5)

# Set wrong light
b4 = 2
b5 = 255
b8 = b1 + b2 + b3 + b4 + b5 + b6
b8 = b8 % 256
sendArray = bytearray([b1,b2,b3,b4,b5,b6,b7,b8])
s.write(sendArray)
read_till_none()
print("Wrong On")
time.sleep(1)
# Clear wrong light
b5 = 0
b8 = b1 + b2 + b3 + b4 + b5 + b6
b8 = b8 % 256
sendArray = bytearray([b1,b2,b3,b4,b5,b6,b7,b8])
s.write(sendArray)
read_till_none()
print("Wrong Off")
time.sleep(0.5)

for x in range(255):
    print("Pwm test" + str(x))
    s.write(sendArray)
    read_till_none()
    # Set actual light
    b3 = 0x02
    b4 = 1
    b5 = x
    b8 = b1 + b2 + b3 + b4 + b5 + b6
    b8 = b8 % 256
    sendArray = bytearray([b1,b2,b3,b4,b5,b6,b7,b8])
    s.write(sendArray)
    read_till_none()
    time.sleep(0.1)