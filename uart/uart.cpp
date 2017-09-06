/*
## Cypress USB-Serial Windows Example source file (uart.cpp)
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

// uart.cpp : Defines the entry point for the console application.
// 
#include "stdafx.h"

#include <stdio.h>
#include <conio.h>
//#include "..\..\..\library\CyUSBSerialVCP\CyUSBSerialVCP.h"

#include "COMPort.h"



/** ****************
Functions
*** **************** */     
int InitProg()
{
	// Printout in the basic command line window.
	_tprintf(_T("Cypress Basic UART Communication Test\n\n"));

	return 0;
}

/*
int COMPort_Open(HANDLE * hPort, DWORD dwComNum)
{		
	// > Open PORT
	char szPort[COM_PORT_STRING_LEN];
	sprintf_s(szPort, sizeof(szPort), "\\\\.\\COM%d", dwComNum);

	// > Define Communication Regime
	// This creates a synchronous handle. So that only a read or write can be performed
	// for this at a particular point of time.
	// For performing Read and Write operation together, please refer to OVERLAPPED COM
	// PORT USAGE example.
	*hPort = CreateFileA(szPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, NULL);
	if (*hPort == INVALID_HANDLE_VALUE) return -1;


	// > Set the Read and Write API time outs.
	// These Values will be over ridden during read/write API timeout.
	COMMTIMEOUTS objTimeout;
	GetCommTimeouts(*hPort, &objTimeout);
	objTimeout.WriteTotalTimeoutConstant = DEFAULT_WRITE_TIMEOUT;
	objTimeout.ReadTotalTimeoutConstant = DEFAULT_READ_TIMEOUT;

	// Set up the time out value for ReadFile and WriteFile API.
	SetCommTimeouts(*hPort, &objTimeout);


	return COM_PORT_OP_SUCCESS;
}


int COMPort_SetConfig(HANDLE * hPort, DWORD dwBaudRate, UCHAR ucByteSize, UCHAR ucStopBits, UCHAR ucParity, BOOL bCTS_flow_ctrl, BOOL bDSR_flow_ctrl)
{
	// NOTE:
	// NAMES:
	// Data Terminal Ready (DTR) - Data Set Ready (DSR)
	// Clear To Send (CTS) - Request To Send (RTS)
	//
	// RESTRICTIONS:
	// 8 or 7 bits are supported.
	// Device supports one or two stop bits.
	// Device supports no parity, odd, even, mark and space.
	//
	// COMMENT:
	// Flow control happen based on option used. 
	// CTS flow control is popularly used.
	// - CTS Flow control.Please interconnect RTS to CTS of the same device.
	// - DSR Flow Control.Please interconnect DTR to DSR of the same device.
	// Between UART, both UART is expected to understand Hardware Flow control.


	// TODO:
	// check COM_Handler for NULL


	DCB dcb;

	// > Get COM Port curr Config
	if (GetCommState(*hPort, &dcb) == FALSE)
	{
		// [ERROR: PORT OP FAILURE]

		return COM_PORT_OP_FAILURE;
	}

	// > Update Config Values
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = ucByteSize;				// 8 or 7 bits are supported.
	dcb.StopBits = ucStopBits;				// Device supports one or two stop bits.
	dcb.Parity = ucParity;					// device supports no parity, odd, even, mark and space.

	dcb.fOutxCtsFlow = bCTS_flow_ctrl;		// This will enable RTS-CTS flow control.
	dcb.fOutxDsrFlow = bDSR_flow_ctrl;		// This will disable DTR-DCR Flow control. 

	// > Set COM Port Config
	if (SetCommState(*hPort, &dcb) == FALSE)
	{
		// [ERROR: PORT OP FAILURE]

		return COM_PORT_OP_FAILURE;
	}



	return COM_PORT_OP_SUCCESS;

}


int COMPort_Write(HANDLE * hPort, UCHAR * v_WriteBuffer, DWORD * dwNumBytesWritten)
{
	// TODO:
	// check COM_Handler for NULL

	*dwNumBytesWritten = 0;

	BOOL bWriteStatus = WriteFile(*hPort, v_WriteBuffer, USBUART_BUFFER_SIZE, dwNumBytesWritten, NULL);

	// > Check Valid PROC
	BYTE errNumber = 0;
	if (bWriteStatus == FALSE)
	{
		// [ERROR: PORT OP FAILURE]

		*dwNumBytesWritten = GetLastError();

		errNumber = COM_PORT_OP_FAILURE;
	}
	else
	{
		if (*dwNumBytesWritten != USBUART_BUFFER_SIZE)
		{
			// [ERROR: TRANSFER MISMATCH]

			errNumber = COM_PORT_OP_MISMATCH;
		}
	}

	if (errNumber != 0)
	{
		// [ERROR CASE]

		CloseHandle(*hPort);

		return errNumber;
	}
	

	return COM_PORT_OP_SUCCESS;
}


int COMPort_Read(HANDLE * hPort, UCHAR * v_ReadBuffer, DWORD * dwNumBytesRead)
{
	// TODO:
	// check COM_Handler for NULL

	BYTE errNumber = 0;
	DWORD dwNumBytesRead_Cyc;
	DWORD dwNumBytesRead_All = 0;
	while ((dwNumBytesRead_All < COM_PORT_OP_FAILURE) && (errNumber == 0))
	{

		BOOL bReadStatus = ReadFile(*hPort, v_ReadBuffer + dwNumBytesRead_All, USBUART_BUFFER_SIZE, &dwNumBytesRead_Cyc, NULL);

		// > Check Valid PROC
		if (bReadStatus == FALSE)
		{
			// [ERROR: PORT OP FAILURE]

			*dwNumBytesRead = GetLastError();

			errNumber = COM_PORT_OP_FAILURE;
		}
		else
		{
			// NOTE: BIG_Buffer MUST be [Times x USBUART_BUFFER_SIZE] or equal.
			// or this PROTECTOR consequencesed to incorrected work!
			if (dwNumBytesRead_Cyc != USBUART_BUFFER_SIZE)
			{
				// [ERROR: TRANSFER MISMATCH]
				
				errNumber = COM_PORT_OP_MISMATCH;
			}
		}

		if (errNumber != 0)
		{
			// [ERROR CASE]

			CloseHandle(*hPort);

			return errNumber;
		}
		else
		{
			// [SUCCESS READ]

			dwNumBytesRead_All += USBUART_BUFFER_SIZE;

			*dwNumBytesRead = dwNumBytesRead_All;
		}	

	} //while ((dwNumBytesRead_All < COM_PORT_OP_FAILURE) && (errNumber == 0)) 	


	return COM_PORT_OP_SUCCESS;
}


int COMPort_Close(HANDLE * hPort)
{
	// TODO:
	// check COM_Handler for NULL

	// > Set COM Port Config
	if (CloseHandle(*hPort) == FALSE)
	{
		// [ERROR: PORT OP FAILURE]

		return COM_PORT_OP_FAILURE;
	}


	return COM_PORT_OP_SUCCESS;
}
*/

/** ********************************
Application main() function
*** ******************************** */     


int _tmain(int argc, _TCHAR* argv[])
{
    // Define Local Variables.
    HANDLE hMaster		            = INVALID_HANDLE_VALUE;
    DWORD dwComNum                  = 0;
	HANDLE hPort					;
	DCB dcb;

	// > Init
	InitProg();

	// > Connect to USBUART Device
	// Output control State
	_tprintf(_T("Enter Master COM Port (1-99): "));

	// Input Conf Parameter
	_tscanf_s(_T("%d"), &dwComNum, 4);

	// > Open USBUART Device	
	char szPort[COM_PORT_STRING_LEN];
	sprintf_s(szPort, sizeof(szPort), "\\\\.\\COM%d", dwComNum);
	int iResult = COMPort_Open(&hPort, dwComNum);

	if (iResult != COM_PORT_OP_SUCCESS)
	{
		return iResult;
	}

	// Output control State
    _tprintf(_T("\n Open ...[OK] \n"));

	// > Set COM Port Config
	iResult = COMPort_SetConfig(&hPort, UART_3M_BAUDRATE, 8, ONESTOPBIT, ONESTOPBIT, 1, 0);

	if (iResult != COM_PORT_OP_SUCCESS)
	{
		return iResult;
	}

    /////////////////////////////////////////////////////////
    // Test, we short the Rx with Tx.
    /////////////////////////////////////////////////////////

    UCHAR wrBuffer[USBUART_BUFFER_SIZE];
    UCHAR rdBuffer[USBUART_BUFFER_SIZE];
    DWORD dwWritten = 0;

    memset(wrBuffer, 0, sizeof(wrBuffer));
    memset(rdBuffer, 0, sizeof(rdBuffer));
    
	for (int nCount = 0; nCount < sizeof(wrBuffer); nCount++)
	{
		wrBuffer[nCount] = nCount;
	}
        

    // Let us perform a simple write of 64 bytes of data.
	DWORD dwNumBytesWritten;
	iResult = COMPort_Write(&hPort, wrBuffer, &dwNumBytesWritten);

	if (iResult != COM_PORT_OP_SUCCESS)
	{
		return iResult;
	}


	BOOL bWriteStatus;

    // Let us read the data that we wrote.
    DWORD dwRead = 0;
    DWORD dwBytesRead = 0;
    DWORD dwSizeLeftToRead = sizeof(rdBuffer);
    BOOL bReadStatus = FALSE;
    
	DWORD dwNumBytesRead;
	iResult = COMPort_Read(&hPort, rdBuffer, &dwNumBytesRead);

	if (iResult != COM_PORT_OP_SUCCESS)
	{
		return iResult;
	}

    
    // Perform a comparison operation to make sure that we have data integrity.
    if (memcmp(wrBuffer, rdBuffer, dwRead) != 0 )
    {
        _tprintf(L"\n\n\nREAD and WRITE data comparison failed.......\n\n");
        CloseHandle(hPort);
        return -3;
    }

    _tprintf(L"\n\n\nSuccessfully Completed Flow Control Enabled UART operation .......\n\n");


	// > Close COM Port
	iResult = COMPort_Close(&hPort);

	if (iResult != COM_PORT_OP_SUCCESS)
	{
		return iResult;
	}

	
    return 0;
}

