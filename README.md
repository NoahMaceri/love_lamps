# LampProject
Simple Lamp using Python/Raspberry Pi/Neopixel

Required PIP Packages:
sudo pip3 install adafruit-circuitpython-charlcd adafruit-circuitpython-neopixel adafruit-circuitpython-led-animation

To enable autostart:
Add "sudo python3 ~/main.py" to ~/.bashrc (without quotes)
Enable automatic console login with raspi-config
Add "pi       ALL=(ALL) NOPASSWD: python3" to sudoers file (sudo visudo to edit sudoers file and without quotes)
