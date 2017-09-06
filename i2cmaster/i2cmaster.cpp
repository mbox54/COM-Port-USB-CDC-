/*
## Cypress USB-Serial Windows Example source file (i2cmaster.cpp)
## ===========================
##
##  Copyright Cypress Semiconductor Corporation, 2015-2016,
##  All Rights Reserved
##  UNPUBLISHED, LICENSED SOFTWARE.
##
##  CONFIDENTIAL AND PROPRIETARY INFORMATION
##  WHICH IS THE PROPERTY OF CYPRESS.
##
##  Use of this file is governed
##  by the license agreement included in the file
##
##     <install>/license/license.txt
##
##  where <install> is the Cypress software
##  installation root directory path.
##
## ===========================
*/


// i2cmaster.cpp : Defines the entry point for the console application.
// 

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <dbt.h>
#include <conio.h>
#include "..\..\..\library\CyUSBSerialVCP\CyUSBSerialVCP.h"

/** ****************
Data Definitions
*** **************** */     
#define I2C_OPERATING_FREQ  400000
#define PAGE_SIZE           64

/** ****************
Functions
*** **************** */     
/**********************************
Application main() function
*** ******************************** */     

int _tmain(int argc, _TCHAR* argv[])
{
    // Define Local Variables.
    HANDLE hMaster		            = INVALID_HANDLE_VALUE;
    DWORD dwComNum                  = 0;
    CY_VCP_RETURN_STATUS retStatus  = CY_VCP_SUCCESS;

    // Printout in the basic command line window.
   	_tprintf(_T("Cypress Basic I2C Master Communication Test\n\n"));
	_tprintf(_T("Enter Master COM Port (1-99): "));
    _tscanf_s(_T("%d"), &dwComNum, 4);
    _tprintf(_T("\n\n"));

    // We have enough information to open the VCP - I2C device.
    if (CyOpenVCP((UINT16)dwComNum, &hMaster) != CY_VCP_SUCCESS ) {
        _tprintf(_T("Failed to Open the I2C Master device...\n\n"));
        return -1;
    }

    // Now set the I2C Operating frequency.
    CySetI2cFreq_VCP(hMaster, I2C_OPERATING_FREQ);

    // This example write a known pattern to USB-Serial DVK kit such
    // as CYUSB236 or CYUSB234.
    // And the read the same patter from the I2C EEPROM.

    // So allocate the data for Write operation.
    UCHAR wData[PAGE_SIZE];
    memset(wData, 0, sizeof(wData));

    for (int nData = 0; nData < sizeof(wData); nData++ )
        wData[nData] = nData;

    // VCP data buffer for I2C communication.
    CY_VCP_DATA_BUFFER wrBuffer;
    wrBuffer.buffer = wData;
    wrBuffer.length = sizeof(wData);

    // VCP-I2C open succeeded. Now, we can transact data through the device.
    CY_VCP_I2C_DATA_CONFIG i2cWriteConfig;

    // Set Internal EEPROM address. EEPROM uses 2 Byte I2C address scheme.
    i2cWriteConfig.internalAddress = (0x10 * PAGE_SIZE);
    i2cWriteConfig.internalAddressLength = 0x2;

    // I2C device internal 7 bit address.
    i2cWriteConfig.slaveAddress = 0x51;

    // Perform a I2C write using the configuration data that is specified above.
    // Data is present in the wrBuffer variable.
    // EEPROM write operation doesn't need a I2C RESTART, so set RESTART argument
    // to FALSE.
    retStatus = CyI2cWrite_VCP(hMaster, &i2cWriteConfig, &wrBuffer, FALSE, 5000);
    if (retStatus != CY_VCP_SUCCESS )
    {
        // I2C Write Operation Failed with an error.
        _tprintf(_T("Failed to write from I2C Master device...(%d)\n\n"), retStatus);
        CyCloseVCP(hMaster);
        return -2;

    }

    _tprintf(_T("Successfully completed the I2C master device WRITE operation...\n\n"));

    // Give a 5 Millisecond delay per the EEPROM data sheet for write operation 
    // completion.
    Sleep(5);

    /*************************************************************************

                I2C Read Operation Starts ...................

    /************************************************************************/
    
    // Allocate buffer for read operation.
    UCHAR rData[PAGE_SIZE];
    memset(rData, 0, sizeof(rData));

    // Data buffer for I2C communication.
    CY_VCP_DATA_BUFFER rdBuffer;
    rdBuffer.buffer = rData;
    rdBuffer.length = sizeof(rData);


    // EEPROM read operation needs a RESTART condition.
    // Address write operation needs 2 byte address scheme.
    // API doesn't take a special parameter for restart but 
    // restart happens through Internal address length size.
    CY_VCP_I2C_DATA_CONFIG i2cReadConfig;

    // Set Internal EEPROM address. EEPROM uses 2 Byte I2C address scheme.    
    i2cReadConfig.internalAddress = (0x10 * PAGE_SIZE);
    i2cReadConfig.internalAddressLength = 0x2;

    // I2C device internal 7 bit address.
    i2cReadConfig.slaveAddress = 0x51;

    // Perform a I2C Read for the I2C Configuration specified above.
    // Data is present in the data buffer.
    // Internal length great than 0 will initiate a RESTART after internal
    // address write.
    retStatus = CyI2cRead_VCP(hMaster, &i2cReadConfig, &rdBuffer, 5000);
    if (retStatus != CY_VCP_SUCCESS )
    {
        _tprintf(_T("Failed to read from I2C Master device...(%d)\n\n"), retStatus);
        CyCloseVCP(hMaster);
        return -3;
    }

    _tprintf(_T("Successfully completed the I2C master device READ operation...\n\n"));

    // Make sure we read correctly from the device.
    if (memcmp(rdBuffer.buffer, wrBuffer.buffer, PAGE_SIZE) != 0 )
    {
        _tprintf(_T("Data comparison operation failed for I2C Master device...(%d)\n\n"), retStatus);
        CyCloseVCP(hMaster);
        return -3;
    }

    // We are done.....
    _tprintf(_T("WRITE & READ Data comparison yielded NO ERRORS...\n\n"));
    CyCloseVCP(hMaster);

    return 0;
}

