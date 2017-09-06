
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sys/time.h>
using namespace std;

#include "atcore.h"
#include "common.h"

#include <chrono>
#include <thread>


//************************************
//*** Data
//************************************
const int MAX_FILENAME_LENGTH = 256;

bool b_verbose = false;

char sz_filename[MAX_FILENAME_LENGTH] = "image.bmp";

int i_deviceId = 0;

int i_handle = AT_HANDLE_UNINITIALISED;

double d_exposureTime = 0.1;

AT_64 i64_aoiHeight = 0, i64_aoiWidth = 0;

AT_64 i64_max = 2047;
AT_64 i64_min = 0;

int i_minScale = -1;
int i_maxScale = -1;
    
//************************************
//*** Functions
//************************************
int showHelp()
{
  printf("Andor SDK 3 Image Capture Example Program\n"
         "\n"
         "Usage:\n"
         "    image [-?] [-vV] [-e <expTime>] [-f <filename>] [-d <device>]\n"
         "          [s <min> <max>]\n"
         "\n"
         "Synopsis:\n"
         "  Captures a single full frame image and saves it to a bitmap.\n"
         "\n"
         "Arguments:\n"
         "  -?             : Show this help\n"
         "  -v/-V          : Verbose mode\n"
         "  -e <exptime>   : Sets the exposure to the specified float value\n"
         "  -f <filename>  : Saves the bitmap to the specified file\n"
         "  -d <device>    : Acquires from the device number specified\n"
         "  -s <min> <max> : Scales the image to the output palette with\n"
         "                   the specified min and max count values. If\n"
         "                   unspecified, the max and min of the image is used\n"
         "\n"
         );     
  return 0;
}


int main(int argc, char ** argv)
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	cout << "Camera Temp Values" << endl;
	
	// Get and display the current date/time, based on current system
  timeval tv;
  gettimeofday(&tv, 0);
  cout << "The start time: " << tv.tv_sec << "\t" << tv.tv_usec << endl;
  time_t now = time(0);
  cout << "The start time: " << ctime(&now) << "\t" << now << endl;
  
	sleep_for(nanoseconds(1000000000));

	int i_returnCode;
	AT_H Hndl;
	int i_cameraIndex = 0;
	i_returnCode = AT_InitialiseLibrary( );
	if (i_returnCode == AT_SUCCESS)
  {
    cout << "AT_InitialiseLibrary( ) == AT_SUCCESS\n";
    i_returnCode = AT_Open ( i_cameraIndex, &Hndl );
		if (i_returnCode == AT_SUCCESS)
    {
      cout << "AT_Open ( i_cameraIndex, &Hndl ) == AT_SUCCESS\n";

      // Get temp and temp status:
      time_t now = time(0);
      gettimeofday(&tv, 0);
      const long double sysTime = time(0);
      const long double sysTimeMS = sysTime*1000;
      double temperature = 0;
      double temperatureOld = 0;
      int temperatureStatusIndex = 0;
      AT_GetFloat(Hndl, L"SensorTemperature", &temperature);
      cout << "Temperature (start): " << now << '\t' << temperature << endl;
      cout << "Temperature (start): " << tv.tv_sec << "\t" << tv.tv_usec << '\t' << temperature << endl;
      AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
      cout << "Temperature Status Index: " << temperatureStatusIndex << endl;

      // Activate the cooling
      AT_SetBool(Hndl, L"SensorCooling", AT_TRUE);
      
      // Wait for camera to cooldown
      while(temperatureStatusIndex != 4)
      {
        // First wait for 1/10 secs
        sleep_for(nanoseconds(100000000));
        
        // Store old temp
        temperatureOld = temperature;
        
        // Get the temp/time and status
        now = time(0);
        gettimeofday(&tv, 0);
        AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
        AT_GetFloat(Hndl, L"SensorTemperature", &temperature);
        
        if (temperatureOld != temperature)
        {
          cout << "Temperature (cooling): " << now << '\t' << temperature << endl;
          cout << "Temperature (cooling): " << tv.tv_sec << "\t" << tv.tv_usec << '\t' << temperature << endl;
        }
      }
      
      // Get temp and temp status:
      now = time(0);
      gettimeofday(&tv, 0);
      AT_GetFloat(Hndl, L"SensorTemperature", &temperature);
      cout << "Temperature (end): " << now << '\t' << temperature << endl;
      cout << "Temperature (end): " << tv.tv_sec << "\t" << tv.tv_usec << '\t' << temperature << endl;
      AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
      cout << "Temperature Status Index: " << temperatureStatusIndex << endl;
      
      /*
      // Cant get the string for the status:
      wchar_t* temperatureStatus[256];
      AT_GetEnumStringByIndex(Hndl, L"TemperatureStatus", temperatureStatusIndex, &temperatureStatus, 256);      
      //Wait for temperature to stabalise
      while(wcscmp(L"Stabilised",temperatureStatus) != 0)
      {
        AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
        AT_GetEnumStringByIndex(Hndl, L"TemperatureStatus", temperatureStatusIndex, temperatureStatus, 256);
        cout << "temp = " + to_string(temperature);
      }      
      */

      // Now close the camera
			i_returnCode = AT_Close ( Hndl );
			if (i_returnCode != AT_SUCCESS)
      {
			  // error closing handle
        cout << "AT_Close ( Hndl ) == AT_SUCCESS\n";
			}
		}
	}
	i_returnCode = AT_FinaliseLibrary( );
	if (i_returnCode != AT_SUCCESS)
  {
		//Error FinaliseLibrary
	}

  sleep_for(nanoseconds(2000000000));
	return 0;
}
