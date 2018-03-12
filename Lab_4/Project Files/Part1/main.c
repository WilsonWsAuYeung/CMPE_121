/*******************************************************************************
 * File Name: main.c
 *
 * Version: 2.0
 *
 * Description:
 *   The component is enumerated as a Virtual Com port. Receives data from the
 *   hyper terminal, then sends back the received data.
 *   For PSoC3/PSoC5LP, the LCD shows the line settings.
 *
 * Related Document:
 *  Universal Serial Bus Specification Revision 2.0
 *  Universal Serial Bus Class Definitions for Communications Devices
 *  Revision 1.2
 *
 ********************************************************************************
 * Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
 * This software is owned by Cypress Semiconductor Corporation and is protected
 * by and subject to worldwide patent and copyright laws and treaties.
 * Therefore, you may use this software only as provided in the license
 *agreement accompanying the software package from which you obtained this
 *software. CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR
 *IMPLIED, WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO,
 *NONINFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *PARTICULAR PURPOSE.
 *******************************************************************************/

#include "stdio.h"
#include <project.h>

#if defined(__GNUC__)
/* Add an explicit reference to the floating point printf library */
/* to allow usage of the floating point conversion specifiers. */
/* This is not linked in by default with the newlib-nano library. */
asm(".global _printf_float");
#endif

#define USBFS_DEVICE (0u)
#define USBUART_BUFFER_SIZE (64u)
#define LINE_STR_LENGTH (20u)
#define USBUART_TEMP_BUFFER_SIZE (512u)
#define BufferMultiplier (64u)
/* The buffer size is equal to the maximum packet size of the IN and OUT bulk
 * endpoints.
 */

#define LINE_STR_LENGTH (20u)

char8 *parity[] = {"None", "Odd", "Even", "Mark", "Space"};
char8 *stop[] = {"1", "1.5", "2"};

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 *
 * Summary:
 *  The main function performs the following actions:
 *   1. Waits until VBUS becomes valid and starts the USBFS component which is
 *      enumerated as virtual Com port.
 *   2. Waits until the device is enumerated by the host.
 *   3. Waits for data coming from the hyper terminal and sends it back.
 *   4. PSoC3/PSoC5LP: the LCD shows the line settings.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  None.
 *
 *******************************************************************************/
int main() {
  // uint16 count;
  // uint8 buffer[USBUART_BUFFER_SIZE];

#if (CY_PSOC3 || CY_PSOC5LP)
  // uint8 state;
  // char8 lineStr[LINE_STR_LENGTH];

  LCD_Start();
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

  int16 total;
  // int8 count1;
  // int8 count2;
  int numtoRun;
  int numRan = 0;
  int BufferRuns = 0;
  // int flag = 1;
  int buff1or2 = 0;
  int leftOver = 0;
  int Lcount = 0;

  uint8 TempStore[USBUART_TEMP_BUFFER_SIZE];
  uint8 remainder[USBUART_BUFFER_SIZE];
  uint8 RxBuffer0[USBUART_BUFFER_SIZE];
  uint8 RxBuffer1[USBUART_BUFFER_SIZE];
  int i = 0;
  LCD_Start();
  CyGlobalIntEnable; /* Enable global interrupts. */

  /* Start USBFS operation with 5-V operation. */
  USBUART_Start(0u, USBUART_3V_OPERATION);
  while (!USBUART_GetConfiguration()) {
  };
  USBUART_CDC_Init();

  for (;;) {
    /* Service USB CDC when device is configured. */
    if (0u != USBUART_GetConfiguration()) {
      /* Check for input data from host. */
      if (0u != USBUART_DataIsReady()) {
        /* Read received data and re-enable OUT endpoint. */
        total = USBUART_GetAll(TempStore);
        numtoRun = (total + leftOver) / 64;
        // leftOver = total % 4;

        if (0u != total) {
          /* Wait until component is ready to send data to host. */
          while (0u == USBUART_CDCIsReady()) {
          }
          /* Send data back to host. */
          buff1or2 = 0;

          while (numtoRun != numRan) {

            if (buff1or2 % 2 == 0) {
              for (i = 0; i < 64; i++) {
                if (Lcount < leftOver) {
                  RxBuffer0[i] = remainder[i];
                  Lcount++;
                } else {
                  RxBuffer0[i] =
                      TempStore[i + (BufferMultiplier * BufferRuns) - leftOver];
                }
              }
              while (0u == USBUART_CDCIsReady()) {
              }
              USBUART_PutData(RxBuffer0, USBUART_BUFFER_SIZE);
              while (0u == USBUART_CDCIsReady()) {
              }

              USBUART_PutData(NULL, 0u);
              buff1or2++;
            } else {

              for (i = 0; i < 64; i++) {
                //
                RxBuffer1[i] =
                    TempStore[i + (BufferMultiplier * BufferRuns) - leftOver];
              }
              while (0u == USBUART_CDCIsReady()) {
              }
              USBUART_PutData(RxBuffer1, USBUART_BUFFER_SIZE);
              while (0u == USBUART_CDCIsReady()) {
              }

              USBUART_PutData(NULL, 0u);

              buff1or2++;
            }

            numRan++;
            BufferRuns++;
          }
          Lcount = 0;
          int aye = leftOver;
          leftOver = (total + aye) % 64;
          for (i = 0; i < leftOver; i++) {
            remainder[i] = TempStore[i + (BufferMultiplier * BufferRuns) - aye];
          }
          LCD_Position(1u, 0u);
          LCD_PrintNumber(numRan);
          LCD_PrintString(" ");
          LCD_PrintNumber(leftOver);
          LCD_PrintString(" ");
          LCD_PrintNumber(BufferRuns);
          // CyDelay(100);
          numRan = 0;
          BufferRuns = 0;
        }
      }
      // numRan=0;

      //        #if (CY_PSOC3 || CY_PSOC5LP)
      //            /* Check for Line settings change. */
      //            state = USBUART_IsLineChanged();
      //            if (0u != state)
      //            {
      //                /* Output on LCD Line Coding settings. */
      //                if (0u != (state & USBUART_LINE_CODING_CHANGED))
      //                {
      //                    /* Get string to output. */
      //                    sprintf(lineStr,"BR:%4ld %d%c%s",
      //                    USBUART_GetDTERate(),\
//                                   (uint16) USBUART_GetDataBits(),\
//                                   parity[(uint16)
      //                                   USBUART_GetParityType()][0],\
//                                   stop[(uint16)
      //                                   USBUART_GetCharFormat()]);
      //
      //                    /* Clear LCD line. */
      //                    LCD_Position(0u, 0u);
      //                    LCD_PrintString("                    ");
      //
      //                    /* Output string on LCD. */
      //                    LCD_Position(0u, 0u);
      //                    LCD_PrintString(lineStr);
      //                }
      //
      //                /* Output on LCD Line Control settings. */
      //                if (0u != (state & USBUART_LINE_CONTROL_CHANGED))
      //                {
      //                    /* Get string to output. */
      //                    state = USBUART_GetLineControl();
      //                    sprintf(lineStr,"DTR:%s,RTS:%s",  (0u != (state &
      //                    USBUART_LINE_CONTROL_DTR)) ? "ON" : "OFF",
      //                                                      (0u != (state &
      //                                                      USBUART_LINE_CONTROL_RTS))
      //                                                      ? "ON" : "OFF");
      //
      //                    /* Clear LCD line. */
      //                    LCD_Position(1u, 0u);
      //                    LCD_PrintString("                    ");
      //
      //                    /* Output string on LCD. */
      //                    LCD_Position(1u, 0u);
      //                    LCD_PrintString(lineStr);
      //                }
      //            }
      //        #endif /* (CY_PSOC3 || CY_PSOC5LP) */
    }
  }
}

/* [] END OF FILE */
