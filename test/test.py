import os
import time

###
# Message spec
###
# Path is just the path to the input
# Param is just what to set it to
# Param is sent as a string don't ask
# IE:
# Float values:
# /input/joystick/x 1.0
# /input/joystick/y 0.6
# Boolean values:
# /input/joystick/click 1
#
# The hand is either 0 or 1
# 0 - left hand
# 1 - right hand
#
# Yeah I know this format is weird just... don't ask mkay? thx

def sendMessage(hand: int, path: str, value: str, fifoPath="/tmp"):
    fifoPath = fifoPath + "/three_hundred_driver_fifo"
    print(f"Setting hand {hand} {path} to {value} (fifo path {fifoPath})")
    toWrite = hand.to_bytes(1, 'little') + len(path).to_bytes(4, 'little') + bytes(path, 'ascii') + len(value).to_bytes(4, 'little') + bytes(value, 'ascii')
    with open(fifoPath, 'wb') as file:
        file.write(toWrite)
