from machine import Pin, Timer
from time import sleep_ms
import ubluetooth
from esp32 import raw_temperature
import machine

from micropython import const
_IRQ_CENTRAL_CONNECT                 = const(1 << 0)
_IRQ_CENTRAL_DISCONNECT              = const(1 << 1)
_IRQ_GATTS_WRITE                     = const(1 << 2)
_IRQ_ENCRYPTION_UPDATE = const(28)

_IRQ_PASSKEY_ACTION = const(31)
_PASSKEY_ACTION_NONE = const(0)
_PASSKEY_ACTION_INPUT = const(2)
_PASSKEY_ACTION_DISPLAY = const(3)
_PASSKEY_ACTION_NUMERIC_COMPARISON = const(4)

keyFromRemote = '243280'
keyFromLocal = '243281'
class ManagedDevice:
    def __init__(self):
        self.state = 0
# 0: Waiting for initial key

# 1:
    def On_Message(self, msg):
        if self.state == 0:
            self.state = 1
            return "randomNumberResponse"
        elif self.state == 1:
            if(msg != "blue_led"):
                self.state = 0
                return None
            else:
                return "ok"

deviceState = ManagedDevice()

class BLE():

    def __init__(self, name):

        self.name = name
        self.ble = ubluetooth.BLE()
        self.ble.active(True)

        self.led = Pin(2, Pin.OUT)
        self.timer1 = Timer(0)
        self.timer2 = Timer(1)

        self.ble.pa

        self.disconnected()
        self.ble.irq(self.ble_irq)
        self.register()
        self.advertiser()

    def connected(self):

        self.timer1.deinit()
        self.timer2.deinit()

    def disconnected(self):

        self.timer1.init(period=1000, mode=Timer.PERIODIC, callback=lambda t: self.led(1))
        sleep_ms(200)
        self.timer2.init(period=1000, mode=Timer.PERIODIC, callback=lambda t: self.led(0))

    def ble_irq(self, event, data):

        if event == _IRQ_CENTRAL_CONNECT:
            '''Central disconnected'''
            self.connected()
            self.ble.gatts_notify(0, self.tx, "data" + '\n')
            self.led(1)

        elif event == _IRQ_CENTRAL_DISCONNECT:
            '''Central disconnected'''
            self.advertiser()
            self.disconnected()

        elif event == _IRQ_GATTS_WRITE:
            '''New message received'''

            buffer = self.ble.gatts_read(self.rx)
            message = buffer.decode('UTF-8').strip()
            print(message)

            msg_res = deviceState.On_Message(message)
            if msg_res is None:
                machine.reset()
            else:
                self.ble.gatts_notify(0, self.tx, msg_res + '\n')
            if message == 'blue_led':
                blue_led.value(not blue_led.value())
        elif event == _IRQ_PASSKEY_ACTION:
            conn_handle, action, passkey = data
            print("passkey action", conn_handle, action, passkey)
            # passkey will be non-zero if action is "numeric comparison".
            if action == _PASSKEY_ACTION_NONE:
                pass
            elif action == _PASSKEY_ACTION_INPUT:
                self._ble.gap_passkey(conn_handle, action, keyFromRemote)
            elif action == _PASSKEY_ACTION_DISPLAY:
                self._ble.gap_passkey(conn_handle, action, keyFromLocal)
                pass
            elif action == _PASSKEY_ACTION_NUMERIC_COMPARISON:
                self._ble.gap_passkey(conn_handle, action, passkey == keyFromLocal)
                pass


    def register(self):

        # Nordic UART Service (NUS)
        NUS_UUID = '6E400001-B5A3-F393-E0A9-E50E24DCCA9E'
        RX_UUID = '6E400002-B5A3-F393-E0A9-E50E24DCCA9E'
        TX_UUID = '6E400003-B5A3-F393-E0A9-E50E24DCCA9E'

        BLE_NUS = ubluetooth.UUID(NUS_UUID)
        BLE_RX = (ubluetooth.UUID(RX_UUID), ubluetooth.FLAG_WRITE)
        BLE_TX = (ubluetooth.UUID(TX_UUID), ubluetooth.FLAG_NOTIFY)

        BLE_UART = (BLE_NUS, (BLE_TX, BLE_RX,))
        SERVICES = (BLE_UART,)
        ((self.tx, self.rx,),) = self.ble.gatts_register_services(SERVICES)

    def advertiser(self):
        name = bytes(self.name, 'UTF-8')
        self.ble.gap_advertise(100, bytearray('\x02\x01\x02') + bytearray((len(name) + 1, 0x09)) + name)


# test
blue_led = Pin(2, Pin.OUT)
ble = BLE("UnFoundBug.Lights")
