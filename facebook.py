import serial
from urllib2 import urlopen
from simplejson import loads
import fbconsole
import serial

#content = loads(urlopen('http://graph.facebook.com/jpwright').read())
#print content

fbconsole.AUTH_SCOPE = ['read_stream']
fbconsole.authenticate()

status = fbconsole.get('/me/home')
for i in range(0,6):
    msg = ""
    post = status["data"][i]
    msg += post["from"]["name"].strip()+": "
    try:
        msg += "\""+post["message"]+"\""
    except KeyError:
        x = 0
        #ignoring
    if post["type"] == "link":
        msg += "link: "+post["name"]+", "+post["description"]
    print msg

ser = serial.Serial('/dev/tty0', 19200, timeout=1)
print ser.portstr
