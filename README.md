# XOSCompiler

# XOSCompiler Prototype

***Keep config_crossx.txt in same directory as inputFile*** 
* One config file per project directory
* Update config file for set parameters (when calling function with parameters this will overwrite any config parameters)

***File Heirarchy:***

* src_dir(src\crossx.ps1, src\compile.ps1, src\crossx.exe), 

* resources_dir(resources\srvx_X11.xlaunch, resources/anyResourceUsedByInputFile), 

* proj_dir(inputFile, CMakeLists.txt, requirements.txt)

****Options:****
    
**-input (file)**

**-lang (programmingLanguage)** 

* C, C++, Python

**-compiler (compiler)**

* cmake, gcc, g++

**-libs (lib1) (lib2) ...**

**-wsl_gui (0 | 1)**

* WSL must be installed

**-srvx_gui (0 | 1)**

Recommended:   
    
* Install VCXSRV (uses configuration file in resources)

* <a href="https://sourceforge.net/projects/vcxsrv/">VCXSRV DOWNLOAD</a>

**-venv (python/venv/name)** 
* default venv is stored in /home/user (linux)

***Example Usages: Always run crossx.exe from project directory ---> src/crossx.exe -parameters*** 

    src\crossx.exe -input libTest.py -venv myVenv 

    src\crossx.exe -input filterImg.cpp -compiler cmake -srvx_gui 1
    
<img width="1918" height="1199" alt="image" src="https://github.com/user-attachments/assets/96efc55d-428e-455e-85d3-6982a8f8ef9b" />


***Host Dependecies***

**Windows:**

* openssh-client 

* VCXSRV (optional) - enables X11 Gui forwarding from linux -> windows 

* wsl (optional) - enables X11 Gui forwarding from linux -> windows

***Remote Dependecies***

* openssh-server

* cmake

* g++

* gcc

* python

