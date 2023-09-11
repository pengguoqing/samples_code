//////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
//////////////////////////////////////////////////////////////////////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "systemclass.h"


//////////////////
// MAIN PROGRAM //
//////////////////
int main()
{
    SystemClass* System;
    bool result;


    // Create and initialize the system object.
    System = new SystemClass;

    result = System->Initialize();
    if(!result){
        return -1;
    }

    // Perform the frame processing for the system object.
    System->Frame();

    // Release the system object.
    System->Shutdown();
    delete System;
    System = 0;

    return 0;
}
