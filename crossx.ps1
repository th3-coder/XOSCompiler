#crossx.ps1
# open powershell with correct permissions
# Start powershell "powershell -ExecutionPolicy bypass"
# run script
#./crossx -inputFile "name" -options

# powershell one-line command:
# powershell -ExecutionPolicy bypass -command "./crossx.ps1 -inputFile filterImg.cpp -compiler cmake"

# Create an executable to run initial .ps1 script and pass all parameters (inputFile, compiler, wd, X11, ...)
# currently in src folder 

param
(
    $config = "$(Get-Location)\config_crossx.txt",
    $inputFile,
    $language,
    $compiler,
    $wd = (Get-Location),
    [int]$wsl_gui = 0,
    [int]$srvx_gui = 0,
    [int]$X11,
    [string]$venv,
    [string[]]$libs = @()
)

class host_machine{
    $proj_dir = (Get-Location)
    $user = $Env:USERNAME
    $drive = "C:"
    $key = ('C:\Users\{0}\.ssh\id_ed25519' -f $this.user)
    [string]$ProjectName
    [string]$OS = "Windows"

    [void]outputInfo(){
        Write-Host "------------------- Host --------------------"
        Write-Host ('OS: {0}' -f $this.OS)
        Write-Host ('Project-Name: {0}' -f $this.ProjectName)
        Write-Host ('Project-Dir: {0}' -f $this.proj_dir)
        Write-Host ('Private Key: {0}' -f $this.key)
        Write-Host "---------------------------------------------"
    }
}

 class client_machine
{
    $user
    $ip
    $port = 22
    $lang = "C"
    $output_dir
    $name = "Client Machine"
    [string]$OS = "Linux"
    [string]$compiler

    #class functions
    [void]outputInfo() {
        Write-Host "----------------- Remote -------------------"
        Write-Host ('OS: {0}' -f $this.OS)
        Write-Host ('Machine name: {0}' -f $this.name)
        Write-Host ('User: {0}' -f $this.user)
        Write-Host ('Machine IP: {0}' -f $this.ip)
        Write-Host ('SSH Port: {0}' -f $this.port)
        Write-Host ('Remote Directory: {0}' -f $this.output_dir)
        Write-Host "---------------------------------------------"
    }
}

class sshConfig{
    #class variables
    [int]$quiet = 0
    [int]$key = 0
    #ensure proper .ssh permissions in wsl for .ssh folder and files
    # sudo apt update
    # sudo apt install openssh-server
    # mkdir ~/.ssh
    # sudo cp /mnt/Users/<user>/.ssh/* ~/.ssh/ 
    # chmod 600 - private key
    # chmod 644 - public key
    # chmod 700 - .ssh folder
    [int]$wsl_X11 = 0
    [int]$srvx_X11 = 0
    [int]$X11 = 0
    [string]$configSettings
    [string]$ssh_cmd
    [string]$scp_cmd

    [void]SSH_SEND_CMD([client_machine]$client, [host_machine]$hostX){
        $this.ssh_cmd = "ssh"
        if( $this.wsl_X11 -eq 1 -or $this.srvx_X11 -eq 1 -or $this.X11 -eq 1) {
            $this.configSettings = ('{0} -Y' -f $this.ssh_cmd)
            #Write-Host $ssh.configSettings
        }
        if($client.port -ne 22){
            $this.ssh_cmd = ('{0} -p {1}' -f $this.ssh_cmd, $client.port)
        }
        if($client.key -ne 0){
            $this.ssh_cmd = ('{0} -i {1}' -f $this.ssh_cmd, $hostX.key)            
        }
        $this.ssh_cmd = ('{0} {1}@{2}' -f $this.ssh_cmd, $client.user, $client.ip)
    }

    [void]sendFiles([client_machine]$client, [host_machine]$hostX){
    $this.configSettings = "scp -r"
    if( $this.quiet -eq 1) {
        $this.configSettings = ('{0} -q' -f $this.configSettings) 
        #Write-Host $ssh.configSettings
    }
    if ($this.key -eq 1) {
        $this.configSettings = ('{0} -i {1}' -f $this.configSettings, $hostX.key)
    }
    if($client.port -ne 22){
        $this.configSettings = ('{0} -P {1}' -f $this.configSettings, $this.port)
    }
    $this.scp_cmd = ('{0} {1} {2}@{3}:''{4}/''' -f $this.configSettings, $hostX.proj_dir, $client.user, $client.ip, $client.output_dir)
}

    [void]displayConfig([int]$X11){
        Write-Host "--------------- ssh config ------------------"
        Write-Host ('Quiet Mode: {0}' -f $this.quiet)
        Write-Host ('Using private key: {0}' -f $this.key)
        Write-Host ('WSL_X11 Forwarding: {0}' -f $this.wsl_X11)
        Write-Host ('SRVX_X11 Forwarding: {0}' -f $this.srvx_X11)
        Write-Host ('X11Forwarding: {0}' -f $X11)
        Write-Host "---------------------------------------------"
    }
}

$client = [client_machine]::new()
$hostX = [host_machine]::new()
$ssh = [sshConfig]::new()

# Write-Host $hostX.proj_dir
# Write-Host $config "<---"

#check config file and set as variables for later usage
function setVariables {
    # get total lines of config
    $content = Get-Content -Path $config
    $totalLines = $content.Count

    for( $i = 0 ; $i -lt $totalLines ; $i++) {

        $currentLine = $content[$i]
        if ( $currentLine.Contains("#") ){
            continue
        }
        # elseif (-not $currentLine.Contains(":")) {
        #    Write-Host "Error in config file, exiting"
        #    Exit
        # }

        $lineContent = $currentLine -split ":"

        #client variables
        if ($lineContent[0] -eq "USERNAME") {
            $client.user = $lineContent[1].Trim()
            #Write-Host ('User: {0}' -f $client.user)
        }
        elseif ($lineContent[0] -eq "HOST_IP"){
            $client.ip = $lineContent[1].Trim()
            #Write-Host $client.ip
        }
        elseif ($lineContent[0] -eq "PORT"){
            $client.port = $lineContent[1].Trim()
            #Write-Host $client.port
        }
        elseif ($lineContent[0] -eq "LANGUAGE"){
            $client.lang = $lineContent[1].Trim()
            #Write-Host $client.lang
        }
        elseif ($lineContent[0] -eq "OUTPUT_DIRECTORY"){
            $client.output_dir = $lineContent[1].Trim()
            #Write-Host $client.output_dir
        }
        elseif ($lineContent[0] -eq "MACHINE_NAME"){
            $client.name = $lineContent[1].Trim()
            #Write-Host $client.output_dir
        }
        elseif ($lineContent[0] -eq "COMPILER"){
            $client.compiler = $lineContent[1].Trim()
            #Write-Host $client.output_dir
        }
        elseif ($lineContent[0] -eq "REMOTE_OS"){
            $client.OS = $lineContent[1].Trim()
            #Write-Host $client.output_dir
        }
        elseif ($lineContent[0] -eq "WSL_X11"){
            $ssh.wsl_X11 = [int]$lineContent[1]
            #Write-Host $client.output_dir
        }
        elseif ($lineContent[0] -eq "SRVX_X11"){
            $ssh.srvx_X11 = [int]$lineContent[1]
            #Write-Host $client.output_dir
        }
        #host variables
        elseif($lineContent[0] -eq "HOST_DRIVE"){
            $hostX.drive = $lineContent[1].Trim()
            #Write-Host $hostX.drive
        }
        elseif($lineContent[0] -eq "PRIVATE_KEY"){
            $hostX.key = ($lineContent[1]).Trim()
            $hostX.key = $hostX.drive + ":" + $hostX.key
            #Write-Host $hostX.key
        }
        elseif($lineContent[0] -eq "PROJECT_NAME"){
            $hostX.ProjectName = ($lineContent[1]).Trim()
            #Write-Host $hostX.ProjectName
        }
        elseif($lineContent[0] -eq "HOST_OS"){
            $hostX.OS = ($lineContent[1]).Trim()
            #Write-Host $hostX.OS
        }

        #scp config
        elseif($lineContent[0] -eq "SCP_QUIET"){
            $ssh.quiet = [int]$lineContent[1]
            #rite-Host ('SSH_KEY: {0}' -f $ssh.$quiet)
        }
        elseif($lineContent[0] -eq "SSH_KEY"){
            $ssh.key = [int]$lineContent[1]
            #Write-Host ('SSH_KEY: {0}' -f $ssh.key)
        }
    }

    #safety check for X11 config
    if($ssh.wsl_X11 -eq 1 -and $ssh.srvx_X11 -eq 1){
        ssh.srvx_X11 = 0
    }
    #check input parameters
    if($language.Length -ne 0){
        $client.lang = $language
    }
    if($compiler.Length -ne 0){
        $client.compiler = $compiler
    }
    if($wsl_gui -ne 0){
        $ssh.wsl_X11 = $wsl_gui
    }
    if( $hostX.OS -ne "Windows"){
        $ssh.wsl_X11 = 0
        $ssh.srvx_X11 = 0
        $ssh.X11 = $X11
    }
    elseif($srvx_gui -ne 0){
        $ssh.srvx_X11 = $srvx_gui
    }

    $client.outputInfo()
    $hostX.outputInfo()
    $ssh.displayConfig($X11)
}

#main function
setVariables
$ssh.sendFiles($client, $hostX) 
$ssh.SSH_SEND_CMD($client, $hostX)
. ./compile.ps1