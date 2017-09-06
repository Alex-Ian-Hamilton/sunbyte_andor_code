#include <stdlib.h>
#include <iostream>
#include "atcore.h"
#include "atutility.h"

int main()
{
  int i_retCode;
  i_retCode = AT_InitialiseLibrary();
  if (i_retCode == AT_SUCCESS) {
    i_retCode =  AT_InitialiseUtilityLibrary ();
    if (i_retCode == AT_SUCCESS) {
    	AT_64 iNumberDevices = 0;
    	AT_GetInt(AT_HANDLE_SYSTEM, L"Device Count", &iNumberDevices);
    	if (iNumberDevices > 0) {
        AT_H Hndl;
        i_retCode = AT_Open(0, &Hndl);
        if (i_retCode == AT_SUCCESS) {
          AT_SetEnumeratedString(Hndl, L"Pixel Encoding", L"Mono12Packed");
          AT_SetFloat(Hndl, L"Exposure Time", 0.01);

          double temperature = 0;
          AT_SetBool(Hndl, L"SensorCooling", true);

          int temperatureCount = 0;
          AT_GetEnumCount(Hndl, L"TemperatureControl", &temperatureCount);
          AT_SetEnumIndex(Hndl, L"TemperatureControl", temperatureCount-1);

          int temperatureStatusIndex = 0;
          wchar_t temperatureStatus[256];
          AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
          AT_GetEnumStringByIndex(Hndl, L"TemperatureStatus", temperatureStatusIndex, temperatureStatus, 256);

          while(wcscmp(L"Stabilised",temperatureStatus) != 0) {
            AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
            AT_GetEnumStringByIndex(Hndl, L"TemperatureStatus", temperatureStatusIndex, temperatureStatus, 256);
          }

          //Get the number of bytes required to store one frame
          AT_64 iImageSizeBytes;
          AT_GetInt(Hndl, L"Image Size Bytes", &iImageSizeBytes);
          int iBufferSize = static_cast<int>(iImageSizeBytes);

          //Allocate a memory buffer to store one frame
          unsigned char* UserBuffer = new unsigned char[iBufferSize];

          AT_QueueBuffer(Hndl, UserBuffer, iBufferSize);
          AT_Command(Hndl, L"Acquisition Start");

          unsigned char* Buffer;
          if (AT_WaitBuffer(Hndl, &Buffer,  &iBufferSize, 10000) == AT_SUCCESS){
            //Unpack the 12 bit packed data
            AT_64 ImageHeight;
            AT_GetInt(Hndl, L"AOI Height", &ImageHeight);
            AT_64 ImageWidth;
            AT_GetInt(Hndl, L"AOI Width", &ImageWidth);
            AT_64 ImageStride;
            AT_GetInt(Hndl, L"AOI Stride", &ImageStride);
            unsigned short* unpackedBuffer = new unsigned short[ImageHeight*ImageWidth];
            AT_ConvertBuffer(Buffer, reinterpret_cast<unsigned char*>(unpackedBuffer), ImageWidth, ImageHeight, ImageStride, L"Mono12Packed", L"Mono16");

            //  process unpacked image data

            delete[] unpackedBuffer;
          }
          AT_Command(Hndl, L"Acquisition Stop");
          AT_Flush(Hndl);
          delete[] UserBuffer;
        }
        AT_Close(Hndl);
      }
    }
  }
  AT_FinaliseLibrary();
  AT_FinaliseUtilityLibrary();

  std::cout << "Press any key and enter to exit." << std::endl;
  char ch;
  std::cin >> ch;
}
