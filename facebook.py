import serial
from urllib2 import urlopen
from simplejson import loads
import fbconsole
import serial
import time

#content = loads(urlopen('http://graph.facebook.com/jpwright').read())
#print content

fbconsole.AUTH_SCOPE = ['read_stream']
fbconsole.authenticate()
print "authenticated"

ser = serial.Serial('/dev/pts/5', 19200, timeout=1)
print "communicating on "+ser.portstr

print "listening"
while(1):
    #ser.write("hello")
    #print "hello"
    #time.sleep(0.2)
    line = ser.readline()
    if line.startswith("h"):
        print "hello"
        ser.write("hello")
    if line.startswith("f"):
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
            ser.write(msg)
            print msg
