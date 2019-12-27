# The deviceinfo C standalone console EXE project

Windows provides information about different sorts of attached devices, like hard disks, CDROMs, printers and others. These devices can be attached to the computer via different interfaces like USB, SCSI, serial or parallel ports, etc.

This project, "deviceinfo" is a console program written in C, that is built around the DeviceIoControl() Win32 API function. It will retrieve and display a (small) set of information about all connected devices that are mapped to a logical drive letter in Windows. This includes connected HDDs, SSDs, and other USB devices.

We can also access the devices information thru the [Windows Management Instrumentation](https://en.wikipedia.org/wiki/Windows_Management_Instrumentation) (WMI) extensions. While the Win32 API is light and fast, WMI is much more high level and a humongous beast in comparison. WMI is far more feature rich than this raw Win32 API and also normalizes access to more than just device specific information while being more scripting friendly. 

However, going low level with the Win32 API when we only need a specific subset of useful information about a device, like the device serial number or vendor information, thru a "low level" language like C, has significant benefits, some of them being:

*  It is independent of changes in the WMI interface, which has already morphed into [MI](https://docs.microsoft.com/fr-fr/windows/win32/wmisdk/wmi-start-page);
* It is lighter and faster;
* It is far more easier to program against, particularly with languages like C/C++.
* Just worry about having/distributing one compiled exe (don't worry too much about the C runtime library, I mean, literally everybody already has it installed).

## Some technical details

The information that is collected about a device using this API is documented on MSDN in the [STORAGE_DEVICE_DESCRIPTOR](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddstor/ns-ntddstor-_storage_device_descriptor) structure.

The DEVINFO structure that you see defined in the header file DeviceInfo.h is custom made. I know this structure name is overused and already exists somewhere I don't know, in the C world, but that's not a problem here, as it is only used "internally" in the DeviceInfo.c source file.

HDEVINFO is yet another "handle", which is really the pointer to the memory allocated for a DEVINFO structure, as returned by malloc(). Which, in the 32bits world, ends up to be a 32bits unsigned integer.

If you take a look at the source code in DeviceInfo.c, you may find it ugly, at first sight, to write another API wrapping this Win32 API (the "*DIxxx*" functions). The reason I did that, is that this project is just the test base for me, for another project (that I'll share on github too) that turns it into a dynamic link library (DLL) that is Visual Basic friendly (classic VB/VBA), and also easily usable with other similarly capable scripting (or not) languages.

This is a Visual Studio 2017, 32bits, UNICODE, project. The produce EXE needs the presence of the corresponding [Visual C 2017 runtime](https://support.microsoft.com/fr-ch/help/2977003/the-latest-supported-visual-c-downloads) (x86) installed on the target computer to function properly.

## Download binaries

A compiled exe from this project is downloadable (from my company website) here:

[download deviceinfo.exe from devinfo.net](https://devinfo.net/downloads/deviceinfo.exe) (MD5 sum: f80aba913810129755df58324fc18e22)

## FAQ

* And a 64bits version ?
  * Nope. This project will be turned into a 32bits DLL. The 32bits DLL will be wrapped in a Visual Basic 5 "out of process" ActiveX EXE server and as such, it will be usable by 32 and 64bits VBA (or other ActiveX client capable) hosts. This project is part of a 3 parts series of articles I'm posting on my personal website, I'll update the links here as they're published.
* Why not consume the Win32 API directly from VB/VBA ? 
  * It was cleaner (no ugly declares or pointer and bits arithmetic in VB), easier, more readable, extensible and maintainable to do it in C (IMHO).
