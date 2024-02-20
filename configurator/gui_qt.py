# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'gui_qt.ui'
##
## Created by: Qt User Interface Compiler version 5.15.2
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide2.QtCore import *
from PySide2.QtGui import *
from PySide2.QtWidgets import *


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(800, 350)
        MainWindow.setAnimated(True)
        self.actionFactory_Reset = QAction(MainWindow)
        self.actionFactory_Reset.setObjectName(u"actionFactory_Reset")
        self.actionToggle_debug_mode = QAction(MainWindow)
        self.actionToggle_debug_mode.setObjectName(u"actionToggle_debug_mode")
        self.actionSoft_reset = QAction(MainWindow)
        self.actionSoft_reset.setObjectName(u"actionSoft_reset")
        self.actionBoard_info = QAction(MainWindow)
        self.actionBoard_info.setObjectName(u"actionBoard_info")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setSpacing(4)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.widget = QWidget(self.centralwidget)
        self.widget.setObjectName(u"widget")
        self.gridLayout = QGridLayout(self.widget)
        self.gridLayout.setObjectName(u"gridLayout")
        self.label_wifi_pass = QLabel(self.widget)
        self.label_wifi_pass.setObjectName(u"label_wifi_pass")
        font = QFont()
        font.setBold(True)
        font.setWeight(75)
        self.label_wifi_pass.setFont(font)
        self.label_wifi_pass.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_wifi_pass, 3, 0, 1, 1)

        self.lineEdit_mqtt_server = QLineEdit(self.widget)
        self.lineEdit_mqtt_server.setObjectName(u"lineEdit_mqtt_server")

        self.gridLayout.addWidget(self.lineEdit_mqtt_server, 2, 1, 1, 1)

        self.label_mqtt_server = QLabel(self.widget)
        self.label_mqtt_server.setObjectName(u"label_mqtt_server")
        self.label_mqtt_server.setFont(font)
        self.label_mqtt_server.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_mqtt_server, 1, 1, 1, 1)

        self.label_id = QLabel(self.widget)
        self.label_id.setObjectName(u"label_id")
        self.label_id.setFont(font)
        self.label_id.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_id, 7, 0, 1, 1)

        self.label_wifi_ssid = QLabel(self.widget)
        self.label_wifi_ssid.setObjectName(u"label_wifi_ssid")
        self.label_wifi_ssid.setFont(font)
        self.label_wifi_ssid.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_wifi_ssid, 1, 0, 1, 1)

        self.spinBox_mqtt_port = QSpinBox(self.widget)
        self.spinBox_mqtt_port.setObjectName(u"spinBox_mqtt_port")
        self.spinBox_mqtt_port.setMinimum(1)
        self.spinBox_mqtt_port.setMaximum(65535)
        self.spinBox_mqtt_port.setValue(1883)

        self.gridLayout.addWidget(self.spinBox_mqtt_port, 4, 1, 1, 1)

        self.lineEdit_wifi_pass = QLineEdit(self.widget)
        self.lineEdit_wifi_pass.setObjectName(u"lineEdit_wifi_pass")
        self.lineEdit_wifi_pass.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lineEdit_wifi_pass, 4, 0, 1, 1)

        self.label_mqtt_pass = QLabel(self.widget)
        self.label_mqtt_pass.setObjectName(u"label_mqtt_pass")
        self.label_mqtt_pass.setFont(font)
        self.label_mqtt_pass.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_mqtt_pass, 7, 1, 1, 1)

        self.lineEdit_target = QLineEdit(self.widget)
        self.lineEdit_target.setObjectName(u"lineEdit_target")
        self.lineEdit_target.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lineEdit_target, 8, 0, 1, 1)

        self.lineEdit_mqtt_pass = QLineEdit(self.widget)
        self.lineEdit_mqtt_pass.setObjectName(u"lineEdit_mqtt_pass")

        self.gridLayout.addWidget(self.lineEdit_mqtt_pass, 8, 1, 1, 1)

        self.write = QPushButton(self.widget)
        self.write.setObjectName(u"write")

        self.gridLayout.addWidget(self.write, 9, 0, 1, 2)

        self.label_mqtt_user = QLabel(self.widget)
        self.label_mqtt_user.setObjectName(u"label_mqtt_user")
        self.label_mqtt_user.setFont(font)
        self.label_mqtt_user.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_mqtt_user, 5, 1, 1, 1)

        self.lineEdit_wifi_ssid = QLineEdit(self.widget)
        self.lineEdit_wifi_ssid.setObjectName(u"lineEdit_wifi_ssid")
        self.lineEdit_wifi_ssid.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lineEdit_wifi_ssid, 2, 0, 1, 1)

        self.label_target = QLabel(self.widget)
        self.label_target.setObjectName(u"label_target")
        self.label_target.setFont(font)
        self.label_target.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_target, 5, 0, 1, 1)

        self.lineEdit_mqtt_user = QLineEdit(self.widget)
        self.lineEdit_mqtt_user.setObjectName(u"lineEdit_mqtt_user")

        self.gridLayout.addWidget(self.lineEdit_mqtt_user, 6, 1, 1, 1)

        self.lineEdit_id = QLineEdit(self.widget)
        self.lineEdit_id.setObjectName(u"lineEdit_id")
        self.lineEdit_id.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.lineEdit_id, 6, 0, 1, 1)

        self.label_mqtt_port = QLabel(self.widget)
        self.label_mqtt_port.setObjectName(u"label_mqtt_port")
        self.label_mqtt_port.setFont(font)
        self.label_mqtt_port.setAlignment(Qt.AlignCenter)

        self.gridLayout.addWidget(self.label_mqtt_port, 3, 1, 1, 1)

        self.label_info = QLabel(self.widget)
        self.label_info.setObjectName(u"label_info")

        self.gridLayout.addWidget(self.label_info, 0, 0, 1, 2)


        self.horizontalLayout.addWidget(self.widget)

        self.widget_2 = QWidget(self.centralwidget)
        self.widget_2.setObjectName(u"widget_2")
        self.verticalLayout = QVBoxLayout(self.widget_2)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.label_serial = QLabel(self.widget_2)
        self.label_serial.setObjectName(u"label_serial")
        self.label_serial.setFont(font)
        self.label_serial.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.label_serial)

        self.textBrowser_serial = QTextBrowser(self.widget_2)
        self.textBrowser_serial.setObjectName(u"textBrowser_serial")

        self.verticalLayout.addWidget(self.textBrowser_serial)

        self.label_console = QLabel(self.widget_2)
        self.label_console.setObjectName(u"label_console")
        self.label_console.setFont(font)
        self.label_console.setAlignment(Qt.AlignCenter)

        self.verticalLayout.addWidget(self.label_console)

        self.textBrowser_console = QTextBrowser(self.widget_2)
        self.textBrowser_console.setObjectName(u"textBrowser_console")

        self.verticalLayout.addWidget(self.textBrowser_console)


        self.horizontalLayout.addWidget(self.widget_2)

        self.widget_3 = QWidget(self.centralwidget)
        self.widget_3.setObjectName(u"widget_3")
        self.verticalLayout_3 = QVBoxLayout(self.widget_3)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.label_mac = QLabel(self.widget_3)
        self.label_mac.setObjectName(u"label_mac")
        self.label_mac.setFont(font)
        self.label_mac.setAlignment(Qt.AlignCenter)

        self.verticalLayout_3.addWidget(self.label_mac)

        self.lineEdit_mac = QLineEdit(self.widget_3)
        self.lineEdit_mac.setObjectName(u"lineEdit_mac")
        self.lineEdit_mac.setEnabled(True)
        self.lineEdit_mac.setReadOnly(True)

        self.verticalLayout_3.addWidget(self.lineEdit_mac)

        self.label_serial_2 = QLabel(self.widget_3)
        self.label_serial_2.setObjectName(u"label_serial_2")
        self.label_serial_2.setFont(font)
        self.label_serial_2.setAlignment(Qt.AlignCenter)

        self.verticalLayout_3.addWidget(self.label_serial_2)

        self.lineEdit_serial = QLineEdit(self.widget_3)
        self.lineEdit_serial.setObjectName(u"lineEdit_serial")
        self.lineEdit_serial.setEnabled(True)
        self.lineEdit_serial.setReadOnly(True)

        self.verticalLayout_3.addWidget(self.lineEdit_serial)

        self.label_llver = QLabel(self.widget_3)
        self.label_llver.setObjectName(u"label_llver")
        self.label_llver.setFont(font)
        self.label_llver.setAlignment(Qt.AlignCenter)

        self.verticalLayout_3.addWidget(self.label_llver)

        self.lineEdit_llver = QLineEdit(self.widget_3)
        self.lineEdit_llver.setObjectName(u"lineEdit_llver")
        self.lineEdit_llver.setEnabled(True)
        self.lineEdit_llver.setReadOnly(True)

        self.verticalLayout_3.addWidget(self.lineEdit_llver)


        self.horizontalLayout.addWidget(self.widget_3)

        self.horizontalLayout.setStretch(0, 2)
        self.horizontalLayout.setStretch(1, 3)
        self.horizontalLayout.setStretch(2, 1)
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.menuBar = QMenuBar(MainWindow)
        self.menuBar.setObjectName(u"menuBar")
        self.menuBar.setGeometry(QRect(0, 0, 800, 21))
        self.menuTools = QMenu(self.menuBar)
        self.menuTools.setObjectName(u"menuTools")
        MainWindow.setMenuBar(self.menuBar)

        self.menuBar.addAction(self.menuTools.menuAction())
        self.menuTools.addAction(self.actionFactory_Reset)
        self.menuTools.addAction(self.actionToggle_debug_mode)
        self.menuTools.addAction(self.actionSoft_reset)

        self.retranslateUi(MainWindow)

        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"Love Lamps Configurator", None))
        self.actionFactory_Reset.setText(QCoreApplication.translate("MainWindow", u"Clear NVS", None))
        self.actionToggle_debug_mode.setText(QCoreApplication.translate("MainWindow", u"Toggle debug mode", None))
        self.actionSoft_reset.setText(QCoreApplication.translate("MainWindow", u"Soft reset", None))
        self.actionBoard_info.setText(QCoreApplication.translate("MainWindow", u"Board info", None))
        self.label_wifi_pass.setText(QCoreApplication.translate("MainWindow", u"WiFi Password", None))
        self.label_mqtt_server.setText(QCoreApplication.translate("MainWindow", u"MQTT Server", None))
        self.label_id.setText(QCoreApplication.translate("MainWindow", u"Board Pair", None))
        self.label_wifi_ssid.setText(QCoreApplication.translate("MainWindow", u"WiFi SSID", None))
        self.label_mqtt_pass.setText(QCoreApplication.translate("MainWindow", u"MQTT Password", None))
        self.write.setText(QCoreApplication.translate("MainWindow", u"Write", None))
        self.label_mqtt_user.setText(QCoreApplication.translate("MainWindow", u"MQTT Username", None))
        self.label_target.setText(QCoreApplication.translate("MainWindow", u"Board User", None))
        self.label_mqtt_port.setText(QCoreApplication.translate("MainWindow", u"MQTT Port", None))
        self.label_info.setText("")
        self.label_serial.setText(QCoreApplication.translate("MainWindow", u"Serial", None))
        self.label_console.setText(QCoreApplication.translate("MainWindow", u"Console", None))
        self.label_mac.setText(QCoreApplication.translate("MainWindow", u"MAC Address", None))
        self.label_serial_2.setText(QCoreApplication.translate("MainWindow", u"Serial Number", None))
        self.label_llver.setText(QCoreApplication.translate("MainWindow", u"Love Lamps Version", None))
        self.menuTools.setTitle(QCoreApplication.translate("MainWindow", u"Tools", None))
    # retranslateUi

