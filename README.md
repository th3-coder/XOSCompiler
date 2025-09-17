# XOSCompiler

# XOSCompiler Prototype

*** Keep config_crossx.txt in same directory as inputFile *** 
* One config file per project directory

Run from project directory

* Options:
    
    -input (file)

    -lang (programmingLanguage) 

    * C, C++, Python

    -compiler (compiler)
    
    * cmake, gcc, g++

    -libs (lib1) (lib2) ...

    -wsl_gui (0 | 1)
    
    * WSL must be installed
    
    -srvx_gui (0 | 1)
     Recommended:   
    * Install VCXSRV (uses configuration file in resources)
    * <href="https://sourceforge.net/projects/vcxsrv/", link>
    -venv (python/venv/name) 

Example Usage
Run from project directory (config, ps1 scripts, and resources must be in project directory for now)
* ./crossx.exe -input libTest.py -venv  

