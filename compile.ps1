#compile.ps1
# fix paths to be input parameters from crossx.exe

class admin {
    [string]$command
    [int]$running = 0
    [int]$background = 0
    [bool]$remotePathExists = $false
    [int]$remoteCheck = 0
}
class compileConfig {
    [string]$lang
    [string]$hostDir
    [string]$inputFile
    [string]$output_file
    [string]$localOut
    [string]$outDir
    [string]$ProjectName
    [string]$compiler
    [string[]]$libraries = @()
    [string[]]$includes = @()
    [string[]]$compile_cmds = @()
    [string]$run
    [string]$venv_path
    [string]$pyDependecies

    [void]GetInputFile([string] $inputFile, [string] $wd){
        $this.inputFile = $inputFile
        $this.inputFile = ('{0}/{1}/{2}' -f $wd, $this.ProjectName, $inputFile)
        if ($this.inputFile -eq 0){
            Write-Host "Error writing output file, check remote directory in config file"
            exit
        }
    }

    [void]GetOutputFile([string] $inputFile, [string] $remote_wd){
        $fileSplitter = "\."
        $buffer = $inputFile -split $fileSplitter

        for([int]$i = 0; $i -lt $buffer.Length ; $i++ ){
            if($buffer[$i] -eq "C"){
                $this.lang = "C"
            }
            elseif($buffer[$i] -eq "cpp"){
                $this.lang = "C++"
            }
            elseif($buffer[$i] -eq "py"){
                $this.lang = "Py"
                $this.localOut = $buffer[0]
                $this.output_file = ('{0}/{1}/build/{2}.py' -f $remote_wd, $this.ProjectName, $buffer[0])
                return 
            }
        }

        #Write-Host $fileExt
        $this.localOut = $buffer[0]
        $this.outDir = "$($remote_wd)/$($this.ProjectName)"
        $this.output_file = ('{0}/{1}/build/{2}' -f $remote_wd, $this.ProjectName, $buffer[0])
        
        if($buffer.Length -eq 0){
            Write-Host "Error writing output file"
            exit
        }
    }

    [void]outputInfo(){
        Write-Host "--------- Compile Configuration -------------"
        Write-Host ('Language: {0}' -f $this.lang)
        Write-Host ('Compiler: {0}' -f $this.compiler)
        Write-Host ('Remote Source File: {0}' -f $this.inputFile)
        Write-Host ('Remote Output File: {0}' -f $this.output_file)
        Write-Host ('Local Directory: {0}' -f $this.ProjectName)
        Write-Host ('Libraries: {0}' -f ($this.libraries).Length)
        Write-Host "---------------------------------------------"
    }

    [void]update_cmake([string]$inputFile){
        #change CMakeLists.txt file
        $cmake_path = "CMakeLists.txt"
        $content = Get-Content -Path $cmake_path
        $totalLines = $content.Count

        for( [int]$i = 0 ; $i -lt $totalLines; $i++){
            $currentLine = $content[$i]
            #Write-Host $currentLine
            if ($currentLine.contains("project")){
                $content[$i] = ('project({0})' -f $this.localOut)
            }
            elseif ($currentLine.contains("add_executable")){
                $content[$i] = "add_executable( $($this.localOut)"
                $content[$i+1] = $inputFile
            }
            elseif ($currentLine.contains("target_include_directories")){
                $content[$i] = "target_include_directories($($this.localOut) PRIVATE `$`{OpenCV_INCLUDE_DIRS`})"
            }
            elseif ($currentLine.contains("target_link_libraries")){
                $content[$i] = "target_link_libraries($($this.localOut) PRIVATE `${OpenCV_LIBS})"
            }
        }
        $content | Set-Content -Path $cmake_path
    }

    [void]compile_Buffer([string]$client_wd, [string]$inputFile, [string]$remoteOS, [admin]$admin) {
        # $compile.output_file = ('{0}/{1}' -f $client.output_dir, $inputFile)
        if( $this.lang -eq "C" -or $this.lang -eq "C++"){
            class c_config{
                [int]$libs = 0
                [int]$static = 0
                [string]$makeFile
            }

            $config = [c_config]::new()
            if($this.compiler -eq "cmake"){
                #Write-Host "Hello"
                $this.update_cmake($inputFile)
                $this.compile_cmds += ('cd {0}/{1}/build  && cmake ..' -f $client_wd, $this.ProjectName);
                $this.compile_cmds += ('cd {0}/{1}/build  && make' -f $client_wd, $this.ProjectName);
            }
            elseif($this.lang -eq "C++"){
                $this.compile_cmds += ('g++ {0} -o {1}' -f $this.inputFile, $this.output_file)
            }

            elseif($this.lang -eq "C") {
                $this.compile_cmds += ('gcc {0} -o {1}' -f $this.inputFile, $this.output_file)
                
                if($config.static -eq 1){
                    $this.compile_cmds += ('{0} --static' -f $this.compile_cmds[0])
                }
                if($config.libs -ne 0){
                    [string]$libBuffer = ""
                    for([int]$i = 0 ; $i -lt ($this.compile_cmds).Length){
                        $libBuffer = (' -l{0}', $this.libraries[$i])
                    }
                    $this.compile_cmds += ('{0}{1}' -f $libBuffer)
                }
            }
            $this.run = "cd $($this.outDir)/build && ./$($this.localOut)"
        }
        elseif ($this.lang -eq "Py"){
            $this.run = ('python {0}' -f $this.inputFile)
            if($this.venv_path.Length -ne 0) {
                $this.python_venv_setup($admin, $client_wd, $remoteOS)
            }
        }
    }

    [void]python_venv_setup([admin]$admin, [string]$remote_wd, [string]$remoteOS){
        $admin.background = 1
        #$admin.command = "python3 -m venv $($remote_wd)/$($this.venv_path)"
        checkRemotePath -remotePath "~/$($this.venv_path)"
        if( -not $admin.remotePathExists){
            $admin.command = "python3 -m venv ~/$($this.venv_path)"
            runSSH
        }
        
        $admin.background = 0
        if((Test-Path "$($this.hostDir)/requirements.txt")){
            #$admin.command = "source $($remote_wd)/$($this.venv_path)/bin/activate && pip install -r $($remote_wd)/$($this.ProjectName)/requirements.txt"
            $admin.command = "source ~/$($this.venv_path)/bin/activate && pip install -r $($remote_wd)/$($this.ProjectName)/requirements.txt"
            runSSH
        }
        
        if($remoteOS -eq "Linux"){
            #$this.run = "source $($remote_wd)/$($this.venv_path)/bin/activate && $($this.run)"
            $this.run = "source ~/$($this.venv_path)/bin/activate && $($this.run)"
        }   
        elseif($remoteOS -eq "Windows"){
            $this.run = ".$($this.venv_path)\Scripts\activate && $($this.run)"
        }
    }

    [void]COMPILE_CMD([admin]$admin){
        if (($this.compile_cmds).Length -eq 0 -and $this.lang -ne "Py"){
            Write-Host "Error - no compile commands"
            exit
        }
        if($this.lang -eq "Py"){
            $this.compile_cmds += "Interpreted Language"
            return
        }
        $admin.command = $this.compile_cmds
        Write-Host "--- Compile Commands ---"
        # $this.compile_cmds | ForEach-Object {
        #     $admin.command = $_
        #     runSSH
        # }
        for([int]$i = 0 ; $i -lt ($this.compile_cmds).Count ; $i++){
            $admin.command = $this.compile_cmds[$i]
            runSSH
        }
        Write-Host "------------------------"
    }
}

$compile = [compileConfig]::new()
$admin = [admin]::new()

function cleanPaths {
    if($ssh.wsl_X11 -eq 1){
        $pattern = [Regex]::Escape($hostX.key)
        #replace key file with wsl/ubuntu style/location key file
        $admin.command = $admin.command -replace $pattern, '~/.ssh/id_ed25519'
    }
}
function cleanDirectory {
    #fix remote directory
    checkRemotePath -remotePath "$($client.output_dir)/$($compile.ProjectName)"
    if( -not $admin.remotePathExists ){
        $admin.command = ('rm -rf {0}/{1}' -f $client.output_dir, $compile.ProjectName)
        runSSH
    }
    checkRemotePath -remotePath $client.output_dir
    if( -not $admin.remotePathExists) {
        $admin.command = ('mkdir {0}' -f $client.output_dir)
        runSSH
    }
    else {
        Write-Host "Project directory already created"
    }
    checkRemotePath -remotePath "$($client.output_dir)/$($compile.ProjectName)"
    #create output directory
    if( -not $admin.remotePathExists ) {
        $admin.command = ('mkdir {0}/{1}' -f $client.output_dir, $compile.ProjectName)
        runSSH
    }
    $admin.command = ('mkdir {0}/{1}/build' -f $client.output_dir, $compile.ProjectName)
    runSSH
    # send files
    Write-Host ('Transferring files: {0}' -f $ssh.scp_cmd)
    Invoke-Expression $ssh.scp_cmd
    
    if (Test-Path "$($compile.hostDir)\resources" ){
        $admin.command = ('mv {0}/{1}/resources/* {0}/{1}/build' -f $client.output_dir, $compile.ProjectName)
        runSSH
    }
}
# old unused function
function createRunScript {
    $Path_Run_Script = "run.sh"
    [string[]]$runScript = @()
    $runScript += "cd $($compile.outDir)/build"
    $runScript += "./$($compile.localOut)"
    $runScript | Set-Content -Path $Path_Run_Script
}
function run_Program {
    param
    (
        [int]$background = 0 ,
        [int]$wsl_gui = 0
    )
    
    if(($compile.run).Length -ne 0){
        Write-Host "----- Run Commands -----"
        #$admin.command = "$($compile.outDir)/run.sh"
        $admin.command = $compile.run
        $admin.running = 1
        runSSH
        $admin.running = 0
        Write-Host "-----------------------"
    }
    else{
        Write-Host "Error - no run command"
        exit
    }
}
function start_wsl_x11 {
    # wsl formatted ssh
    # wsl sh -c "ssh -Y -t -i ~/.ssh/id_ed25519 th3@192.168.0.220 'cd ~/Desktop/crossx/crossOS_compiler/build && ./filterImg '"
    cleanPaths
    $admin.command = $admin.command -replace "ssh ", "ssh -Y -t "
    $admin.command = ('wsl sh -c "{0}"' -f $admin.command)
}
function start_srvx_x11 {
    $Path_srvx = "$($hostX.proj_dir)\resources\srvx_X11.xlaunch"
    taskkill /IM "vcxsrv.exe" /F
    Start-Process $Path_srvx
    $Env:DISPLAY="localhost:0.0"
    $admin.command = $admin.command -replace "ssh ", "ssh -Y -t "
}
function runSSH {
    if( ($admin.command).Length -eq 0){
        Write-Host "Error - No ssh command"
        exit
    }
    
    $admin.command = ('{0} ''{1}''' -f $ssh.ssh_cmd, $admin.command) 
    
    if( $admin.background -eq 1){
        $admin.command = $admin.command -replace "ssh ", "ssh -t "
    }
    if( $admin.remoteCheck -eq 1){
        $admin.command = "$($admin.command) 2>$\null"
    }
    if( $admin.running -eq 1 ) {       
        if($ssh.wsl_X11 -eq 1){
            start_wsl_x11
        }
        elseif($ssh.srvx_X11 -eq 1){
            start_srvx_x11
        }
    }
    Write-Host ('Running SSH: {0}' -f $admin.command) 
    Invoke-Expression $admin.command
}
function Cleanup {
    Write-Host "Cleaning up proccesses"
    if ($ssh.wsl_X11 -ne 0){
        wsl --shutdown
    }
    elseif( $ssh.srvx_X11 -ne 0){
        taskkill /IM "vcxsrv.exe" /F
    }
}
function RunProcces {
    #createRunScript
    cleanDirectory # prepare remote preoject directory
    $compile.compile_Buffer($client.output_dir, $inputFile, $client.OS, $admin)
    $compile.outputInfo()    
    $compile.COMPILE_CMD($admin) #compile project
    run_Program #run output file
    Cleanup
}
function checkRemotePath {
    param
    (
        [string]$remotePath

    )
    $admin.remoteCheck = 1
    $admin.remotePathExists=$false
    try{
        $admin.command = "stat $($remotePath)"
        runSSH
        if($LASTEXITCODE -eq 0){
            $admin.remotePathExists = $true
        }
    }
    catch{
        $admin.remoteCheck = 0    
    }
    $admin.remoteCheck = 0
}

function GetConfigData {
    # order of operations are important in this function
    if( $venv.Length -ne 0 ){
        $compile.venv_path = $venv
    }
    $compile.hostDir = $hostX.proj_dir
    $compile.ProjectName = $hostX.ProjectName
    $compile.compiler = $client.compiler
    $compile.GetInputFile($inputFile, $client.output_dir)
    $compile.lang = $client.lang
    $compile.GetOutputFile($inputFile, $client.output_dir)
    $client.lang = $compile.lang

    if($compile.lang -eq "Py"){
        $compile.compiler = "None"
        $client.compiler = "None"
    }
}

# main function
GetConfigData

RunProcces

Read-Host