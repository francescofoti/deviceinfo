#include <Windows.h>
#include <windef.h>
#include <tchar.h>
#include "DeviceInfo.h"

int AnsiToWide(PTCHAR psDest, char *psSrc, int piDestSize) {
  int i = 0;
  if(piDestSize>0) {
    while ( ( i++ < piDestSize ) && *psSrc) {
      *psDest++ = (TCHAR)*psSrc++;
    }
    *psDest = '\0';
  }
  return i; // count includes the final '\0' terminator
}

VOID DevInfoSetLastError(HDEVINFO hDevInfo) {
  TCHAR sErrText[MAX_ERRSTR_LENGTH];
  sErrText[0] = '\0';
  hDevInfo->dwLastError = GetLastError();
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
                NULL, 
                hDevInfo->dwLastError, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                sErrText, MAX_ERRSTR_LENGTH-1, NULL);
  _tcsncpy_s(hDevInfo->sLastError, MAX_ERRSTR_LENGTH, sErrText, _TRUNCATE);
}

HDEVINFO DICreateDeviceInfo() {
  HDEVINFO data;
  data = (HDEVINFO)malloc(sizeof(DEVINFO));
  ZeroMemory(data, sizeof(DEVINFO));
  return data;
}

VOID DIDestroyDeviceInfo(HDEVINFO hDevInfo) {
  if(hDevInfo) {
    free(hDevInfo);
  }
}

// HDEVINFO Accessors

DWORD DIGetVersion(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (DWORD)0;
  return hDevInfo->Version;
}

BYTE DIGetDeviceType(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (BYTE)0;
  return hDevInfo->DeviceType;
}

BYTE DIGetDeviceTypeModifier(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (BYTE)0;
  return hDevInfo->DeviceTypeModifier;
}

BOOLEAN DIGetRemovableMedia(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (BOOLEAN)0;
  return hDevInfo->RemovableMedia;
}

BOOLEAN DIGetCommandQueueing(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (BOOLEAN)0;
  return hDevInfo->CommandQueueing;
}

BYTE DIGetBusType(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (BYTE)0;
  return hDevInfo->BusType;
}

DWORD DIGetLastError(HDEVINFO hDevInfo) {
  if (hDevInfo == (HDEVINFO)NULL) return (DWORD)0;
  return hDevInfo->dwLastError;
}

DWORD DIGetLastErrorText(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen) {
  if (hDevInfo == (HDEVINFO)NULL) return (DWORD)0;
  _tcsncpy_s(lpRetOutputBuf, iOutputBufLen, hDevInfo->sLastError, _TRUNCATE);
  return hDevInfo->dwLastError;
}

VOID DIGetVendorID(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen) {
  if (hDevInfo == (HDEVINFO)NULL) return;
  if(hDevInfo->iVendorIDPos) {
    _tcsncpy_s(lpRetOutputBuf, iOutputBufLen, &hDevInfo->asInfo[hDevInfo->iVendorIDPos], _TRUNCATE);
  }
  else {
    *lpRetOutputBuf = (TCHAR)0;
  }
}

VOID DIGetProductID(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen) {
  if (hDevInfo == (HDEVINFO)NULL) return;
  if (hDevInfo->iProductIDPos) {
    _tcsncpy_s(lpRetOutputBuf, iOutputBufLen, &hDevInfo->asInfo[hDevInfo->iProductIDPos], _TRUNCATE);
  }
  else {
    *lpRetOutputBuf = (TCHAR)0;
  }
}

VOID DIGetProductRevision(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen) {
  if (hDevInfo == (HDEVINFO)NULL) return;
  if (hDevInfo->iProductRevisionPos) {
    _tcsncpy_s(lpRetOutputBuf, iOutputBufLen, &hDevInfo->asInfo[hDevInfo->iProductRevisionPos], _TRUNCATE);
  }
  else {
    *lpRetOutputBuf = (TCHAR)0;
  }
}

VOID DIGetSerialNumber(HDEVINFO hDevInfo, PTCHAR lpRetOutputBuf, WORD iOutputBufLen) {
  if (hDevInfo == (HDEVINFO)NULL) return;
  if (hDevInfo->iSerialNumberPos) {
    _tcsncpy_s(lpRetOutputBuf, iOutputBufLen, &hDevInfo->asInfo[hDevInfo->iSerialNumberPos], _TRUNCATE);
  }
  else {
    *lpRetOutputBuf = (TCHAR)0;
  }
}

// Query for device information and translate into a custom DEVINFO structure.
// Provide and hDevInfo as a handle previously returned by DICreateDeviceInfo().
// Don't forget to free the allocated memory with DIDestroyDeviceInfo() after (single/multiple) use.
BOOL DIQueryDeviceInfo(HDEVINFO hDevInfo, PTCHAR psDrivePath)
{
  if(hDevInfo == (HDEVINFO)NULL) return FALSE;
  // fool check that the drive path is not garbage
  // (although this will not set the error info/text in hDevInfo)
  if (_tcslen(psDrivePath) > INFOSTRING_MAX_LENGTH) {
    return FALSE;
  }

  // Get a handle to physical drive
  HANDLE hDevice = CreateFile(psDrivePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL, OPEN_EXISTING, 0, NULL);
  if (hDevice == INVALID_HANDLE_VALUE) {
    DevInfoSetLastError(hDevInfo);
    return FALSE;
  }

  // setup the storage property query
  STORAGE_PROPERTY_QUERY tPropertyQuery;
  ZeroMemory(&tPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
  tPropertyQuery.PropertyId = StorageDeviceProperty;
  tPropertyQuery.QueryType = PropertyStandardQuery;

  // first get the storage descriptor header
  STORAGE_DESCRIPTOR_HEADER tDescriptorHeader = { 0 };
  DWORD dwBytesReturned = 0;
  if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
    &tPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
    &tDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
    &dwBytesReturned, NULL))
  {
    DevInfoSetLastError(hDevInfo);
    CloseHandle(hDevice);
    return FALSE;
  }

  // now we know how much to allocate for the full structure info
  DWORD dwOutBufferSize = tDescriptorHeader.Size;
  PBYTE pOutBuffer = (PBYTE)malloc(dwOutBufferSize);
  ZeroMemory(pOutBuffer, dwOutBufferSize);

  // get the full storage device descriptor
  if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
    &tPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
    pOutBuffer, dwOutBufferSize,
    &dwBytesReturned, NULL))
  {
    free(pOutBuffer);
    DevInfoSetLastError(hDevInfo);
    CloseHandle(hDevice);
    return FALSE;
  }

  // copy the information from the descriptor to our custom device info structure
  TCHAR sVendorID[INFOSTRING_MAX_LENGTH];
  TCHAR sProductID[INFOSTRING_MAX_LENGTH];
  TCHAR sProductRevision[INFOSTRING_MAX_LENGTH];
  TCHAR sSerialNumber[INFOSTRING_MAX_LENGTH];

  PSTORAGE_DEVICE_DESCRIPTOR pDevDtor = (PSTORAGE_DEVICE_DESCRIPTOR)pOutBuffer;

  // simple copy assignments for non string data types
  hDevInfo->Version            = pDevDtor->Version;
  hDevInfo->DeviceType         = pDevDtor->DeviceType;
  hDevInfo->DeviceTypeModifier = pDevDtor->DeviceTypeModifier;
  hDevInfo->RemovableMedia     = pDevDtor->RemovableMedia;
  hDevInfo->CommandQueueing    = pDevDtor->CommandQueueing;
  hDevInfo->BusType            = pDevDtor->BusType;

  // Copy and concatenate strings from STORAGE_DEVICE_DESCRIPTOR into DEVINFO's asInfo[]
  // Store along the position of the copied strings in our DEVINFO structure
  DWORD dwOffset; 
  int iPos = 0;
  int iLen = 0;

  hDevInfo->asInfo[0] = '\0';
  _tcsncpy_s(&hDevInfo->asInfo[iPos], INFOSTRING_MAX_LENGTH, psDrivePath, _TRUNCATE);
  iPos += _tcslen(&hDevInfo->asInfo[iPos])+1;
  // Now, having put psDrivePath in asInfo, we can safely assume that
  // any of the following "offsets" must be >0 to be valid.

  // vendor id
  dwOffset = pDevDtor->VendorIdOffset;
  if (dwOffset != 0) { 
    iLen = AnsiToWide((PTCHAR)&sVendorID[0], (char *)(pOutBuffer + dwOffset), INFOSTRING_MAX_LENGTH);
    _tcsncpy_s(&hDevInfo->asInfo[iPos], INFOSTRING_MAX_LENGTH, &sVendorID[0], _TRUNCATE);
    hDevInfo->iVendorIDPos = iPos;  iPos += iLen;
  }
  // product id
  dwOffset = pDevDtor->ProductIdOffset;
  if (dwOffset != 0) {
    iLen = AnsiToWide((PTCHAR)&sProductID[0], (char *)(pOutBuffer + dwOffset), INFOSTRING_MAX_LENGTH); 
    _tcsncpy_s(&hDevInfo->asInfo[iPos], INFOSTRING_MAX_LENGTH, &sProductID[0], _TRUNCATE);
    hDevInfo->iProductIDPos = iPos; iPos += iLen;
  }
  // product revision
  dwOffset = pDevDtor->ProductRevisionOffset;
  if (dwOffset != 0) {
    iLen = AnsiToWide((PTCHAR)&sProductRevision[0], (char *)(pOutBuffer + dwOffset), INFOSTRING_MAX_LENGTH);
    _tcsncpy_s(&hDevInfo->asInfo[iPos], INFOSTRING_MAX_LENGTH, &sProductRevision[0], _TRUNCATE);
    hDevInfo->iProductRevisionPos = iPos; iPos += iLen;
  }
  // at last, serial number
  dwOffset = pDevDtor->SerialNumberOffset;
  if (dwOffset != 0) {
    iLen = AnsiToWide((PTCHAR)&sSerialNumber[0], (char *)(pOutBuffer + dwOffset), INFOSTRING_MAX_LENGTH);
    _tcsncpy_s(&hDevInfo->asInfo[iPos], INFOSTRING_MAX_LENGTH, &sSerialNumber[0], _TRUNCATE);
    hDevInfo->iSerialNumberPos = iPos;
  }
  
  CloseHandle(hDevice);
  free(pOutBuffer);
  return TRUE;
 }
