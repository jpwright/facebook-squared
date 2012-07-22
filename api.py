import serial
import re
import time

def waitforack(ser, cmd):
    line = ""
    while line != ".z"+cmd:
        line = ser.readline()
        time.sleep(1)
        print "waiting for ack..."
        print "heard: "+line

def lcd(msg):
    msg = "".join([x if ord(x) < 128 else '?' for x in msg]) #strips non-ASCII characters
    regex = re.compile("\<[a-z]\>")
    numBrackets = len(regex.findall(msg))*2
    msg = msg[:16+numBrackets+6] #16 char screens, plus room for brackets, plus command bytes
    return msg

def cb(msg):
    msg = msg.replace("<","[")
    msg = msg.replace(">","]")
    return msg

def line2id(line):
    if line < 2:
        return "t,"+str(line)+","
    else:
        lineRel = line-2
        screen = lineRel/4 + 1
        linenum = lineRel%4
        return str(screen)+","+str(linenum)+","
        

def splitlines(msg):
    words = msg.split(" ")
    linearray = []
    line = ""
    regex = re.compile("\<[a-z]\>")
    for i in range(0, len(words)):
        word = words[i]
        #print word
        if (line+word).__len__()-len(regex.findall(line+word))*2 < 17:
            line = line + word + " "
        else:
            line = line.strip()
            #out += ".p"+line2id(start+linenum)+line+"\n"
            linearray.append(line)
            line = word + " "
    linearray.append(line.strip())
    return linearray
            
def printlines(linearray, firstline, start, num, ser):
    #for i in range(0, len(linearray)):
        #print linearray[i]
    out = ""
    linenum = 0
    numlines = len(linearray)
    cut = "no"
    firstPrint = True
    for i in range(firstline, firstline+num):
        if i < numlines: #check if line exist in linearray
            line = linearray[i]
            if linenum < num-1: #if this is not the last line we're printing
                out = ".w"+line2id(start+i-firstline)+line+"\n"
                linenum += 1
                cut = "no:"+str(linenum)
            else:
                if i < numlines-1:
                    regex = re.compile("\<[a-z]\>")
                    numBrackets = len(regex.findall(line))*2
                    out = ".w"+line2id(start+i-firstline)+line[:15+numBrackets]+"<e>\n"
                    cut = str(i)
                else:
                    out = ".w"+line2id(start+i-firstline)+line+"\n"
                    linenum += 1
                    cut = "no:"+str(linenum)
            if firstPrint:
                firstPrint = False
            else:
                waitforack(ser, "w")
            print lcd(out)
            ser.write(lcd(out))

    #print lcd(out)
    print "printline cut: "+cut
    return cut

def post2checkin(post):
    msg = "<p>"+cb(post["name"])
    if "with_tags" in post:
        people = post["with_tags"]
        if "data" in people:
            msg += " w/ "
            numpeople = len(people["data"])
            for j in range(0,numpeople):
                if j < numpeople-1:
                    msg += cb(people["data"][j]["name"])+", "
                else:
                    msg += cb(people["data"][j]["name"])
    return msg

def printnews(newsfeed, start, ser):
    for i in range(start,start+3):
        msg = ""
        post = newsfeed["data"][i]
        msg += post["from"]["name"].strip()+": "
        if "message" in post:
            msg += "\""+cb(post["message"])+"\""
        if post["type"] == "link":
            if "name" in post:
                msg += "<u>"+cb(post["name"])
            if "description" in post:
                msg += ", "+cb(post["description"])
        if post["type"] == "checkin":
            msg += post2checkin(post)
        printlines(splitlines(msg.replace("\n"," ")), 0, ((i-start)*4)+2, 4, ser)
        #ser.write(msg)
        print msg

def printitem(newsfeed, itemnum, inCut, ser):
    print itemnum
    msg = ""
    out = "no"
    outSet = False
    post = newsfeed["data"][itemnum]
    top = cb(post["from"]["name"])
    if "likes" in post:
        top += " <l>"+str(post["likes"]["count"])
    print "Name/Likes: "+top
    cut = printlines(splitlines(top), 0, 0, 2, ser) #this value of cut is not used
    text = ""
    try:
        if "message" in post:
            text += "\""+cb(post["message"])+"\""
        if post["type"] == "link":
            text += " <u>"+cb(post["name"])
            if "description" in post:
                text += ", "+cb(post["description"])
        if post["type"] == "checkin":
            text += post2checkin(post)

        msgStart = 0
        if inCut.startswith("msg"):
            msgStart = inCut[4:]
            print "starting msg at line "+msgStart

        if inCut.startswith("cmt") == False:
            cut = printlines(splitlines(text.replace("\n"," ")), int(msgStart), 2, 12, ser)
            if cut.startswith("no") == False:
                out = "msg:"+cut
                outSet = True
        else:
            cut = "no:-1"

        

        if outSet == False:
            print "going to print comments"
            commentstart = ((int(cut[3:])/4)*4)+6
            cmtNumStart = 0
            if inCut.startswith("cmt"):
                cmtNumStart = int(inCut[4:])
            if "comments" in post:
                comments = post["comments"]
                if "data" in comments:
                    comments = comments["data"]
                    numcomments = len(comments)
                    for j in range(cmtNumStart,numcomments):
                        comment = comments[j]
                        commentstring = "<c>"
                        commentstring += cb(comment["from"]["name"])+": "
                        commentstring += "\""+cb(comment["message"])+"\""
                        cut = printlines(splitlines(commentstring.replace("\n", " ")), 0, commentstart, 14-commentstart, ser)
                        commentstart += 4
                        if outSet == False and commentstart > 15:
                            out = "cmt:"+str(j)
                            outSet = True
                            #commentstart = commentstart + len(commentstring.split("\n")) -1
    except KeyError:
        ser.write("error")
    print msg
    print "printitem cut: "+out
    return out
