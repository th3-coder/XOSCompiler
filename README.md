# XOSCompiler

# XOSCompiler Prototype

*** Keep config_crossx.txt in same directory as inputFile *** 
* One config file per project directory

Run from project directory
* Options:
    
    -inputFile (file)

    -lang (programmingLanguage) 

    * C, C++, Python

    -compiler (compiler)
    
    * cmake, gcc, g++

    -libs (lib1) (lib2) ...

    -wsl_gui (0 | 1)
    
    * WSL must be installed
    
    -srvx_gui (0 | 1)
    
    * Install VCXSRV (uses configuration file in resources)
    
    -venv (python/venv/name)
