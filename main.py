#NMACERI - 11/22/2021
#CONFIDENTAL - DO NOT SHARE

import RPi.GPIO as GPIO
from adafruit_led_animation.animation.blink import Blink
from adafruit_led_animation.animation.comet import Comet
from adafruit_led_animation.animation.rainbowsparkle import RainbowSparkle
from adafruit_led_animation.animation.pulse import Pulse
from adafruit_led_animation.animation.sparkle import Sparkle
from adafruit_led_animation.animation.rainbowcomet import RainbowComet
from adafruit_led_animation.sequence import AnimationSequence
from adafruit_led_animation.sequence import AnimateOnce
from adafruit_led_animation.color import *
import smtplib, ssl, imaplib, time, board, neopixel, threading, digitalio, email

#The GPIO is setup as BCM through the LCD init function, use GPIO names not pins numbers


#LCD SETUP
i2C_LCD = False

# Define LCD column and row size for 16x2 LCD.
lcd_columns = 16
lcd_rows = 2

if i2C_LCD:
    import adafruit_character_lcd.character_lcd_i2c as character_lcd
    import busio

    # LCD Pin out (I2C Conf)
    i2c = busio.I2C(board.SCL, board.SDA)

    # Setup LCD screen
    lcd = character_lcd.Character_LCD_I2C(i2c, lcd_columns, lcd_rows)

else:
    import adafruit_character_lcd.character_lcd as characterlcd

    # NPN transistor - Emitter connected to ground of LCD backlight, Base connected to GPIO 2, Collector connected to ground
    # This effectively toggles the backlight on and off on the LCD
    lcd_backlight = 2
    GPIO.setup(lcd_backlight, GPIO.OUT)

    # LCD Pin out
    lcd_rs = digitalio.DigitalInOut(board.D25)
    lcd_en = digitalio.DigitalInOut(board.D24)
    lcd_d4 = digitalio.DigitalInOut(board.D23)
    lcd_d5 = digitalio.DigitalInOut(board.D17)
    lcd_d6 = digitalio.DigitalInOut(board.D18)
    lcd_d7 = digitalio.DigitalInOut(board.D22)

    # Setup LCD screen (disable blink and cursor bc its annoying)
    lcd = characterlcd.Character_LCD_Mono(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7, lcd_columns, lcd_rows)

#General LCD Setup
lcd.clear()
lcd.blink = False
lcd.cursor = False


#RGB Strip Setup
pixel_pin = board.D12 #GPIO 12
num_pixels = 60 # The number of NeoPixels
ORDER = neopixel.GRB

#Buttons
RIGHT = 4
ENTER = 15
LEFT = 16

#Button GPIO setup
GPIO.setup(ENTER, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(LEFT, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(RIGHT, GPIO.IN, pull_up_down=GPIO.PUD_UP)

#LED Strip Setup
pixels = neopixel.NeoPixel(pixel_pin, num_pixels, brightness=0.2, auto_write=False, pixel_order=ORDER)

##Global Variable configs
#Email configs
port = 465
username = "thelampproject2021@gmail.com"
password = "thelamppog2021!"
smtp_server = "smtp.gmail.com"
sender_email = username
receiver_email = username

#Message buffer + index
message_list = ["I love you", "Hey!", "Check your phone", "I miss you", "Goodnight <3", "Good morning :)", "POG", "I need help!", "*Flashbang*"]
message_index = 0

#User Settings
userList = ['Noah', 'Gavin', 'Jordan', 'Liz']
deviceUser = userList[0]
targetUserIndex = 0 #Now this might seem stupid but trust me, it is
targetUser = userList[targetUserIndex]

#Backlight toggle
def backlight_toggle(state):
    if i2C_LCD and state:
        lcd.backlight = True
    elif i2C_LCD and not state:
        lcd.backlight = False

    if not i2C_LCD and state:
        GPIO.output(lcd_backlight, True)
    elif not i2C_LCD and not state:
        GPIO.output(lcd_backlight, False)

#Tab through users
def user_button(channel):
    global targetUserIndex
    global targetUser
    backlight_toggle(True)
    if targetUserIndex == 3:
        targetUserIndex = 0
    else:
        targetUserIndex += 1
    lcd.clear()
    lcd.message = "Selected " + userList[targetUserIndex]
    targetUser = userList[targetUserIndex]

#Tab through messages
def message_button(channel):
    global message_index
    backlight_toggle(True)
    if message_index == 8:
        message_index = 0
    else:
        message_index += 1
    lcd.clear()
    lcd.message = "Message: \n" + message_list[message_index]

#When the enter button is pressed it displays what is being sent, sends it, then lets the user know
def send_button(channel):
    global message_index
    backlight_toggle(True)
    lcd.clear()
    lcd.message = "To: " + targetUser + "\n" + message_list[message_index]
    time.sleep(0.25)
    message = targetUser + "-" + deviceUser + "-" + message_list[message_index]
    with smtplib.SMTP_SSL("smtp.gmail.com", port, context=ssl.create_default_context()) as server:
        server.login(username, password)
        server.sendmail(sender_email, receiver_email, message)
    lcd.clear()
    lcd.message = "Sent to " + targetUser

#Given a raw byte email, return the payload
def get_body_from_email(msg):
    if msg.is_multipart():
        return get_body_from_email(msg.get_payload(0))
    else:
        return msg.get_payload(None, True)

#Returns the latest email in the inbox (raw bytes)
def grab_latest_email(mail_obj):
    # Grab latest email
    mail_obj.list()
    mail_obj.select("inbox")
    result, data = mail_obj.search(None, "ALL")
    ids = data[0]
    id_list = ids.split()
    latest_email_id = id_list[-1] #Latest email ID
    result, data = mail_obj.fetch(latest_email_id, "(RFC822)") #Store the latest email in data
    # Returns the latest email in the inbox
    return data[0][1]

#Kills the LED strip, clears the LCD, and toggles the backlight off
def off():
    global pixels
    pixels.fill((0, 0, 0))
    pixels.show()
    backlight_toggle(False)
    lcd.clear()

#So here's the thing, this code is hot garbage and has band-aids on band-aids but guess what it works
if __name__ == "__main__":

    #Version info
    lcd.clear()
    backlight_toggle(True)
    lcd.message = "  Message Lamp\n  Version 0.95"
    time.sleep(5)

    #Turn off LED and clear LCD
    off()

    #Interupt handlers for the buttons
    GPIO.add_event_detect(LEFT, GPIO.FALLING, callback=user_button, bouncetime=550)
    GPIO.add_event_detect(RIGHT, GPIO.FALLING, callback=message_button, bouncetime=550)
    GPIO.add_event_detect(ENTER, GPIO.FALLING, callback=send_button, bouncetime=550)

    #To check data time (fixes date time bug)
    oldEmail = ""

    #Login to the imap server
    mail = imaplib.IMAP4_SSL('imap.gmail.com')
    mail.login(username, password)

    while True:
        #get the latest email
        try:
            #Grab the un-decoded email (fixes time/date bug)
            fullEmail = grab_latest_email(mail)
            #decode the latest email
            latestEmail = (get_body_from_email(email.message_from_bytes(fullEmail))).decode("utf-8")
        except imaplib.IMAP4.abort: #If the IMAP server times out, login again
            print("Email server re-logging")
            #You have to recreate the imap class or it won't log in
            mail = imaplib.IMAP4_SSL('imap.gmail.com')
            mail.login(username, password)
        finally:
            fullEmail = grab_latest_email(mail)
            latestEmail = (get_body_from_email(email.message_from_bytes(fullEmail))).decode("utf-8")
        emailSplit = latestEmail.split("-")
        #0 - targetUser
        #1 - sendingUser
        #2 - payload

        if oldEmail != fullEmail:
            #check for receiver name, if not found, do not update
            if emailSplit[0] == deviceUser:
                backlight_toggle(True)
                #Animations have to be recreated every time bc of the animation library
                if latestEmail.find(message_list[0]) > 0: #I love you
                    heartbeat_pulse = Pulse(pixels, speed=0.01, color=RED, period=1.25)
                    heartbeat = AnimationSequence(heartbeat_pulse, advance_interval=5, auto_clear=True, auto_reset=True)
                    heartbeatAnim = AnimateOnce(heartbeat, auto_clear=True, auto_reset=True)
                    while heartbeatAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[0]
                elif latestEmail.find(message_list[1]) > 0: #Hey!
                    hello = Pulse(pixels, speed=0.01, color=GREEN, period=5)
                    helloAnim = AnimateOnce(hello, auto_clear=True, auto_reset=True)
                    while helloAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[1]
                elif latestEmail.find(message_list[2]) > 0: #Check your phone
                    checkPhone = RainbowSparkle(pixels, speed=0.1, period=5, num_sparkles=5, step=1, background_brightness=0.2, precompute_rainbow=True)
                    checkPhoneAnim = AnimateOnce(checkPhone, auto_clear=True, auto_reset=True)
                    while checkPhoneAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[2]
                elif latestEmail.find(message_list[3]) > 0: #I miss you
                    miss1 = Comet(pixels, speed=0.042, color=PURPLE, tail_length=10, bounce=True)
                    miss2 = Comet(pixels, speed=0.042, color=BLUE, tail_length=10, bounce=True)
                    miss = AnimationSequence(miss1, miss2, advance_interval=3, auto_clear=True, auto_reset=True)
                    missAnim = AnimateOnce(miss, auto_clear=True, auto_reset=True)
                    while missAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[3]
                elif latestEmail.find(message_list[4]) > 0: #Goodnight <3
                    goodnight_sparkles = Sparkle(pixels, speed=0.5, color=OLD_LACE, num_sparkles=2)
                    goodnight = AnimationSequence(goodnight_sparkles, advance_interval=5, auto_clear=True, auto_reset=True)
                    goodnightAnim = AnimateOnce(goodnight, auto_clear=True, auto_reset=True)
                    while goodnightAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[4]
                elif latestEmail.find(message_list[5]) > 0: #Good Morning :)
                    goodMorning = Pulse(pixels, speed=0.01, color=YELLOW, period=5)
                    goodMorningAnim = AnimateOnce(goodMorning, auto_clear=True, auto_reset=True)
                    while goodMorningAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[5]
                elif latestEmail.find(message_list[6]) > 0: #POG
                    pog_comet = RainbowComet(pixels, speed=0.01, tail_length=10, bounce=True, colorwheel_offset=0, step=25)
                    pog = AnimationSequence(pog_comet, advance_interval=5, auto_clear=True, auto_reset=True)
                    pogAnim = AnimateOnce(pog, auto_clear=True, auto_reset=True)
                    while pogAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[6]
                elif latestEmail.find(message_list[7]) > 0: #I need help!
                    help1 = Blink(pixels, speed=0.1, color=RED)
                    help2 = Blink(pixels, speed=0.1, color=BLUE)
                    help_group = AnimationSequence(help1, help2, advance_interval=0.1, auto_clear=True, auto_reset=True)
                    helpMe = AnimationSequence(help_group, advance_interval=5, auto_clear=True, auto_reset=True)
                    helpMeAnim = AnimateOnce(helpMe, auto_clear=True, auto_reset=True)
                    while helpMeAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[7]
                elif latestEmail.find(message_list[8]) > 0: #FLASHBANG OUT
                    flashbang = Pulse(pixels, speed=0.1, color=WHITE, period=5)
                    flashbangAnim = AnimateOnce(flashbang, auto_clear=True, auto_reset=True)
                    while flashbangAnim.animate():
                        pass
                    lcd.clear()
                    lcd.message = "From: " + emailSplit[1] + "\n" + message_list[8]

            # Interrupt handler for automatic clearing (shutdown after x seconds of downtime)
            t = threading.Timer(10, off)
            t.start()

        #The fullEmail contains the date time so set it so we can check it again
        oldEmail = fullEmail
        #Debounce/rate limit
        time.sleep(2)