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

#include <project.h>



int main()
{
    int32 PotVal;
    int32 NewPWM;
    PWM_1_Start();
    ADC_DelSig_1_Start();
    ADC_DelSig_1_StartConvert();
    
    
    for(;;)
    {
       PotVal = ADC_DelSig_1_Read32();
        if(PotVal<0x0000){
                PotVal = 0x0000;
            }
            if(PotVal > 0xFDD1){
                PotVal = 0xFFFF;
            }
            NewPWM = ((PotVal * 999) / 0xFFFF);
            
            PWM_1_WriteCompare(NewPWM/2);
           
    }
}

/* [] END OF FILE */
