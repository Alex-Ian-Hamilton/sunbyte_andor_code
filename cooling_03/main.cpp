
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
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

timeval tv;

int i_handle = AT_HANDLE_UNINITIALISED;

double d_exposureTime = 0.1;

AT_64 i64_aoiHeight = 0, i64_aoiWidth = 0;

AT_64 i64_max = 2047;
AT_64 i64_min = 0;

int i_minScale = -1;
int i_maxScale = -1;

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
    
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

int cooldown( AT_H Hndl )
{
  cout << "In Cooldown Function" << endl;
  // Get time
  gettimeofday(&tv, 0);
  
  // Holders for the temps and temp status:
  double temperature = 0;
  double temperatureOld = 0;
  int temperatureStatusIndex = 0;

  // Read in (and output) the current temperature
  AT_GetFloat(Hndl, L"SensorTemperature", &temperature);
  //cout << "Temperature (start): " << tv.tv_sec << "\t" << tv.tv_usec << '\t' << temperature << endl;
  AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
  //cout << "Temperature Status Index: " << temperatureStatusIndex << endl;
  printf("Temperature (start):\t%ld\t%ld\t%0.2f\t%d\n", tv.tv_sec, tv.tv_usec, temperature, temperatureStatusIndex);
  cout << "In Cooldown Function" << endl;
  
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
    gettimeofday(&tv, 0);
    AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
    AT_GetFloat(Hndl, L"SensorTemperature", &temperature);

    // Only output if the temperature has changed
    if (temperatureOld != temperature)
    {
      //cout << "Temperature (cooling): " << tv.tv_sec << "\t" << tv.tv_usec << '\t' << temperature << endl;
      printf("Temperature (start):\t%ld\t%ld\t%0.2f\t%d\n", tv.tv_sec, tv.tv_usec, temperature, temperatureStatusIndex);
    }
  }

  // Get temp and temp status:
  gettimeofday(&tv, 0);
  AT_GetFloat(Hndl, L"SensorTemperature", &temperature);
  //cout << "Temperature (end): " << tv.tv_sec << "\t" << tv.tv_usec << '\t' << temperature << endl;
  AT_GetEnumIndex(Hndl, L"TemperatureStatus", &temperatureStatusIndex);
  //cout << "Temperature Status Index: " << temperatureStatusIndex << endl;
  
  // Get the file name string
  printf("Temperature (end):\t%ld\t%ld\t%0.2f\t%d", tv.tv_sec, tv.tv_usec, temperature, temperatureStatusIndex);

}

int main(int argc, char ** argv)
{
  cout << "Camera Temp Values" << endl;
  

    // Get and display the current date/time, based on current system
  
  gettimeofday(&tv, 0);
  cout << "The start time: " << tv.tv_sec << "\t" << tv.tv_usec << endl;
  
  sleep_for(nanoseconds(1000000000));

  int i_returnCode;
  AT_H Hndl;
  int i_cameraIndex = 0;
  
  // Start the library
  i_returnCode = AT_InitialiseLibrary( );
  if (i_returnCode == AT_SUCCESS)
  {
    cout << "AT_InitialiseLibrary( ) == AT_SUCCESS\n";
    i_returnCode = AT_Open ( i_cameraIndex, &Hndl );
    if (i_returnCode == AT_SUCCESS)
    {
      cout << "AT_Open ( i_cameraIndex, &Hndl ) == AT_SUCCESS\n";

      // Now cooldown the camera
      i_returnCode = cooldown( Hndl );

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
