DISCLAIMER
==========
All files and instructions in this repo provided "as is", without any warranty 
and so on.

I've just got few sources and made them work together for myself and i'll be 
glad if my work will help someone else.

Apologize in advance for my English and note: this is my first repository 
publishing so i could do something wrong.

SYNOPSIS
========
This repository contains result of my little work with upgrading FreeBSD 7.0 
STABLE to FreeBSD 9.2 STABLE on production web server with MegaRAID SAS 8704ELP 
card on board which is not supported by the native FreeBSD mfi driver.

Files in this repository are based on stable versions of FreeBSD: 7.4, 8.2 and 
9.2 obtained using CVS and LSI MegaRAID Driver from an official LSI website:

Description: http://www.lsi.com/downloads/Public/MegaRAID%20Common%20Files/4.410.01.00_freebsd_components.txt.txt
Download: http://www.lsi.com/downloads/Public/MegaRAID%20Common%20Files/freebsd_components.tgz

According to LSI statement, this mfi driver supports all models below: 

MegaRAID SAS 9240-4i
MegaRAID SAS 9240-8i
MegaRAID SAS 9260-4i
MegaRAID SAS 9260CV-4i
MegaRAID SAS 9260-8i
MegaRAID SAS 9260CV-8i
MegaRAID SAS 9260DE-8i
MegaRAID SAS 9261-8i
MegaRAID SAS 9280-4i4e
MegaRAID SAS 9280-8e
MegaRAID SAS 9280DE-8e
MegaRAID SAS 9280-24i4e
MegaRAID SAS 9280-16i4e
MegaRAID SAS 9260-16i
MegaRAID SAS 8704ELP
MegaRAID SAS 8704EM2
MegaRAID SAS 8708ELP
MegaRAID SAS 8708EM2 
MegaRAID SAS 8880EM2
MegaRAID SAS 8888ELP

Also, current version of LSI's driver and driver from this repository does not 
supports Thunderbolt (whatever it is).

In my case mfi driver was statically built into kernel and works fine so far.

WHAT FOR?
=========
LSI mfi driver probably doesn't work as it is. At least mfiutil (native mfi tool 
from FreeBSD) can not be built with LSI sources and you won't be able to build
the world. 

Don't know if this is a real problem, because mfiutil won't work anyway and you 
will use MegaCli util from FreeBSD ports.

Also driver files have few typos, like it was never really tested.

WHAT IS IN REPO
===============
Repository contains directories for FreeBSD 7.4, 8.2 and 9.2 in which you'll 
find sources for mfi driver and mfiutil, ready for compilation. 

INSTALLATION INSTRUCTIONS
=========================
Upgrade FreeBSD step by step. In my case I went 7.0 => 7.4 => 8.2 => 9.2

1) Obtaining FreeBSD source

First, install cvsup from ports:

	cd /usr/ports/net/cvsup-without-gui
	make install clean

than copy sample config file to /usr/local/etc

	cp /usr/share/examples/cvsup/stable-supfile /usr/local/etc/
	
Edit this copied file and set host and required version of FreeBSD:

	default host=cvsup1.us.FreeBSD.org
	default release=cvs tag=RELENG_7

RELENG fetches STABLE version of sources. Change it to RELENG_8 or RELENG_9 if 
you need 8.2 STABLE or 9.2 STABLE.

Than fetch sources:

	cvsup -L 2 /usr/local/etc/stable-supfile
	
You are now ready to apply our mfi source.

2) Installing mfi source files (target dirs are always the same, source dir is
for 7.4 in this example): 

	cd /usr/src/sys/dev/mfi
	rm *
	cp /this_repo/bsd7/mfi/* ./
	cd /usr/src/sys/modules/mfi
	rm *
	cp /this_repo/bsd7/mfi_module/* ./
	cd /usr/src/usr.sbin/mfiutil/
	rm *
	cp /this_repo/bsd7/mfiutil/* ./
	
3) Adjusting installation files:

Open your kernel configuration file, for example
	
	ee /usr/src/sys/amd64/conf/GENERIC
	
make sure that this line is not commented:

	device          mfi		# LSI MegaRAID SAS

make sure that these lines are commented: 

#	device         cbb             # cardbus (yenta) bridge
#	device         mpt             # LSI-Logic MPT-Fusion
#	device         mps             # LSI-Logic MPT-Fusion 2

Save changes and close the file.
	
Open /usr/src/sys/conf/files

	ee /usr/src/sys/conf/files
	
Make sure that there is a line:
 
	dev/mfi/mfi_syspd.c   optional mfi
	
If there is no such line, add it after "dev/mfi/mfi_cam.c"	

Also, comment lines below, if there are any of them:

#           dev/mfi/mfi_fp.c      optional mfi
#           dev/mfi/mfi_tbolt.c   optional mfi

Last two lines are for thunderbolt support which is disabled in the driver 
provided by LSI.

Save changes and close the file. 

Congrats, you are now ready to build and install your world and kernel:
http://www.freebsd.org/doc/handbook/makeworld.html

FOR THE HORDE!