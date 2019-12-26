#pragma once

#define MAX_ERRSTR_LENGTH 512
#define INFOSTRING_MAX_LENGTH 128

typedef struct {
  DWORD   Version;
  BYTE    DeviceType;
  BYTE    DeviceTypeModifier;
  BOOLEAN RemovableMedia;
  BOOLEAN CommandQueueing;
  BYTE    BusType;  // STORAGE_BUS_TYPE enum scope is a byte
  DWORD   dwLastError;
  TCHAR   sLastError[MAX_ERRSTR_LENGTH];
  // indices where strings start in lpStringData
  int     iVendorIDPos;
  int     iProductIDPos;
  int     iProductRevisionPos;
  int     iSerialNumberPos;
  TCHAR   asInfo[5 * INFOSTRING_MAX_LENGTH];
} DEVINFO;

#define HDEVINFO  DEVINFO*

HDEVINFO  DICreateDeviceInfo();
VOID      DIDestroyDeviceInfo(HDEVINFO hDevInfo);
BOOL      DIQueryDeviceInfo(HDEVINFO hDevInfo, PTCHAR psDrivePath);
DWORD     DIGetVersion(HDEVINFO hDevInfo);
BYTE      DIGetDeviceType(HDEVINFO hDevInfo);
BYTE      DIGetDeviceTypeModifier(HDEVINFO hDevInfo);
BOOLEAN   DIGetRemovableMedia(HDEVINFO hDevInfo);
BOOLEAN   DIGetCommandQueueing(HDEVINFO hDevInfo);
BYTE      DIGetBusType(HDEVINFO hDevInfo);
DWORD     DIGetLastError(HDEVINFO hDevInfo);
DWORD     DIGetLastErrorText(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen);
VOID      DIGetVendorID(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen);
VOID      DIGetProductID(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen);
VOID      DIGetProductRevision(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen);
VOID      DIGetSerialNumber(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen);