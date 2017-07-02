
def printFile(filename):
    try:
        fobj = open(filename,'r')
    except IOError:
        print("Error %s" %e)
    else:
        i = 0
        for eachline in fobj:
            print(i, eachline)
            i+=1
        fobj.close();

printFile("file.py")
