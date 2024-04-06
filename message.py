import apimessage
import time
#函数封装
apimessage.start()

while 1:
    if apimessage.funenable():
        print(apimessage.get_data())

    time.sleep(0.5)
