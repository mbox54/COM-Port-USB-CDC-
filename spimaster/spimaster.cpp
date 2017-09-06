/*
## Cypress USB-Serial Windows Example source file (spimaster.cpp)
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

// spimaster.cpp : Defines the entry point for the console application.
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
#define SPI_OPERATING_FREQ  3000000
#define PAGE_SIZE           256
#define SPI_WRITE_CMD       0x2
#define SPI_READ_CMD        0x3
#define SPI_STATUS_QUERY    0x5
/** ****************
Functions
*** **************** */     

/*
Function Name: CY_VCP_RETURN_STATUS cySPIWriteEnable (HANDLE cyHandle)
Purpose: Function to send Write Enable command to SPI

Arguments:
cyHandle - cyHandle of the device
Retrun Code: returns falure code of USB-Serial API

*/


CY_VCP_RETURN_STATUS cySPIWriteEnable (HANDLE cyHandle)
{
    unsigned char wr_data,rd_data;
    CY_VCP_RETURN_STATUS status = CY_VCP_SUCCESS;
    CY_VCP_DATA_BUFFER writeBuf;
    CY_VCP_DATA_BUFFER readBuf;

    printf("\nSending SPI Write Enable command to device...");
    writeBuf.buffer = &wr_data;
    writeBuf.length = 1;

    readBuf.buffer = &rd_data;
    readBuf.length = 1;

    wr_data = 0x06; /* Write enable command*/

    status = CySpiReadWrite_VCP (cyHandle, &readBuf,&writeBuf, 5000);
    if (status != CY_VCP_SUCCESS)
    {
        printf("\nFailed to send SPI Write Enable command to device.");
        return status;
    }    
    printf("\nSPI Write Enable command sent successfully.");
    return status;

}



/*
Function Name: CY_VCP_RETURN_STATUS cySPIWaitForIdle (HANDLE cyHandle)
Purpose: Function to check for SPI status

Arguments:
cyHandle - cyHandle of the device
Retrun Code: returns falure code of USB-Serial API

*/

CY_VCP_RETURN_STATUS cySPIWaitForIdle (HANDLE cyHandle)
{
    char rd_data[2], wr_data[2];
    CY_VCP_DATA_BUFFER writeBuf, readBuf;
    int nMaxRetries = 0xFFFF;
    CY_VCP_RETURN_STATUS status;


    printf("\nSending SPI Status query command to device...");
    writeBuf.length = 2;
    writeBuf.buffer = (unsigned char *)wr_data;

    readBuf.length = 2;
    readBuf.buffer = (unsigned char *)rd_data;

    // Loop here till read data indicates SPI status is not idle
    // EEPROM status 0x1 indicates that EEPROM is IDLE.
    // We are performing two bytes write here.
    // First Byte of write carries the Status Query command.
    // And, Second byte of Read has the EEPROM status.
    do
    {
        wr_data[0] = SPI_STATUS_QUERY; /* Get SPI status */
        status = CySpiReadWrite_VCP (cyHandle, &readBuf, &writeBuf, 5000);

        if (status != CY_VCP_SUCCESS)
        {
            printf("\nFailed to send SPI status query command to device.");
            break;
        }
        nMaxRetries--;
        if (nMaxRetries == 0)
        {
            printf("\nMaximum retries completed while checking SPI status, returning with error code.");
            status = CY_VCP_ERROR_IO_TIMEOUT;
            return status;
        }

    } while (rd_data[1] & 0x01); //Check SPI Status

    printf("\nSPI is now in idle state and ready for receiving additional data commands.");
    return status;
}



/** ********************************
Application main() function
*** ******************************** */     


int _tmain(int argc, _TCHAR* argv[])
{
    // Define Local Variables.
    HANDLE hMaster		            = INVALID_HANDLE_VALUE;
    DWORD dwComNum                  = 0;
    CY_VCP_RETURN_STATUS retStatus  = CY_VCP_SUCCESS;

    // Printout in the basic command line window.
   	_tprintf(_T("Cypress Basic SPI Master Communication Test\n\n"));
	_tprintf(_T("Enter Master COM Port (1-99): "));
    _tscanf_s(_T("%d"), &dwComNum, 4);
    _tprintf(_T("\n\n"));

    // We have enough information to open the VCP - SPI device.
    if (CyOpenVCP((UINT16)dwComNum, &hMaster) != CY_VCP_SUCCESS ) {
        _tprintf(_T("Failed to Open the SPI Master device...\n\n"));
        return -1;
    }

    // Now set the SPI Operating frequency.
    CY_VCP_SPI_CONFIG spiMasterConfig;
    spiMasterConfig.frequency = SPI_OPERATING_FREQ;
    spiMasterConfig.dataWidth = 8;
    CySetSpiConfig_VCP(hMaster, &spiMasterConfig);

    // This example write a known pattern to USB-Serial DVK kit such
    // as CYUSB236 or CYUSB234.
    // And the read the same patter from the SPI EEPROM.

    // So allocate the data for Write & Read operation.
    UCHAR wData[PAGE_SIZE+4], rData[PAGE_SIZE+4];
    memset(wData, 0, sizeof(wData));
    memset(rData, 0, sizeof(rData));

    // VCP data buffer for SPI communication.
    CY_VCP_DATA_BUFFER wrBuffer, rdBuffer;
    wrBuffer.buffer = wData;
    wrBuffer.length = sizeof(wData);
    rdBuffer.buffer = rData;
    rdBuffer.length = sizeof(rData);


    /****************************************

    Start SPI EEPROM Write Operation.

    ****************************************/
    CY_VCP_RETURN_STATUS rStatus = cySPIWaitForIdle (hMaster); 
    if (rStatus){
        _tprintf(L"Error in Waiting for EEPOM active state:0x%X \n", rStatus);            

        return rStatus;
    }
    rStatus =  cySPIWriteEnable (hMaster);
    if (rStatus){
        _tprintf(L"Error in setting Write Enable:0x%X \n", rStatus);            
        return rStatus;
    }

    // Perform SPI Write now.
    UINT32 startAddress = 0x10*(PAGE_SIZE);
    wData[0] = SPI_WRITE_CMD; 
    wData[1] = (startAddress >> 16)&0xFF; //Page address
    wData[2] = (startAddress >> 8)&0xFF;
    wData[3] =  startAddress&0xFF; 

    // Fill the data buffer
    for (unsigned int index = 0; index < PAGE_SIZE ; index++)        
        wData[index+4] = index;			

    // This is Write ONLY SPI operation for the API. However, by the SPI
    // protocol the device will read the data & discard.
    rStatus = CySpiReadWrite_VCP (hMaster, NULL, &wrBuffer, 5000);
    if (rStatus != CY_VCP_SUCCESS){           

        _tprintf(L"Error in doing actual SPI data write:0x%X (Status = %d) \n" , wrBuffer.transferCount, rStatus);
        CyCloseVCP(hMaster);
        return rStatus;    
    } 

    _tprintf(_T("\n\nSuccessfully completed the SPI master device WRITE operation...\n\n"));

    // Delay needed for SPI EEPROM write operation to complete (Per datasheet - SPI EEPROM)
    Sleep(5);


    /****************************************

    Start SPI EEPROM Read Operation.

    ****************************************/

    // Wait till SPI EEPROM is available for SPI Transaction.
    rStatus = cySPIWaitForIdle (hMaster); 
    if (rStatus){
        _tprintf(L"Error in Waiting for EEPOM active state:0x%X \n", rStatus);            

        return rStatus;
    }

    // Perform SPI Read now.
    startAddress = 0x10*(PAGE_SIZE);
    wData[0] = SPI_READ_CMD; 
    wData[1] = (startAddress >> 16)&0xFF; //Page address
    wData[2] = (startAddress >> 8)&0xFF;
    wData[3] =  startAddress&0xFF; 

    // Initiate a SPI READ and Write Operation together.
    // The intension is to read. First four bytes sends the command to SPI
    // EEPROM to start the READ operation from the given Address Location.
    // Remaining PAGE_SIZE bytes of data is need to complete the actual
    // Read Operation.
    rStatus = CySpiReadWrite_VCP (hMaster, &rdBuffer, &wrBuffer, 5000);
    if (rStatus != CY_VCP_SUCCESS){           

        _tprintf(L"Error in doing actual SPI EEPROM READ :0x%X (Status = %d) \n" , wrBuffer.transferCount, rStatus);
        CyCloseVCP(hMaster);
        return rStatus;    
    } 

    _tprintf(_T("\n\nSuccessfully completed the SPI master device READ operation...\n\n"));

    // Make sure we read correctly from the device.
    if (memcmp(rdBuffer.buffer+4, wrBuffer.buffer+4, PAGE_SIZE ) != 0 )
    {
        _tprintf(_T("Data comparison operation failed for SPI Master device...(%d)\n\n"), rStatus);
        CyCloseVCP(hMaster);
        return rStatus;    
    }

    _tprintf(_T("WRITE & READ Data comparison yielded NO ERRORS...\n\n"));
    return CY_VCP_SUCCESS;
}

