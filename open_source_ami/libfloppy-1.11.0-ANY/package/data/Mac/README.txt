Steps to be followed to create the Mac FLOPPY library
-----------------------------------------------------

      To compile and create the Mac FLOPPY library, we have to mount our source to Mac box.
      
TIPS TO MOUNT SOURCE IN MAC BOX
-------------------------------
	Before mounting the source in mac box, we have to change the mount permission in our linux box which has source.

1) In Linux box which have the source files , Edit the file /etc/exports and enter the permission of the shared folder as "insecure".
	
	For Example
	***********
	
	/home/sengud/megarac/ast2100evb 		*(rw,sync,no_root_squash,insecure)
	
2) Save the "/etc/exports" file and restart the nfs service of the Linux box.

3) Go to your Mac box and open the terminal window. 

mount command syntax is as below

	mount syntax:
	************
		mount -t nfs SourceMachineIP:SourcePath mountPath

	For Example
	***********
		mount -t nfs 10.0.0.251:/home/sengud/megarac/ast2100evb /mnt
		
        Where 10.0.0.251 is your source machine ip, /home/sengud/megarac/ast2100evb (SourcePath) is the source path and /mnt (mountPath)is the folder to be mount in the mac box.
	

TO CREATE MAC LIBFLOPPY LIBRARY
-------------------------------
	
1) After mounting the source in mac box, go the path "development/proprietary/software/LIBFLOPPY/src/Mac" and type the command "make clean; make all".
	"libfloppyMac.a" will be created in the path "development/proprietary/software/LIBFLOPPY/obj/Mac/libfloppyMac.a"

2) Go to the path "development/proprietary/software/javafloppywrapper/mac/" and give the command "make". 

	Mac floppy library will be created in the path "development/proprietary/software/javafloppywrapper/output/mac/libjavafloppywrapper.jnilib"
	


