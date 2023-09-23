import os

print(os.popen("cd PlugIns/ImagePrepare && py build.py").read())
print(os.popen("cd PlugIns/FaceLocator && py build.py").read())

print(os.popen("Debug\\VFWCAPTURE.exe").read())