Original build instructions for Coral can be found here:
https://docs.google.com/document/d/1opVxXawewCnoqNyrQgm2-kre98UbGQDgEdTm68TYRTs/edit

-----------------------------------------
LINUX - RPM systems:
Install dependencies:

yum install scons OpenImageIO OpenImageIO-devel glew glew-devel tbb tbb-devel openexr openexr-devel

Pick the build environment (assuming you're in the coraline base folder):

cp buildEnvs/buildEnv_rpm.py buildEnvs/buildEnv.py

Build and run:
scons -f buildStandalone.py
cd build/coralStandaloneBuild
python launchApp.py


-----------------------------------------
LINUX - DEBIAN systems:
Install dependencies:

sudo apt-get install git scons libopenimageio-dev libboost-dev libboost-python-dev libopenexr-dev libglew-dev libtbb-dev qt4-dev-tools python-qt4-gl

Pick the build environment (assuming you're in the coraline base folder):

cp buildEnvs/buildEnv_debian.py buildEnvs/buildEnv.py

Build and run:
scons -f buildStandalone.py
cd build/coralStandaloneBuild
python launchApp.py


-----------------------------------------
WINDOWS 7:
Install dependencies:
    Download and install Microsoft Visual Studio 2010:
        http://www.google.com/url?q=http%3A%2F%2Fgo.microsoft.com%2F%3Flinkid%3D9709949&sa=D&sntz=1&usg=AFQjCNEW-ZbMFCLAiUjnPBOmMOt8HtxCdw
    
    Download and install Microsoft Windows SDK for Windows 7.1:
        http://www.google.com/url?q=http%3A%2F%2Fwww.microsoft.com%2Fen-us%2Fdownload%2Fconfirmation.aspx%3Fid%3D8279&sa=D&sntz=1&usg=AFQjCNGMynVXXkTx0Mk9s-qAoD73XNPPag

    Download and install Python 2.7 (x86 or x64):
        http://www.python.org/download/
        
    Add Python to environment variables, if it wasn't already added:
        Go to Start menu
        Right click on Computer
        Select Properties
        On left of Control Panel, click Advanced System Settings
        In the Advanced tab, click Environment Variables
        Find PATH in the System Variables, and click Edit
        Add the path to your python installation (C:\Python2.7 by default) and the Scripts folder, separated by a ;
        
    Install Scons (x86 or x64):
        https://sourceforge.net/projects/scons/
        
    Install PyQt4 (x86 or x64):
        http://www.riverbankcomputing.com/software/pyqt/download
        
    Download addition Microsoft Visual C++ files:
        https://www.google.com/url?q=https%3A%2F%2Fdl.dropbox.com%2Fu%2F10965660%2Fcoral_site%2Fdownload%2Fvs2010-addons.zip&sa=D&sntz=1&usg=AFQjCNG4uLrNY4ct-AvfWBGukOTp_m1rAg
        Unzip a VC folder to Visual Studio path: C:\Program Files (x86)\Microsoft Visual Studio 10.0

    Download additional coral files:
        https://www.google.com/url?q=https%3A%2F%2Fdl.dropbox.com%2Fu%2F10965660%2Fcoral_site%2Fdownload%2Fcoral-repo.zip&sa=D&sntz=1&usg=AFQjCNHbaN9S9UpZflMBjVWuLpO3lgM8EQ
        unzip to cloned repo folder

Building:
    Tell Scons where VC is:
        In the following folders you'll find a SContruct file.  Near the top of the file is a variable "msStudioPath".  Make sure that is the correct installation path of Visual Studio.
            coral
            coralUi
            imath
    
    Check the buildEnvs/buildEnv_win.py paths, to make sure they are correct, then make a copy of the buildEnv_win.py as buildEnvs/buildEnv.py

    Run one of the vs_cmd_x## links to set up the VC headers and libraries.
    
    Change directory into the "coral" subfolder of the repository and run:
        scons -f buildStandalone.py

                