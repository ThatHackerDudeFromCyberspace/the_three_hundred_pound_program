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

path = os.getenv("HOME", "/tmp") + "/300_fifo"
if (not os.path.exists(path)):
    print("ERR: Could not open fifo file")
    exit(1)


def sendMessage(hand: int, msgPath:str, msgParam: str):
    path = os.getenv("HOME", "/tmp") + "/300_fifo"
    print(f"Sending to path: {path}")
    toWrite = hand.to_bytes(1, 'little') + len(msgPath).to_bytes(4, 'little') + bytes(msgPath, 'ascii') + len(msgParam).to_bytes(4, 'little') + bytes(msgParam, 'ascii')
    print(f"Writing: {toWrite}")
    with open(path, 'wb') as file:
        file.write(toWrite)

# Example: hold the left joystick button down for 1 second
sendMessage(0, "/input/joystick/click", "1")
time.sleep(1)
sendMessage(0, "/input/joystick/click", "0")

# Example: hold the right joystick button down for 1 second
time.sleep(1)
sendMessage(1, "/input/joystick/click", "1")
time.sleep(1)
sendMessage(1, "/input/joystick/click", "0")

# Example: move right joystick x-axis
time.sleep(1)
sendMessage(1, "/input/joystick/x", "1.0")
time.sleep(1)
sendMessage(1, "/input/joystick/x", "-1.0")
time.sleep(1)
sendMessage(1, "/input/joystick/x", "0")
