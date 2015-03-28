all:
	MSBuild.exe /nologo getjpegs.sln /p:Configuration=Debug
	MSBuild.exe /nologo getjpegs.sln /p:Configuration=Release

clean:
	MSBuild.exe /nologo getjpegs.sln /p:Configuration=Debug   /t:clean
	MSBuild.exe /nologo getjpegs.sln /p:Configuration=Release /t:clean

upgrade:
	devenv getjpegs.sln /upgrade
	
.PHONY:	all clean upgrade

