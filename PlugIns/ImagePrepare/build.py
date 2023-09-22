import os

CPP="cl "
CPP_FLAGS="/MDd /W3 /Gm /GX /ZI /Od /I \"../../\" /D \"WIN32\" /D \"_DEBUG\" /D \"_WINDOWS\" /D \"_WINDLL\" /D \"_AFXDLL\" /D \"_MBCS\" /D \"_USRDLL\" /Fp\"Debug/ImagePrepare.pch\" /Yu\"stdafx.h\" /Fo\"Debug/\" /Fd\"Debug/\" /FD /GZ /c "

command = CPP + CPP_FLAGS + "ImagePrepare.cpp"

print(os.popen(command).read())


LINK="D:/user/programming/C/vc6.0/vc98/Bin/link "
LINK_FLAGS = "ImageProc.lib TraceFeature.lib /subsystem:windows /dll /incremental:no \"/pdb:Debug/PA_ImagePrepare.pdb\" /debug /machine:I386 \"/def:.\ImagePrepare.def\" \"/out:../../Debug/Plugin/PA_ImagePrepare.dll\" \"/implib:Debug/PA_ImagePrepare.lib\" /pdbtype:sept \"/libpath:../../\" "

print(os.popen(LINK + LINK_FLAGS +  ".\Debug\ImagePrepare.obj .\Debug\StdAfx.obj .\Debug\ImagePrepare.res").read())