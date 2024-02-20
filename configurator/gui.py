import sys
import serial
import serial.tools.list_ports as list_ports
import time
import yaml
import pathlib
from PySide2 import QtCore, QtGui, QtWidgets
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

        # get exe path (if frozen or not)
        cwd = ''
        if getattr(sys, 'frozen', False):
            cwd = pathlib.Path(sys.executable).parent
        else:
            cwd = pathlib.Path(__file__).parent
        
        # Load config file
        with open(cwd / 'config.yaml', 'r') as f:
            config = yaml.safe_load(f)

        self.lineEdit_wifi_ssid.setText(config['ssid'])
        self.lineEdit_wifi_pass.setText(config['pass'])
        self.lineEdit_target.setText(config['target'])
        self.lineEdit_id.setText(config['id'])
        self.lineEdit_mqtt_server.setText(config['mqtt_server'])
        self.lineEdit_mqtt_user.setText(config['mqtt_user'])
        self.lineEdit_mqtt_pass.setText(config['mqtt_pass'])
        self.spinBox_mqtt_port.setValue(config['mqtt_port'])

        self.get_board_info()

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
        self.ser.write("toggle_debug".encode('utf-8'))

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
        start_time = time.time()
        self.ser.write("board_info\n".encode('utf-8'))

        mac_adr = ''
        serial_num = ''
        version = ''

        while True:
            line = self.ser.readline()
            self.textBrowser_serial.append(line.decode('ascii'))
            QtWidgets.QApplication.processEvents()
            if 'MAC' in line.decode('ascii'):
                mac_adr = line.decode('ascii').split(' ')[3]
            if 'Serial' in line.decode('ascii'):
                serial_num = line.decode('ascii').split(' ')[3]
            if 'Version' in line.decode('ascii'):
                version = line.decode('ascii').split(' ')[4]
            if 'BOARD_INFO_END' in line.decode('ascii'):
                break
            time.sleep(0.025)

            if time.time() - start_time > 5:
                self.msg = QtWidgets.QMessageBox()
                self.msg.setIcon(QtWidgets.QMessageBox.Critical)
                self.msg.setText("Error: Board info not received")
                self.msg.setWindowTitle("Error")
                self.msg.exec_()
                sys.exit(-1)

        self.lineEdit_mac.setText(mac_adr)
        self.lineEdit_serial.setText(serial_num)
        self.lineEdit_llver.setText(version)

        self.ser.reset_input_buffer()
        self.textBrowser_serial.clear()

    def factory_reset(self):
        self.textBrowser_console.clear()
        self.textBrowser_serial.clear()

        self.reset_esp()

        self.textBrowser_console.append('Forcing ESP32 to clear NVS...')
        QtWidgets.QApplication.processEvents()
        self.ser.write("clear_nvs".encode('utf-8'))

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
                if 'INIT MODE' in line.decode('ascii') :
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
        self.reset_esp()

                # if any of the fields are empty, pop up error
        if self.lineEdit_wifi_ssid.text() == '' or self.lineEdit_wifi_pass.text() == '' or self.lineEdit_target.text() == '' or self.lineEdit_id.text() == '' or \
        self.lineEdit_mqtt_server.text() == '' or self.lineEdit_mqtt_user.text() == '' or self.lineEdit_mqtt_pass.text() == '':
            self.msg = QtWidgets.QMessageBox()
            self.msg.setIcon(QtWidgets.QMessageBox.Critical)
            self.msg.setText("Error: One or more fields are empty")
            self.msg.setWindowTitle("Error")
            self.msg.exec_()
            return

        config_cmd = f'config {self.lineEdit_wifi_ssid.text()} {self.lineEdit_wifi_pass.text()} {self.lineEdit_target.text()} {self.lineEdit_id.text()} {self.lineEdit_mqtt_server.text()} {str(self.spinBox_mqtt_port.value())} {self.lineEdit_mqtt_user.text()} {self.lineEdit_mqtt_pass.text()}\n'

        self.textBrowser_console.append('Configuring ESP32...')
        QtWidgets.QApplication.processEvents()
        self.ser.write(config_cmd.encode('utf-8'))

        while True:
            line = self.ser.readline()
            self.textBrowser_serial.append(line.decode('ascii'))
            QtWidgets.QApplication.processEvents()
            if 'Preferences stored' in line.decode('ascii'):
                self.textBrowser_console.append('ESP32 Configured!')
                QtWidgets.QApplication.processEvents()
                break

        time.sleep(1)
        self.reset_esp()

        self.finished_popup('Config set successfully!')

    def reset_esp(self):
        # Reset esp32
        self.textBrowser_console.append('Resetting ESP32...')
        QtWidgets.QApplication.processEvents()

        # reset ESP32

        self.ser.write("reset\n".encode('utf-8'))
        # Flush serial buffer
        self.ser.reset_input_buffer()

        while True:
            line = self.ser.readline()
            try:
                self.textBrowser_serial.append(line.decode('ascii'))
            except UnicodeDecodeError:
                continue
            QtWidgets.QApplication.processEvents()
            if 'INIT MODE' in line.decode('ascii') or 'Setup Complete' in line.decode('ascii'):
                self.textBrowser_console.append('ESP32 reset!')
                QtWidgets.QApplication.processEvents()
                break
            time.sleep(0.025)


if __name__ == '__main__':
    app = QtWidgets.QApplication([])
    window = MainWindow()
    window.show()
    app.exec_()
