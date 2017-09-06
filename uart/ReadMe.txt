========================================================================
    CONSOLE APPLICATION : uart Project Overview
========================================================================

This example project demostrates how to use the Win32 Serial Communication API.

Please ensure that the device is configured as CDC-UART using Cypress USB-Serial 
Configuration Utility.

This project Synchronous Read and Write operation using Hardware Flow and without
Hardware Flow Control. Hardware flow works only with Cypress VCP driver and doesn't
work with Microsoft Native driver.


This file contains a summary of what you will find in each of the files that
make up your uart application.


uart.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

uart.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named uart.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
