#include <msp430.h>
#include "i2c_lcd.h"

int gtmacdinh = 0;
unsigned long dem = 0;
unsigned char need_update_lcd = 1;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    P2DIR &= ~0x08;
    
    LCD_Init(0X3F, 2, 16);
    LCD_Clear();
    LCD_SetCursor(3,0);
    LCD_Print("Project VXL");
    LCD_SetCursor(2,1);
    LCD_Print("Chao Cac Ban!");
    __delay_cycles(5000000);
    LCD_Clear();
    LCD_SetCursor(6,0);
    LCD_Print("Group");
    LCD_SetCursor(2,1);
    LCD_Print("Le Quang Vinh");
    __delay_cycles(5000000);
    LCD_Clear();

    unsigned long prev_dem = 0;
    while(1)
    {
        if((P2IN & 0x08) != gtmacdinh)
        {
            if((P2IN & 0x08) == 0)
            {
                dem++;
                __delay_cycles(10);
            }
            gtmacdinh = (P2IN & 0x08);
        }
        
        if (dem != prev_dem || (dem == 0 && need_update_lcd))
        {
            prev_dem = dem;
            
            LCD_Clear();
            LCD_SetCursor(2,0);
            LCD_Print("Dem San Pham");
            LCD_SetCursor(4,1);
            lcd_put_num(dem,0,0);
            LCD_SetCursor(10,1);
            LCD_Print("SP");
            
            if (dem == 0)
                need_update_lcd = 0;
        }
        

    }
}
