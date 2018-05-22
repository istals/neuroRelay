#! python

import os
import time
import RPi.GPIO as GPIO


BUTTON_TRIGGER_PIN = 35
LED_INDICATOR_PIN = 7

def main():
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(BUTTON_TRIGGER_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    GPIO.setup(LED_INDICATOR_PIN, GPIO.OUT)

    GPIO.output(LED_INDICATOR_PIN, GPIO.LOW)
    counter = 0
    pressed = False
    while True:
        if GPIO.input(BUTTON_TRIGGER_PIN) == GPIO.LOW:
            pressed = True
        elif GPIO.input(BUTTON_TRIGGER_PIN) == GPIO.HIGH:
            if pressed:
                if counter < 5:
                   ntries = 5
                   while os.system('sudo systemctl restart NeuroRelay.service') != 0 and ntries > 0:
                       time.sleep(1)
                       ntries -= 1
                else:
                    GPIO.output(LED_INDICATOR_PIN, GPIO.HIGH)
                    os.system('sudo shutdown now')
                pressed = False
                counter = 0

        if pressed:
            counter += 1
        time.sleep(1)


if __name__ == '__main__':
    # run main command
    try:
        main()
    except Exception as error:
        os.system('sudo systemctl restart shutdownButton.service')
        pass
