import sys
import serial
import time
from threading import Thread

from PyQt4.QtCore import *
from PyQt4.QtGui import *
from PyQt4 import QtGui
from PyQt4.QtGui import QFont
 
# Create an PyQT4 application object.
a = QApplication(sys.argv)
 
# The QWidget widget is the base class of all user interface objects in PyQt4.
w = QWidget()
p=w.palette()
p.setColor(w.backgroundRole(), QColor('black'));
w.setPalette(p);
# Set window size.
w.resize(1024, 600)
# Set window title
w.setWindowTitle("Canfield STEM Camp RoboCup 2017")
 
# Create widget
#label = QLabel(w)
#pixmap = QPixmap('/home/nick/Desktop/logo.png')
#label.setPixmap(pixmap)
#w.resize(pixmap.width(),pixmap.height())

lcdNumber = QLCDNumber(w)
lcdNumber.setGeometry(QRect(300, 500, 191, 81))
lcdNumber.setObjectName("lcdNumber")

lcdNumber1 = QLCDNumber(w)
lcdNumber1.setGeometry(QRect(24, 500, 191, 81))
lcdNumber1.setObjectName("lcdNumber1")

lcdMainClock = QLCDNumber(w)
lcdMainClock.setGeometry(QRect(512, 180, 191, 81))
lcdMainClock.setObjectName("MainClock")

font = QtGui.QFont("Times", 30, QFont.Bold)
font.setBold(True)

lbl = QtGui.QLabel('RED', w)
lbl.move(24,500)
lbl.setStyleSheet("color: red")
lbl.setFont(font)

lbl1 = QtGui.QLabel('BLUE', w)
lbl1.setStyleSheet("color: blue")
lbl1.move(300,500)
lbl1.setFont(font)

lbl2 = QtGui.QLabel('Time:', w)
lbl2.move(520,140)
lbl2.setStyleSheet("color: aqua")
lbl2.setFont(font)

lbl3 = QtGui.QLabel('THE GAMES!!!', w)
lbl3.setStyleSheet("color: aqua")
lbl3.move(450,100)
lbl3.setFont(font)

# Create a button in the window
btn = QPushButton('Start', w)
btn.move(560, 260)

# Create the actions
@pyqtSlot()
def on_click():
    t.start()
 
# connect the signals to the slots
btn.clicked.connect(on_click)

#establish serial comm channel
ser = serial.Serial('/dev/ttyACM0', 9600)

#the countdown method, which is in a thread
def _countdown():
         x = 121
         for i in xrange(x,0,-1):
             time.sleep(.5)
             stuff = ser.readline()
	     stuffArr = stuff.split()
	     red = stuffArr[0]
	     blue = stuffArr[1]
	     lcdNumber.display(red)
	     lcdNumber1.display(blue)
	     lcdMainClock.display(i-1)

#the thread for the countdown 
t = Thread(target=_countdown)

# Show window
w.show()
	
sys.exit(a.exec_())
