# Import Libraries
import os # Gives Python access to Linux commands
import time # Proves time related commands
from gpiozero import Buzzer # The GPIO Zero buzzer functions
from time import sleep
# Set pin 22 as a buzzer
buzzer = Buzzer(22)

while True:
    #buzzer.beep()
    buzzer.on()
    sleep(0.005)
    buzzer.off()
    sleep(0.005)

    
