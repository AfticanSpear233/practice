# encoding=utf-8

#此文档多数为阿里文档粘贴，但是为后续功能进行了一定的魔改，原代码网站:
#https://help.aliyun.com/zh/iot/developer-reference/connect-a-client-to-iot-platform-by-using-the-sdk-for-python


import time 
import time
import sys
import hashlib
import hmac
import base64
import stomp
import ssl
import schedule
import threading
import os
import json
import stomp.utils

data=None
#  接入域名，请参见AMQP客户端接入说明文档。这里直接填入域名，不需要带amqps://前缀
conn = stomp.Connection([('', 61614)], heartbeats=(0,300))   #'处填入用户的接入域名'
conn.set_ssl(for_hosts=[('', 61614)], ssl_version=ssl.PROTOCOL_TLS)  #'处填入用户的接入域名'
def connect_and_subscribe(conn):
    # 工程代码泄露可能会导致 AccessKey 泄露，并威胁账号下所有资源的安全性。以下代码示例使用环境变量获取 AccessKey 的方式进行调用，仅供参考
    accessKey = ''#用户的阿里云accessKey
    accessSecret =''#用户的阿里云accessSecret
    consumerGroupId = "DEFAULT_GROUP"
    # iotInstanceId：实例ID。
    iotInstanceId = ""#用户的阿里云IoT实例ID
    clientId = ""#用户的阿里云产品id
    # 签名方法：支持hmacmd5，hmacsha1和hmacsha256。
    signMethod = "hmacsha1"
    timestamp = current_time_millis()
    # userName组装方法，请参见AMQP客户端接入说明文档。
    # 若使用二进制传输，则userName需要添加encode=base64参数，服务端会将消息体base64编码后再推送。具体添加方法请参见下一章节“二进制消息体说明”。
    username = clientId + "|authMode=aksign" + ",signMethod=" + signMethod \
                    + ",timestamp=" + timestamp + ",authId=" + accessKey \
                    + ",iotInstanceId=" + iotInstanceId \
                    + ",consumerGroupId=" + consumerGroupId + "|"
    signContent = "authId=" + accessKey + "&timestamp=" + timestamp
    # 计算签名，password组装方法，请参见AMQP客户端接入说明文档。
    password = do_sign(accessSecret.encode("utf-8"), signContent.encode("utf-8"))
    
    conn.set_listener('', MyListener(conn))
    conn.connect(username, password, wait=True)
    # 清除历史连接检查任务，新建连接检查任务
    schedule.clear('conn-check')
    schedule.every(1).seconds.do(do_check,conn).tag('conn-check')

class MyListener(stomp.ConnectionListener):
    def __init__(self, conn):
        self.conn = conn
        

    def on_error(self, frame):
        print('received an error "%s"' % frame.body)

    def on_message(self, frame):
        global data
        data=str(frame.body)
        data.replace('received a message "',"")
        data=data[0:(data.__len__())]
        data=json.loads(data)

        #print(data.get("items").get("T"))
        #return data.get("items").get("T")
        #json.dumps(data).get("items").get("T")
        #print(data)
        #print('received a message "%s"' % frame.body)

    def on_heartbeat_timeout(self):
        print('on_heartbeat_timeout')

    def on_connected(self, headers):
        print("successfully connected")
        conn.subscribe(destination='/topic/#', id=1, ack='auto')
        print("successfully subscribe")

    def on_disconnected(self):
        print('disconnected')
        connect_and_subscribe(self.conn)

def current_time_millis():
    return str(int(round(time.time() * 1000)))

def do_sign(secret, sign_content):
    m = hmac.new(secret, sign_content, digestmod=hashlib.sha1)
    return base64.b64encode(m.digest()).decode("utf-8")

# 检查连接，如果未连接则重新建连
def do_check(conn):
    #print('check connection, is_connected: %s', conn.is_connected())
    
    if (not conn.is_connected()):
        try:
            connect_and_subscribe(conn)
        except Exception as e:
            print('disconnected, ', e)

# 定时任务方法，检查连接状态
def connection_check_timer():
    while 1:
        schedule.run_pending()
        time.sleep(10)


    
# # 异步线程运行定时任务，检查连接状态
# thread = threading.Thread(target=connection_check_timer)
# thread.start()

def start():
    
    try:
        connect_and_subscribe(conn)
    except Exception as e:
        print('connecting failed')
        raise e
    
    # 异步线程运行定时任务，检查连接状态
    thread = threading.Thread(target=connection_check_timer)
    thread.start()

def funenable():
    if not data==None:
        return True

def get_data():
    global data
    if type(data)=="str":
        data=json.loads(data)
    return data.get("items").get("T")



if __name__=="__main__":
    
    conn = stomp.Connection([('', 61614)], heartbeats=(0,300))   #'处填入用户的接入域名'
    conn.set_ssl(for_hosts=[('', 61614)], ssl_version=ssl.PROTOCOL_TLS)  #'处填入用户的接入域名'
    try:
        connect_and_subscribe(conn)
    except Exception as e:
        print('connecting failed')
        raise e
    
    # 异步线程运行定时任务，检查连接状态
    thread = threading.Thread(target=connection_check_timer)
    thread.start()
    while 1:
        if not data ==None:
            print(data)
        time.sleep(0.5)