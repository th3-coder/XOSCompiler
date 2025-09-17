# XOSCompiler

# XOSCompiler Prototype

***Keep config_crossx.txt in same directory as inputFile*** 
* One config file per project directory

***File Heirarchy:***

* src_dir(src\crossx.ps1, src\compile.ps1, src\crossx.exe), 

* resources_dir(resources\srvx_X11.xlaunch, resources/anyResourceUsedByInputFile), 

* proj_dir(inputFile, CMakeLists.txt, requirements.txt)

****Options:****
    
    -input (file)

    -lang (programmingLanguage) 

    * C, C++, Python

    -compiler (compiler)
    
    * cmake, gcc, g++

    -libs (lib1) (lib2) ...

    -wsl_gui (0 | 1)
    
    * WSL must be installed
    
    -srvx_gui (0 | 1)
    
    <\t>Recommended:   
    
    * Install VCXSRV (uses configuration file in resources)
    
    * <a href="https://sourceforge.net/projects/vcxsrv/">VCXSRV DOWNLOAD</a>

    -venv (python/venv/name) 

***Example Usages: Always run crossx.exe from project directory ---> src/crossx.exe -parameters*** 
* src\crossx.exe -input libTest.py -venv  

