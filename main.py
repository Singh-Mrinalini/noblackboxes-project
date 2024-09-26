import camera_stream
from threading import Thread
from arduino_connect import ArduinoConnect

BAUDRATE = 19200
PORT = '/dev/ttyUSB0'

arduino = ArduinoConnect()
arduino.open_serial_port(baudrate=BAUDRATE, port=PORT)

# Set up a server on the Raspberry Pi
HOST = '192.168.11.204'  
PORT = 6500   # Arbitrary port number

# Start both functionalities in separate threads
streaming_thread = Thread(target=camera_stream.start_server)
# keyboard_thread = Thread(target=arduino.start_keyboard_listener)
gamepad_thread = Thread(target=arduino.start_gamepad_listener, args=(HOST, PORT))

streaming_thread.start()
# keyboard_thread.start()
gamepad_thread.start()

streaming_thread.join()
# keyboard_thread.join()
gamepad_thread.join()

arduino.close_serial_port()




