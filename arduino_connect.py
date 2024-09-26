import os
import serial
import time
from sshkeyboard import listen_keyboard
import socket
import os
import serial
import time
import json
import numpy as np

class ArduinoConnect():
    def __init__(self):
        self.keys_pressed = set()
        self.moving = False
        self.quit = False
        
    def open_serial_port(self, baudrate=19200, port='/dev/ttyUSB0'):
        self.ser = serial.Serial()
        self.ser.baudrate = baudrate
        self.ser.port = port
        self.ser.open()
        time.sleep(2.00)  # Wait for connection before sending any data
    
    def close_serial_port(self):
        self.ser.close()

    def send_keyboard_command(self):
        if 'up' in self.keys_pressed:
            print("Forward")
            self.ser.write(b'KB_f')
            self.moving = True
        elif 'down' in self.keys_pressed:
            print("Backward")
            self.ser.write(b'KB_b')
            self.moving = True
        elif 'left' in self.keys_pressed:
            print("Left")
            self.ser.write(b'KB_l')
            self.moving = True
        elif 'right' in self.keys_pressed:
            print("Right")
            self.ser.write(b'KB_r')
            self.moving = True
        elif 'q' in self.keys_pressed:
            print("Stop")
            self.ser.write(b'KB_x')
            self.moving = False
        else:
            if self.moving:  # Only stop if previously moving
                print("Slow")
                self.ser.write(b'KB_s')
                self.moving = False

    def keyboard_press(self, key): 
        if key == "q":
            print("Quit")
            self.ser.write(b'KB_x')
            self.moving = False
            self.quit = True
            time.sleep(0.05)
            return False
        self.keys_pressed.add(key)
        self.send_keyboard_command()
        return True

    def keyboard_release(self, key):
        if key in self.keys_pressed:
            self.keys_pressed.remove(key)
        self.send_keyboard_command()
        return True
    
    # Start the keyboard listener for control
    def start_keyboard_listener(self):
        listen_keyboard(on_press=self.keyboard_press, on_release=self.keyboard_release, delay_second_char=0.10, until="q")
        # Turn on key echo
        os.system("stty echo")

    def start_gamepad_listener(self, host, port):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.bind((host, port))
            s.listen()
            conn, addr = s.accept()
            with conn:
                print(f'Connected by {addr}')
                while True:
                    # First, receive the length of the message
                    length_bytes = conn.recv(4)
                    if not length_bytes:
                        break  # Connection closed
                    
                    message_length = int.from_bytes(length_bytes, byteorder='big')

                    # Now, receive the actual message based on the length
                    message = b''
                    while len(message) < message_length:
                        chunk = conn.recv(message_length - len(message))
                        if not chunk:
                            break  # Connection closed
                        message += chunk
                    
                    # Decode the message
                    received_dict = json.loads(message.decode())
                    # print(f"Received dictionary: {received_dict}")
                
                    up_down = received_dict['axis_1']
                    left_right = received_dict['axis_2']
                    info = ("GP", format(up_down, "10.2f"), format(left_right, "10.2f"))
                    message = '_'.join(x for x in info)+"\n"
                    print(message)
                    self.ser.reset_output_buffer()
                    self.ser.write(message.encode())
                    time.sleep(0.05)
