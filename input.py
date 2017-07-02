import sys

class foo(object):
    version = 1
    def __init__(self, version=2):
        self.version = version
    def showversion(self):
        return self.version
    def setversion(self, version):
        self.version = version;

obj = foo()
version = input()
obj.setversion(version)
version = obj.showversion()
print(version)

