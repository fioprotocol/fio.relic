#(!)Make sure using WSL2.


##Location of WSL vhdx images on Windows:
c:\Users\serge\AppData\Local\Packages\CanonicalGroupLimited.Ubuntu<...>


##Importing one more WSL2 Ubuntu20.04 distro:
((!)Importing differs from installing by that it does not create an exe. Installing from tar.gz seems to be tricky.)
- download https://cloud-images.ubuntu.com/releases/focal/release-20240408/ubuntu-20.04-server-cloudimg-amd64-wsl.rootfs.tar.gz
- $ wsl --import <custom dirsto name> <custom target distro directory> <distro tar.gz path>  
#Example:
$ wsl --import Ubuntu-20.2 "c:\Users\serge\AppData\Local\Packages\CanonicalGroupLimited.Ubuntu20.04onWindows_80jfkruskfcde" ubuntu-20.04-server-cloudimg-amd64-wsl.rootfs.tar.gz
- run the distro: 
$ wsl -d Ubuntu-20.2
- $ adduser cliver
- $ sudo adduser cliver sudo
- create /etc/wsl.conf
- write there:
[boot]
systemd=true
command=cd /home/cliver
[user]
default=cliver
- $ wsl --terminate Ubuntu-20.2
- $ wsl -d Ubuntu-20.2
- $ sudo apt-get update
- $ sudo apt-get install mc


##Running the distro
$ wsl -d Ubuntu-20.2


#Running the wsl distro for a specific user:
$ wsl -d Ubuntu-20.2 -u cliver/root


##Exporting the WSL image:
$ wsl -l -v
$ wsl --terminate <distroname>
$ wsl --export Ubuntu-20.04 "C:\temp\Ubuntu-20.04.tar"


##Importing the WSL image:
//(!)This will erase your existing distro's file contents.
$ wsl --unregister <distroname>


#Importing your VHD backup
$ wsl --import-in-place <distroname> <Install Location with filename>



