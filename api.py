import serial
import re

def splitlines(msg, start, num):
    words = msg.split(" ")
    out = ""
    line = ""
    linenum = 0
    for i in range(0, len(words)):
        word = words[i]
        if (line+word).__len__() < 17:
            line = line + word + " "
        else:
            line = line[:16]
            if linenum < num-1:
                out += ".p"+str(start+linenum)+line+"\n"
            if linenum == num-1:
                out += ".p"+str(start+linenum)+line[:13]+"...\n"
            line = word + " "
            linenum += 1
    return out
            

def printcheckin(post):
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
            msg += printcheckin(post)
        msg = "".join([x if ord(x) < 128 else '?' for x in msg]) #strips non-ASCII characters
        msg = splitlines(msg.replace("\n"," "), (i-start)*4, 4)
        ser.write(msg)
        print msg

def printitem(newsfeed, itemnum, ser):
    print itemnum
    msg = ""
    post = newsfeed["data"][itemnum]
    top = post["from"]["name"].strip()
    if "likes" in post:
        top += "<l>"+str(post["likes"]["count"])
    top = splitlines(top, 0, 2)
    print top
    msg += top
    
    text = ""
    try:
        if "message" in post:
            text += "\""+post["message"]+"\""
        if post["type"] == "link":
            text += "<u>"+post["name"]+"<d>"+post["description"]
        if post["type"] == "checkin":
            text += printcheckin(post)

        text = splitlines(text.replace("\n"," "), 2, 12)
        msg += text
        commentstart = 1 + len(text.split("\n"))
        if "comments" in post:
            comments = post["comments"]
            if "data" in comments:
                comments = comments["data"]
                numcomments = len(comments)
                for j in range(0,min(numcomments,6)):
                    comment = comments[j]
                    commentstring == "<c>"
                    commentstring += comment["from"]["name"]+": "
                    commentstring += "\""+comment["message"]+"\""
                    commentstring = splitlines(commentstring.replace("\n", " "), commentstart, 12-commentstart)
                    msg += commentstring
                    commentstart = commentstart + len(commentstring.split("\n")) -1
    except KeyError:
        msg += "<error>"
    msg = "".join([x if ord(x) < 128 else '?' for x in msg]) #strips non-ASCII characters
    ser.write(msg)
    print msg
