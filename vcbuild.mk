all:
	VCBuild.exe /nologo getjpegs.vcproj /rebuild

clean:
	VCBuild.exe /nologo getjpegs.vcproj /clean
	
upgrade:
	devenv getjpegs.sln /upgrade

.PHONY:	all clean upgrade

