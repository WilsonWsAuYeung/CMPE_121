/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
int flag = 1;
int x = 0;
int column = 0;
int row = 0;
#include <project.h>
CY_ISR(ISR) {
  if (flag == 0) {
    flag = 1;
  }
  Timer_ReadStatusRegister();
}
uint8 red[4][4];
uint8 green[4][4];
uint8 blue[4][4];

int main() {
  int k = 0;
  int l = 0;
  // red[0][0] = 1;
  // green[2][2] = 1;
  for (k = 0; k < 4; k++) {

    red[k][k] = 1;
  }
  //    for(l=0;l<4;l++){
  //            green[2][l] = 1;
  //        }
  CyGlobalIntEnable; /* Enable global interrupts. */
  isr_StartEx(ISR);
  Timer_Start();
  uint8 mask;
  for (;;) {
    if (flag == 1) {
      if (row == 4) {
        row = 0;
      }

      OE_Write(1);
      PL_Write(0);
      clk_Write(0);
      Rowz_Write(row);
      //     Control_Reg_1_Write(0b100);
      for (column = 0; column < 32; column++) {
        mask = 0;
        if (column < 4) {
          if (red[row][column] == 1) {
            mask = mask | 0b100;
          }
          if (green[row][column] == 1) {
            mask = mask | 0b010;
          }
          if (blue[row][column] == 1) {
            mask = mask | 0b001;
          }
        }

        RGB_Write(mask);
        clk_Write(1);

        clk_Write(0);
      }
      PL_Write(1);
      PL_Write(0);
      OE_Write(0);

      flag = 0;
      row++;
    }
  }
}

/* [] END OF FILE */
