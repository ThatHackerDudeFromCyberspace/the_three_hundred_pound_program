def set_input(hand, path, value):
    if type(value) == float:
        value = str(value)
    elif type(value) == bool:
        value = "1" if value else "0"
    to_write = hand.to_bytes(1, 'little') + len(path).to_bytes(4, 'little') + bytes(path, 'ascii') + len(value).to_bytes(4, 'little') + bytes(value, 'ascii')
    with open("/tmp/three_hundred_driver_fifo", 'wb') as fifo:
        fifo.write(to_write)
