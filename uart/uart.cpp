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
	int iResult = COMPort_Open(&hPort, dwComNum, 0);

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
        

 //   // Let us perform a simple write of 64 bytes of data.
	//DWORD dwNumBytesWritten;
	//iResult = COMPort_Write(&hPort, wrBuffer, &dwNumBytesWritten);

	//if (iResult != COM_PORT_OP_SUCCESS)
	//{
	//	return iResult;
	//}


	//BOOL bWriteStatus;

 //   // Let us read the data that we wrote.
 //   DWORD dwRead = 0;
 //   DWORD dwBytesRead = 0;
 //   DWORD dwSizeLeftToRead = sizeof(rdBuffer);
 //   BOOL bReadStatus = FALSE;
 //   
	//DWORD dwNumBytesRead;
	//iResult = COMPort_Read(&hPort, rdBuffer, &dwNumBytesRead);

	//if (iResult != COM_PORT_OP_SUCCESS)
	//{
	//	return iResult;
	//}

 //   
 //   // Perform a comparison operation to make sure that we have data integrity.
 //   if (memcmp(wrBuffer, rdBuffer, dwRead) != 0 )
 //   {
 //       _tprintf(L"\n\n\nREAD and WRITE data comparison failed.......\n\n");
 //       CloseHandle(hPort);
 //       return -3;
 //   }

 //   _tprintf(L"\n\n\nSuccessfully Completed Flow Control Enabled UART operation .......\n\n");

	

	if (!SetCommMask(hPort, EV_RXCHAR))
	{
		// [ Error setting communications event mask ]

		return -1;
	}
		

	// Test RX Listen example


	//// Set Values to Transfer - Get
	//for (BYTE kk = 0; kk < 8; kk++)
	//{
	//	wrBuffer[kk] = 0x10 * k + kk;

	//}

	//// ** TRANSFER
	//DWORD dwNumBytesWritten;
	//iResult = COMPort_Write8(&hPort, wrBuffer, &dwNumBytesWritten);

	//if (iResult != COM_PORT_OP_SUCCESS)
	//{
	//	return iResult;
	//}

	// ** RECEIVE
	DWORD dwCommEvent;
	DWORD dwRead;
	char  chRead;			

	WORD usReceiveCounter = 0;

	for (; ; ) 
	{

		if (WaitCommEvent(hPort, &dwCommEvent, NULL)) 
		{
			// [ Read OP defined ]

			BYTE ucByteIndex = 0;

			BYTE bAct = 1;
			while (bAct)
			{
				if (ReadFile(hPort, &chRead, 1, &dwRead, NULL))
				{
					// [ Read OP success ]
							
					if (dwRead)
					{
						// [ NOT EMPTY ]
							
						rdBuffer[ucByteIndex] = chRead;
						ucByteIndex++;

						printf(".");
					}
					else
					{
						// [ EMPTY ]

						// end operation
						bAct = 0;
					}
				}			
				else
				{
					// [ An error occurred in the ReadFile call ]

					break;
				}							

			}//while (bAct) 

			// end string
			rdBuffer[ucByteIndex] = '\0';

			// output
			printf("%Receive %03d: %s \n", usReceiveCounter, rdBuffer);

			usReceiveCounter++;

		}
		else
		{
			// [ Error in WaitCommEvent ]

			break;
		}
					
	}//for (; ; )


	// > Close COM Port
	iResult = COMPort_Close(&hPort);

	_tprintf(L"\n\n\n End of program, Port closed.  \n\n");
	getch();

	if (iResult != COM_PORT_OP_SUCCESS)
	{
		return iResult;
	}


    return 0;
}

