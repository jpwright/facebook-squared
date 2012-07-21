import serial
from urllib2 import urlopen
from simplejson import loads
import fbconsole
import serial
import time
from api import *

#content = loads(urlopen('http://graph.facebook.com/jpwright').read())
#print content

fbconsole.AUTH_SCOPE = ['read_stream']
fbconsole.authenticate()
print "authenticated"

ser = serial.Serial('/dev/pts/5', 19200, timeout=1)
print "communicating on "+ser.portstr

print "listening"

displaying = "none"

while(1):
    #ser.write("hello")
    #print "hello"
    #time.sleep(0.2)
    line = ser.readline()
    if line.startswith("h"):
        print "hello"
        ser.write("hello")
    if line.startswith("f"):
        newsfeed = fbconsole.get('/me/home')
        newsfeedData = newsfeed["data"]
        for i in range(0, len(newsfeedData)-4): #last entry is paging stuff
            #print i
            if newsfeedData[i]["type"] == "photo":
                newsfeed["data"].pop(i)
        newsfeedstart = 0
        displaying = "feed"
        printnews(newsfeed, newsfeedstart, ser)
    if line.startswith("n"):
        if displaying == "feed":
            newsfeedstart += 7
            printnews(newsfeed, newsfeedstart, ser)
        if displaying == "post":
            x = 0
            #print comments, tbd
    if line.startswith("p"):
        if(newsfeedstart>0):
            newsfeedstart -= 7
        printnews(newsfeed, newsfeedstart, ser)
    if line.startswith("e"):
        num = int(line[1:2])
        itemnum = num+newsfeedstart
        printitem(newsfeed, itemnum, ser)
        displaying = "post"
        
