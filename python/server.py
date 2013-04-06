import serial
from urllib2 import urlopen
from simplejson import loads
from urllib import urlretrieve
import imp
urlretrieve('https://raw.github.com/facebook/fbconsole/master/src/fbconsole.py',
                '.fbconsole.py')
fbconsole = imp.load_source('fb', '.fbconsole.py')
import serial
import time
from api import *
import Image
import urllib

#content = loads(urlopen('http://graph.facebook.com/jpwright').read())
#print content

fbconsole.AUTH_SCOPE = ['read_stream', 'publish_stream']
fbconsole.authenticate()
#print "authenticated"

port = "/dev/pts/5"
port = "/dev/ttyACM0"
ser = serial.Serial(port, 57600, timeout=1)
print "communicating on "+ser.portstr

print "listening"

bannedTypes = ["photo", "video"]

displaying = "none"

cutnum = 0
cuts = ["no"]

myname = ""

postid = ""

liked = []

while(1):
    #ser.write("hello")
    #print "hello"
    #time.sleep(0.2)
    line = ser.readline()
    print "heard: "+line
    if line.startswith(".h"):
        info = fbconsole.get('/me')
        myname = info["first_name"]
        print "hello "+myname
        ser.write(".h"+myname+"\n")
        waitforack(ser, "h")
    if line.startswith(".f"):
        newsfeed = fbconsole.get('/me/home')
        newsfeedData = newsfeed["data"]
        nfDataClean = newsfeedData
        it = iter(nfDataClean)
        #for i in range(0, len(newsfeedData)-2): #last entry is paging stuff
        try:
            while 1:
                newsItem = it.next()
                #print newsItem["id"]
                if newsItem["type"] in bannedTypes:
                    newsfeed["data"].remove(newsItem)
        except StopIteration:
            x = 0
        newsfeedstart = 0
        displaying = "feed"
        clearscreens(ser)
        ser.write(".wt,0,News Feed\n")
        waitforack(ser, "w")
        ser.write(".wt,1,----------------\n")
        waitforack(ser, "w")
        printnews(newsfeed, newsfeedstart, ser)
        imsg = ".g0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000111111111111111111111111111111111111111110000000000111111111111111111111111111111111111111110000000011111111111111111111111111111111111011101000000000110000000000000000000000000000000000000111000000000110000000000000000000000000000000000001100000000011100000000000000000000000000000000000001100000000111000000000000000000000000000000000000111000000001110000000000000000000000000000000000001110000000011100000000000000000000000000000000000011100000000110000111111111111111111111111111110000111000000000110000111011111111111111111111111110001110000000011100011111111101110111011101110111000011100000000111000000000000000000000000000000000000111000000001110000000000000000000000000000000000001110000000011100000000000000000000000000000000000011100000000111000000000000000000000000000000000000111000000001100001111111111111111111111111111100001110000000001100001111111111111011101111111011100011100000000111000111011101110111111111011101111000111000000001110000000000000000000000000000000000001110000000011100000000000000000000000000000000000011100000000111000000000000000000000000000000000000110000000001110000000000000000000000000000000000000110000000011000011111111111111111111111111111100011100000000011000111111111111111111111111111111111101000000001110000111011101110111011101110111011111100000000011100000000000000000000011111111101111011100000000111000000000000000000000110000000000000100000000001110000000000000000000000110000000000011100000000011100000000000000000000011100000000001110000000000110000111111111111111111110000000000111000000000000110001000111111111111111100000000011000000000000011100001111101110111011101100000001111000000000000111000000000000000000000111000001111000000000000001110000000000000000000001110000001000000000000000011100000000000000000000011100001111000000000000000111000000000000000000000111000111100000000000000001110000000000000000000001100011000000000000000000011000000000000000000000001111010000000000000000000011111111111111111111111110110000000000000000000001111111111111111111111111110100000000000000000000011101110111011101110111011100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\n"
        ser.write(imsg)
        waitforack(ser, "g")
    if line.startswith(".n"):
        if displaying == "feed":
            clearscreens(ser)
            newsfeedstart += 3
            ser.write(".wt,0,News Feed\n")
            waitforack(ser, "w")
            ser.write(".wt,1,----------------\n")
            waitforack(ser, "w")
            printnews(newsfeed, newsfeedstart, ser)
        if displaying == "post":
            print cuts
            cutNext = cuts[cutnum+1]
            if cutNext != "no":
                clearscreens(ser)
                cuts.append(printitem(newsfeed, itemnum, cutNext, ser))
                cutnum += 1
            #print comments, tbd
    if line.startswith(".p"):
        if(newsfeedstart>0):
            clearscreens(ser)
            newsfeedstart -= 3
            ser.write(".wt,0,News Feed\n")
            waitforack(ser, "w")
            ser.write(".wt,1,----------------\n")
            waitforack(ser, "w")
            printnews(newsfeed, newsfeedstart, ser)
        if displaying == "post":
            print cuts
            if cutnum > 0:
                clearscreens(ser)
                printitem(newsfeed, itemnum, cuts[cutnum-1], ser)
                cutnum -= 1
    if line.startswith(".e"):
        clearscreens(ser)
        num = int(line[2:])-1
        itemnum = num+newsfeedstart
        cuts = ["no"]
        cutnum = 0
        cuts.append(printitem(newsfeed, itemnum, "no", ser))
        postid = newsfeedData[itemnum]["id"]
        print "POST ID: "+postid
        #print cutNext
        userid = newsfeed["data"][itemnum]["from"]["id"]
        pictureUrl = fbconsole.graph_url("/"+userid+"/picture")
        urllib.urlretrieve(pictureUrl, "profile.jpg")
        im = Image.open("profile.jpg")
        #print im.format, im.size, im.mode
        bw = im.convert("1")
        bw.save("profile_bw.jpg")
        pix = bw.load()
        imsg = ".g"
        for x in range(0,50):
            for y in range(0,50):
                if pix[y,x] == 255:
                    imsg += "0"
                else:
                    imsg += "1"
        imsg += "\n"
        print "writing image"
        print imsg
        ser.write(imsg)
        waitforack(ser, "g")
        displaying = "post"
    if line.startswith(".l"):
        if displaying == "post":
            likeurl = "/"+postid+"/likes"
            print "POST "+likeurl
            post = newsfeedData[itemnum]
            if postid not in liked:
                fbconsole.post("/"+postid+"/likes", {})
                #Just assume that it worked and change the data ;-)
                if "likes" not in post:
                    
                    PyDict_SetItem(post, "likes", {"count":1})
                else:
                    numlikes = post["likes"]["count"]
                    post["likes"]["count"] = numlikes+1
                liked.append(postid)
            else:
                fbconsole.post("/"+postid+"/likes&method=DELETE", {})
                numlikes = post["likes"]["count"]
                if numlikes == 1:
                    post["likes"].remove("count")
                else:
                    post["likes"]["count"] = numlikes-1
                liked.remove(postid)
            printitem(newsfeed, itemnum, "no", ser)
