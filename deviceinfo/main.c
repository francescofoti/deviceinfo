#include <Windows.h>
#include <windef.h>
#include <stdio.h>
#include <tchar.h>
#include "DeviceInfo.h"

// Error(s) returned in main()
#define ERR_GENERAL_FAILURE 1

VOID DisplayError() {
  TCHAR sErrText[MAX_ERRSTR_LENGTH];
  sErrText[0] = '\0';
  DWORD dwLastError = GetLastError();
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    dwLastError,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    sErrText, MAX_ERRSTR_LENGTH - 1, NULL);
  _tprintf(_T("ERROR: %s"), sErrText);
}

VOID DisplayDriveInfo(HDEVINFO hDevInfo, PTCHAR psDrivePath) {
  TCHAR sErrorText[512];
  DWORD dwErrCode;
  BOOL  fOK = FALSE;
  TCHAR sInfo[INFOSTRING_MAX_LENGTH];

  fOK = DIQueryDeviceInfo(hDevInfo, psDrivePath);
  if (!fOK) {
    dwErrCode = DIGetLastErrorText(hDevInfo, &sErrorText[0], sizeof(sErrorText) / sizeof(TCHAR));
    _tprintf(_T("Failed to query device info for drive: %s\n"), psDrivePath); // just the text here
    _tprintf(_T(" Reason is:%s\n"), sErrorText);
  }
  else {
    // which drive
    _tprintf(_T("Drive: %s\n"), psDrivePath);
    // bus type
    PTCHAR asBusType[18] = {
      _T("Unknown"), _T("Scsi"), _T("Atapi"),
      _T("Ata"), _T("1394"),  _T("Ssa"),
      _T("Fibre"), _T("Usb"), _T("RAID"),
      _T("iScsi"), _T("Sas"), _T("Sata"),
      _T("Sd"), _T("Mmc"), _T("Virtual"),
      _T("FileBackedVirtual"), _T("Spaces"), _T("Max")
    };
    STORAGE_BUS_TYPE eBusType = (STORAGE_BUS_TYPE)DIGetBusType(hDevInfo);
    if(eBusType <= BusTypeMax) {
      _tprintf(_T("Bus type            : %s\n"), asBusType[eBusType]);
    }
    else {
      _tprintf(_T("Bus type            : (out of known range)\n"));
    }
    // version
    _tprintf(_T("Version             : %ul\n"), DIGetVersion(hDevInfo));
    // device type (I couldn't find a suitable reference to friendly print this value)
    _tprintf(_T("Device type         : %u\n"), DIGetDeviceType(hDevInfo));
    // device type modifier
    _tprintf(_T("Device type modifier: %u\n"), DIGetDeviceTypeModifier(hDevInfo));
    // removable media ?
    _tprintf(_T("Is a removable media: %u\n"), DIGetRemovableMedia(hDevInfo));
    // command queueing
    _tprintf(_T("Supports cmd Q/ing  : %u\n"), DIGetCommandQueueing(hDevInfo));
    // vendor ID
    DIGetVendorID(hDevInfo, &sInfo[0], INFOSTRING_MAX_LENGTH);
    _tprintf(_T("Vendor ID           : %s\n"), sInfo);
    // product ID
    DIGetProductID(hDevInfo, &sInfo[0], INFOSTRING_MAX_LENGTH);
    _tprintf(_T("Product ID          : %s\n"), sInfo);
    // product revision
    DIGetProductRevision(hDevInfo, &sInfo[0], INFOSTRING_MAX_LENGTH);
    _tprintf(_T("Product Revision    : %s\n"), sInfo);
    // serial number
    DIGetSerialNumber(hDevInfo, &sInfo[0], INFOSTRING_MAX_LENGTH);
    _tprintf(_T("Serial number       : %s\n"), sInfo);
    
    _tprintf(_T("\n"));
  }
}

int main()
{
    // We'll display information for all logical drives
    DWORD dwDrives = 0;
    int   iDrive = 0;
    TCHAR sDrive;

    dwDrives = GetLogicalDrives();
    if(dwDrives==0) {
      DisplayError();
      return ERR_GENERAL_FAILURE;
    }

    HDEVINFO hDevInfo;
    TCHAR    sDrivePath[20];
    int      iDriveCt = 0;

    hDevInfo = DICreateDeviceInfo();
    if (hDevInfo == NULL) {
      _tprintf(_T("Couldn't create DEVINFO structure\n"));
      return;
    }

    for (iDrive = 0; iDrive < 26; iDrive++) {
      if (dwDrives & 1) {
        sDrive = (TCHAR)('A' + iDrive);
        swprintf_s(sDrivePath, 20, _T("\\\\.\\%C:"), sDrive); // that's the logical filename (vs physical, eg: \\.\PhysicalDrive1)
        DisplayDriveInfo(hDevInfo, sDrivePath);
        iDriveCt++;
      }
      dwDrives = dwDrives >> 1;
    }
    _tprintf(_T("%u drive(s) listed.\n"), iDriveCt);

    DIDestroyDeviceInfo(hDevInfo);
    return 0;
}
