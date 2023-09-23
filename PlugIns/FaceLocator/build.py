import os

CPP="cl "
CPP_FLAGS="/MDd /W3 /Gm /GX /ZI /Od /I \"../../\" /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"_WINDLL\" /D \"_AFXDLL\" /D \"_MBCS\" /D \"_USRDLL\" /Fp\"Debug/FaceLocator.pch\" /Yu\"stdafx.h\" /Fo\"Debug/\" /Fd\"Debug/\" /FD /GZ /c "

command = CPP + CPP_FLAGS + "FaceLocator.cpp"

print(os.popen(command).read())


LINK="D:/user/programming/C/vc6.0/vc98/Bin/link "
LINK_FLAGS = "ImageProc.lib TraceFeature.lib /subsystem:windows /dll /incremental:no \"/pdb:Debug/PB_FaceLocator.pdb\" /debug /machine:I386 \"/def:.\FaceLocator.def\" \"/out:../../Debug/Plugin/PB_FaceLocator.dll\" \"/implib:Debug/PB_FaceLocator.lib\" /pdbtype:sept \"/libpath:../../\" "

print(os.popen(LINK + LINK_FLAGS +  ".\Debug\FaceLocator.obj .\Debug\StdAfx.obj .\Debug\FaceLocator.res").read())