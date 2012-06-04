Overview
-----------------------------------------------
This package contains a custom binary distribution of
the RXTX serial package for Java.

Courtesy of Cloudhopper, Inc.
http://rxtx.cloudhopper.net/
http://www.cloudhopper.net/opensource/rxtx/

NOTE: If you include my builds in any of your distributions,
please make sure to at least provide a note of thanks to
Cloudhopper in your own ReleaseNotes.  For example,

"RXTX binary builds provided as a courtesy of Cloudhopper.
Please see http://rxtx.cloudhopper.net/ for more info."

RXTX is a great package, but they were lacking pre-built
binaries for x64 versions of Windows.  I also wanted a
version built explicitly with Microsoft Visual Studio
rather than MinGW.

Please see ReleaseNotes.txt for information about this
specific release.


Customization
-----------------------------------------------

1. I've based my build on recent CVS snapshots. Please
see the ReleaseNotes.txt for information about which
snapshot I based this distribution on.

2. Removed UTS_NAME warning from .c files to match
kernel with the version you compiled against.

3. Changed version in RXTXVersion.jar and in SerialImp.c
to match my release so that I know its compiled via a CVS
snapshot.


win-x86, win-x64, ia64
-----------------------------------------------
Built using Microsoft Visual C++ 2008 - not MinGW. The
x86 and x64 versions are native and do not rely on
any other non-standard windows libraries.  Just drop
in the compiled .dlls that are specific to the version
of Java you run. If you installed the 64-bit version
of the JDK, then install the x64 build.

I've tested the x86 and x64 version with Windows 2008,
2003, and Vista SP1.


linux-i386 & linux-x86_64
-----------------------------------------------
Built using CentOS 5.2 and gcc 4.1.2.

Just drop in the compiled .dlls that are specific to
the version of Java you run. If you installed the 64-bit
version of the JDK, then install the x64 build.

I've tested the x86 and x64 versions with x86 and x64
versions of CentOS 5.0 and 5.2.
