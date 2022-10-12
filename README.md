# BGI-Arkanoid

Multi-Threading Arkanoid on bgi
Last updated December 2013

## Setup

This project is using WinBGI Library that is designed only for Windows.

For documentation of the Winbgi library, please see:

[Borland Graphics Interface (BGI) for Windows](www.cs.colorado.edu/~main/cs1300/doc/bgi/bgi.html)
[The WINBGIM Library](www.cs.colorado.edu/~main/cs1300/doc/bgi/index.html)

Originally build with Visual Studio 2010 and later ported to VS 2019.

To build project required VS 2019 MsBuild.exe. It can be found in [Microsoft Build Tools](https://visualstudio.microsoft.com/downloads/?q=build+tools)

You can use Visual Studio IDE or run build commands from cli.
Usually MsBuild.exe located somewhere in visual studio installation folder `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe`

Then run `"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" ./bgi.vcxproj /p:configuration=debug`
