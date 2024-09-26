import pygame
import socket
import json
import time

# Initialize pygame and the gamepad
pygame.init()
pygame.joystick.init()

# Open the first connected joystick
if pygame.joystick.get_count() == 0:
    raise Exception("No joystick detected")
joystick = pygame.joystick.Joystick(0)
joystick.init()

# Raspberry Pi IP and port
HOST = '192.168.11.204'  # Replace with Raspberry Pi IP
PORT = 6500

# Create a socket and send data to the Raspberry Pi
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    # Stream gamepad inputs
    try:

        while True:
            # Poll for events
            pygame.event.pump()

            # Capture gamepad inputs
            axes = {f'axis_{i}': joystick.get_axis(i) for i in range(joystick.get_numaxes())}
            # buttons = {f'button_{i}': joystick.get_button(i) for i in range(joystick.get_numbuttons())}
            # hats = {f'hat_{i}': joystick.get_hat(i) for i in range(joystick.get_numhats())}

            # Create a dictionary of all the inputs
            # gamepad_state = {
            #     'axes': axes,
            #     'buttons': buttons,
            #     'hats': hats
            # }

            # Send data as JSON over the socket
            message = json.dumps(axes).encode()
            message_length = len(message)
            s.sendall(message_length.to_bytes(4, byteorder='big'))
            s.sendall(message)

            time.sleep(0.05)

    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        s.close()
        pygame.quit()
