

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "atcore.h"
#include "common.h"

#include <chrono>
#include <thread>


//************************************
//*** Data
//************************************

// Some variables for storing software parameters
int i_deviceId = 0;

int i_handle = AT_HANDLE_UNINITIALISED;



    
//************************************
//*** Functions
//************************************
int showHelp()
{
  printf("Example of opening the camera using the Andor SDK 3\n"
         "\n"
         "Usage:\n"
         "    template [-?]\n"
         "\n"
         "Synopsis:\n"
         "  This program does nothing but initilise the library and open the camera, then closing it correctly.\n"
         "  This is made as a very basic example of the minimum code to connect to the camera.\n"
         "\n"
         "Arguments:\n"
         "  -?             : Show this help\n"
         "\n"
        );     
  return 0;
}


int main(int argc, char ** argv)
{
  // Useful time delay extensions:
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds
  //e.g.: sleep_for(nanoseconds(1000000000));

  // Variables to store the retun codes and camera handel (Hndl)
	int i_returnCode;
	AT_H Hndl;
	int i_cameraIndex = 0;
  
  // Initialise the camera library:
	i_returnCode = AT_InitialiseLibrary( );
  
  // Check the return code for errors
	if (i_returnCode == AT_SUCCESS)
  {
    // The library was loaded fine
    // Open the camera (default is camera with index 0)
    i_returnCode = AT_Open ( i_cameraIndex, &Hndl );
    
    // Check the return code for errors
		if (i_returnCode == AT_SUCCESS)
    {
			// The camera is active/ready
        cout << "No code in here!" << endl;
      
      /*
       *   Put working code in here, ideally direct to functions.
       */

      // Now close the camera
			i_returnCode = AT_Close ( Hndl );
			if (i_returnCode != AT_SUCCESS)
      {
			  // Error
        cout << "Issue closing the camera link using AT_Close ( Hndl )" << endl;
			}
		}
    else
    {
      // Error
      cout << "Issue opening the camera using AT_Open ( i_cameraIndex, &Hndl )" << endl;
    }
	}
  else
  {
    // Error
    cout << "Issue initilising the library using AT_InitialiseLibrary( )" << endl;
  }
  
  // Now close the library
	i_returnCode = AT_FinaliseLibrary( );
	if (i_returnCode != AT_SUCCESS)
  {
		//Error FinaliseLibrary
    cout << "Issue closing the library using AT_FinaliseLibrary( )" << endl;
	}

  // Now end the program
	return 0;
}
