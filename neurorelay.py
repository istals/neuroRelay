#! python

import os
import time
import config
import serial
import logging
import mindwave
import subprocess
import signal
import sys
import re


if config.DEBUG:
    logging.basicConfig(
        level=logging.DEBUG,
        format = '[%(asctime)s.%(msecs)03d] %(levelname)s [%(filename)s:%(lineno)d] - %(message)s', datefmt = '%H:%M:%S'
    )
else:
    logging.basicConfig(
        filename=config.LOG_PATH,
        level=logging.DEBUG,
        format = '[%(asctime)s.%(msecs)03d] %(levelname)s [%(filename)s:%(lineno)d] - %(message)s', datefmt = '%H:%M:%S'
    )

logger = logging.getLogger('mindwave')


NEURO_RELAY = None
SAME_VALUE_MAX = 17
KILL_SIGNAL_RECEIVED = False
PROGRAM_STOPPED = False


def callCommandLine(command):
    extractProc = subprocess.Popen([command], shell=True, bufsize=0,
                                   executable="/bin/bash", stdin=None, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdOut = extractProc.communicate()[0]
    returnCode = extractProc.returncode
    response = str(bytearray(stdOut).decode('utf-8'))
    return returnCode, response


class Statuses:
    DISCONNECTED = 0
    CONNECTING = 1
    CONNECTED = 2


class NeuroRelay:
    def __init__(self):
        self.millis        = 0
        self.headset       = None
        self.dongleStatus  = Statuses.DISCONNECTED
        self.headsetStatus = Statuses.DISCONNECTED
        self.attention = None
        self.lastAttention = 0
        self.sameValueCounter = 0

        self.killBluetooth()

    def init(self):
        # Arduino serial port setup
        if config.ARDUINO:
            self._serial = serial.Serial(
                port='/dev/serial0',
                baudrate=9600,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS
            )

            while not self._serial.isOpen():
                self.connectSerial()
                time.sleep(0.5)

        self.run()

    @staticmethod
    def killBluetooth():
        """Disable Bluetooth service"""
        logger.debug("Disable Bluetooth service")
        os.system("sudo service bluetooth stop")

    def connectSerial(self):
        """Serial port initialization"""
        logger.debug("Open serial port")
        try:
            self._serial.open()
        except Exception as err:
            logger.error("Error opening serial port - {}".format(err))


    def checkDongleStatus(self):
        command = 'find /sys/devices/ -name "ttyUSB*"'
        returnCode, response = callCommandLine(command)

        if 'ttyUSB' in response:
            splitted = response.split('\n')
            if len(splitted) == 3:
                donglePortName = splitted[0].split('/')[-1]
                self.dongleStatus = Statuses.CONNECTED
                return '/dev/' + donglePortName
        else:
            self.dongleStatus = Statuses.DISCONNECTED
            self.headsetStatus = Statuses.DISCONNECTED
            logger.warning("Mindwave device not detected")

        logger.info('none')
        return None

    def getAttentionHandler(self, headset_obj, attention):
        logger.debug('Attention: {}'.format(attention))
        attention = float(attention)
        self.attention = int((attention / 100) * 120)

    def checkHeadsetStatus(self):
        if self.headset and self.headset.status == 'connected':
            current_time = mindwave.current_milli_time()
            headset_last_message_time = self.headset.last_message_time

            if headset_last_message_time + 1000 < current_time:
                self.headset.disconnect()

        if not self.headset or self.headset.status != 'connected':
            self.headsetStatus = Statuses.DISCONNECTED

    def run(self):
        global KILL_SIGNAL_RECEIVED

        nextAttention = self.millis

        while True:
            if KILL_SIGNAL_RECEIVED:
                self.headset.force_disconnect()
                time.sleep(2)
                break

            try:
                proceed = True
                donglePortName = self.checkDongleStatus()
                if not donglePortName:
                    continue

                self.checkHeadsetStatus()

                if config.ARDUINO and not self._serial.isOpen():
                    proceed = False
                    self.connectSerial()

                if proceed and self.headsetStatus != Statuses.CONNECTED:
                    proceed = False
                    self.headsetConnect(donglePortName)

                if proceed and nextAttention < self.millis:
                    self.sendAttention()
                    nextAttention = self.millis + 5

                self.millis += 1
            except Exception as err:
                logger.error(err)
                pass
            time.sleep(.1)

    def headsetConnect(self, donglePortName):
        try:
            if not self.headset or not self.headset.is_open():
                self.headset = mindwave.Headset(donglePortName, config.MINDSET_ID)

                self.headset.attention_handlers.append(self.getAttentionHandler)

            logger.info('Connect to the Mindwave headset')
            if self.headset.status != 'connected':
                if self.headset.status == 'standby':
                    self.headsetStatus = Statuses.DISCONNECTED

                if self.headsetStatus != Statuses.CONNECTING:
                    self.headsetStatus = Statuses.CONNECTING
                    self.headset.connect(config.MINDSET_ID)
            else:
                logger.info('Mindwave headset connected')
                self.headsetStatus = Statuses.CONNECTED
        except Exception as err:
            logger.error(err)
            self.headset.disconnect()
            self.headsetStatus = Statuses.DISCONNECTED
            pass
        return

    def sendAttention(self):
        if not self.attention:
            return

        message = '%d\n' % self.attention
        if self.headset.poor_signal < 200:
            if config.ARDUINO:
                print('send to arduino')
                self._serial.write(message.encode('ascii'))

        self.lastAttention = self.attention
        self.attention = None
        return

    def disconnect(self):
        logger.debug('Disconnect headset manually')
        if self.headset:
            self.headset.disconnect()
            self.headset.serial_close()


def on_exit(signum, frame):
    global NEURO_RELAY, KILL_SIGNAL_RECEIVED, PROGRAM_STOPPED

    KILL_SIGNAL_RECEIVED = True

    while NEURO_RELAY.headset.status not in "standby":
        NEURO_RELAY.headset.disconnect()
        time.sleep(.5)

    NEURO_RELAY.headset.force_disconnect()
    time.sleep(1)

    sys.exit()


if __name__ == '__main__':
    logger.info('Start Script')
    # run main command
    signal.signal(signal.SIGINT, on_exit)
    signal.signal(signal.SIGTERM, on_exit)

    try:
        NEURO_RELAY = NeuroRelay()
        NEURO_RELAY.init()
        PROGRAM_STOPPED = True
    except Exception as error:
        logger.error(error)
        #os.system('sudo systemctl restart NeuroRelay.service')
