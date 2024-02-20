import sys
import serial
import serial.tools.list_ports as list_ports
import json
import time

from PyQt5 import QtCore, QtGui, QtWidgets
from gui_qt import Ui_MainWindow


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.setupUi(self)

        # Fix window size
        self.setFixedSize(self.size())

        self.COM_PORT = None
        ports = list(list_ports.comports())
        self.tmp = QtWidgets.QInputDialog()
        self.tmp.setWindowTitle("Board selection")
        self.tmp.setComboBoxItems([f"{p.device} | {p.description}" for p in ports])
        self.tmp.setLabelText("Select COM port:")
        self.tmp.setOkButtonText("Select")
        self.tmp.setCancelButtonText("Cancel")
        # if the user cancels the dialog, exit the program
        if not self.tmp.exec_():
            sys.exit(-1)
        self.COM_PORT = self.tmp.textValue().split(' | ')[0]

        self.label_info.setText(f'{self.COM_PORT} | {ports[0].description}')

        # Connect buttons
        self.write.clicked.connect(self.config_board)

        # Connect menu items
        self.actionFactory_Reset.triggered.connect(self.factory_reset)
        self.actionToggle_debug_mode.triggered.connect(self.toggle_debug)
        self.actionSoft_reset.triggered.connect(self.reset_esp)
        self.actionBoard_info.triggered.connect(self.get_board_info)

        try:
            self.ser = serial.Serial(self.COM_PORT, 115200, timeout=1)
        except serial.SerialException:
            # pop up error
            self.msg = QtWidgets.QMessageBox()
            self.msg.setIcon(QtWidgets.QMessageBox.Critical)
            self.msg.setText("Error: Cannot open serial port")
            self.msg.setWindowTitle("Error")
            self.msg.exec_()
            sys.exit(-1)

    def finished_popup(self, msg):
        self.msg = QtWidgets.QMessageBox()
        self.msg.setIcon(QtWidgets.QMessageBox.Information)
        self.msg.setText(msg)
        self.msg.setWindowTitle("Success")
        self.msg.exec_()

    def toggle_debug(self):
        self.textBrowser_console.clear()
        self.textBrowser_serial.clear()

        self.textBrowser_console.append('Forcing ESP32 to toggle debug mode...')
        QtWidgets.QApplication.processEvents()
        self.ser.write("CMD_TOGGLE_DEBUG".encode('utf-8'))

        self.textBrowser_console.append('Waiting for ESP32 to report debug status...')
        QtWidgets.QApplication.processEvents()

        while True:
            line = self.ser.readline()
            self.textBrowser_serial.append(line.decode('ascii'))
            QtWidgets.QApplication.processEvents()
            if 'Debug mode' in line.decode('ascii'):
                self.textBrowser_console.append('ESP32 reported debug status (see serial output)')
                QtWidgets.QApplication.processEvents()
                break
            time.sleep(0.025)

        time.sleep(1)
        self.ser.reset_input_buffer()
        self.reset_esp()
        self.finished_popup('Debug mode toggled!')

    def get_board_info(self):
        self.textBrowser_console.clear()
        self.textBrowser_serial.clear()

        self.textBrowser_console.append('Asking ESP32 for board info...')
        QtWidgets.QApplication.processEvents()
        self.ser.write("CMD_BOARD_INFO".encode('utf-8'))

        self.textBrowser_console.append('Waiting for ESP32 to report board info...')
        QtWidgets.QApplication.processEvents()

        while True:
            line = self.ser.readline()
            self.textBrowser_serial.append(line.decode('ascii'))
            QtWidgets.QApplication.processEvents()
            if 'BOARD_INFO_END' in line.decode('ascii'):
                self.textBrowser_console.append('ESP32 reported board info (see serial output)')
                QtWidgets.QApplication.processEvents()
                break
            time.sleep(0.025)

        self.ser.reset_input_buffer()
        self.finished_popup('Board info retrieved!')

    def factory_reset(self):
        self.textBrowser_console.clear()
        self.textBrowser_serial.clear()

        self.reset_esp()

        self.textBrowser_console.append('Forcing ESP32 to clear NVS...')
        QtWidgets.QApplication.processEvents()
        self.ser.write("CMD_CLEAR_NVS".encode('utf-8'))

        # Wait for "NVS cleared"
        self.textBrowser_console.append('Waiting for ESP32 to clear NVS...')
        QtWidgets.QApplication.processEvents()

        while True:
            line = self.ser.readline()
            try:
                self.textBrowser_serial.append(line.decode('ascii'))
            except UnicodeDecodeError:
                pass
            QtWidgets.QApplication.processEvents()
            try:
                if 'NVS cleared' in line.decode('ascii') or 'BOARD_READY' in line.decode('ascii'):
                    self.textBrowser_console.append('ESP32 cleared NVS!')
                    QtWidgets.QApplication.processEvents()
                    break
            except UnicodeDecodeError:
                pass

        time.sleep(1)
        self.finished_popup('Factory reset complete!')

    def config_board(self):
        self.textBrowser_console.clear()
        self.textBrowser_serial.clear()

        # Reset esp32
        # self.reset_esp()

        config_dict = {
            'wifi_ssid': self.lineEdit_wifi_ssid.text(),
            'wifi_pass': self.lineEdit_wifi_pass.text(),
            'board_target': self.lineEdit_target.text(),
            'board_id': self.lineEdit_id.text()
        }
        # if any of the fields are empty, pop up error
        if config_dict['wifi_ssid'] == '' or config_dict['board_target'] == '' or config_dict['board_id'] == '':
            self.msg = QtWidgets.QMessageBox()
            self.msg.setIcon(QtWidgets.QMessageBox.Critical)
            self.msg.setText("Error: One or more fields are empty")
            self.msg.setWindowTitle("Error")
            self.msg.exec_()
            return
        # convert to json
        config = json.dumps(config_dict)

        self.textBrowser_console.append('Forcing ESP32 to enter config mode...')
        QtWidgets.QApplication.processEvents()
        self.ser.write("CMD_CONFIG_NVS".encode('utf-8'))

        # Wait for "Waiting for config string..."
        self.textBrowser_console.append('Waiting for ESP32 to enter config mode...')
        QtWidgets.QApplication.processEvents()

        while True:
            line = self.ser.readline()
            self.textBrowser_serial.append(line.decode('ascii'))
            QtWidgets.QApplication.processEvents()
            if 'CONFIG_START' in line.decode('ascii'):
                self.textBrowser_console.append('ESP32 entered config mode!')
                QtWidgets.QApplication.processEvents()
                break

        # Send config string
        self.textBrowser_console.append('Sending config...')
        QtWidgets.QApplication.processEvents()
        self.ser.write(config.encode('utf-8'))
        while True:
            line = self.ser.readline()
            self.textBrowser_serial.append(line.decode('ascii'))
            QtWidgets.QApplication.processEvents()
            if 'CONFIG_END' in line.decode('ascii'):
                break
            self.ser.write(config.encode('utf-8'))

        # Print success message
        self.textBrowser_console.append('Config set successfully!')
        QtWidgets.QApplication.processEvents()

        time.sleep(1)
        self.finished_popup('Config set successfully!')

    def reset_esp(self):
        # Reset esp32
        self.textBrowser_console.append('Resetting ESP32...')
        QtWidgets.QApplication.processEvents()

        # reset ESP32

        self.ser.write("CMD_RESET".encode('utf-8'))
        # Flush serial buffer
        self.ser.reset_input_buffer()

        while True:
            line = self.ser.readline()
            try:
                self.textBrowser_serial.append(line.decode('ascii'))
            except UnicodeDecodeError:
                continue
            QtWidgets.QApplication.processEvents()
            if 'BOARD_READY' in line.decode('ascii'):
                self.textBrowser_console.append('ESP32 reset!')
                QtWidgets.QApplication.processEvents()
                break
            time.sleep(0.025)


if __name__ == '__main__':
    app = QtWidgets.QApplication([])
    window = MainWindow()
    window.show()
    app.exec_()
