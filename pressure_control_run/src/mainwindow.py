import sys
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *

import sys
from PyQt5 import Qt, QtWidgets, uic
from qwt import *


Form, Window = uic.loadUiType("main_window_dynamic.ui")

app = QApplication([])
window = Window()
form = Form()
form.setupUi(window)
window.show()
app.exec_()

