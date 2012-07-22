import serial
import re

def line2id(line):
    if line < 2:
        return "t"+str(line)
    else:
        lineRel = line-2
        screen = lineRel/4 + 1
        linenum = lineRel%4
        return str(screen)+str(linenum)
        

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
            
def printlines(linearray, start, num):
    #for i in range(0, len(linearray)):
        #print linearray[i]
    out = ""
    linenum = 0
    numlines = len(linearray)
    for i in range(0, num):
        if i < numlines:
            line = linearray[i]
            if linenum < num-1:
                out += ".w"+line2id(start+i)+line+"\n"
                linenum += 1
            else:
                regex = re.compile("\<[a-z]\>")
                numBrackets = len(regex.findall(line))*2
                out += ".w"+line2id(start+i)+line[:13+numBrackets]+"...\n"
    return out

def post2checkin(post):
    msg = "<p>"+post["name"]
    if "with_tags" in post:
        people = post["with_tags"]
        if "data" in people:
            msg += " w/ "
            numpeople = len(people["data"])
            for j in range(0,numpeople):
                if j < numpeople-1:
                    msg += people["data"][j]["name"]+", "
                else:
                    msg += people["data"][j]["name"]
    return msg

def printnews(newsfeed, start, ser):
    for i in range(start,start+3):
        msg = ""
        post = newsfeed["data"][i]
        msg += post["from"]["name"].strip()+": "
        if "message" in post:
            msg += "\""+post["message"]+"\""
        if post["type"] == "link":
            if ("name" in post) and ("description" in post):
                msg += "<l>"+post["name"]+", "+post["description"]
        if post["type"] == "checkin":
            msg += post2checkin(post)
        msg = "".join([x if ord(x) < 128 else '?' for x in msg]) #strips non-ASCII characters
        msg = printlines(splitlines(msg.replace("\n"," ")), ((i-start)*4)+2, 4)
        ser.write(msg)
        print msg

def printitem(newsfeed, itemnum, ser):
    print itemnum
    msg = ""
    post = newsfeed["data"][itemnum]
    top = post["from"]["name"]
    if "likes" in post:
        top += " <l>"+str(post["likes"]["count"])
    print "Name/Likes: "+top
    msg += printlines(splitlines(top), 0, 2)
    #print top
    #msg += top
    
    text = ""
    try:
        if "message" in post:
            text += "\""+post["message"]+"\""
        if post["type"] == "link":
            text += "<u>"+post["name"]+"<d>"+post["description"]
        if post["type"] == "checkin":
            text += post2checkin(post)

        text = printlines(splitlines(text.replace("\n"," ")), 2, 12)
        msg += text
        commentstart = 1 + len(text.split("\n"))
        if "comments" in post:
            comments = post["comments"]
            if "data" in comments:
                comments = comments["data"]
                numcomments = len(comments)
                for j in range(0,min(numcomments,6)):
                    comment = comments[j]
                    commentstring = "<c>"
                    commentstring += comment["from"]["name"]+": "
                    commentstring += "\""+comment["message"]+"\""
                    commentstring = printlines(splitlines(commentstring.replace("\n", " ")), commentstart, 12-commentstart)
                    msg += commentstring
                    commentstart = commentstart + len(commentstring.split("\n")) -1
    except KeyError:
        msg += "<error>"
    msg = "".join([x if ord(x) < 128 else '?' for x in msg]) #strips non-ASCII characters
    ser.write(msg)
    print msg
