/*
 * Some comments here
 */

#include "main.h"
#include "i2c_lcd.h"

// global 
uint8_t code[10], user[5] = {0};
uint8_t i, count;

void buzzer(uint8_t mode)
{
    RB2 = (__bit)mode;
}

void warning_buzzer(void){
    buzzer(ON);
    __delay_ms(500);
    buzzer(OFF);
    __delay_ms(500);
}

void press_button_buzzer(void){
    buzzer(ON);
    __delay_ms(100);
    buzzer(OFF);
}

void obstacles_detect_buzzer(void){
    buzzer(ON);
    __delay_ms(500);
    buzzer(OFF);
    __delay_ms(500);
}   

void wrong_password_buzzer(){
    buzzer(ON);
    __delay_ms(300);
    buzzer(OFF);
    __delay_ms(100);
    buzzer(ON);
    __delay_ms(500);
    buzzer(OFF);
    __delay_ms(100);
}

void initialization_buzzer(){
    buzzer(ON);
    __delay_ms(350);
    buzzer(OFF);
    __delay_ms(150);
    buzzer(ON);
    __delay_ms(350);
    buzzer(OFF);
    __delay_ms(150);
}

uint8_t keys[16] = {
        '1', '2', '3', 'A',
        '4', '5', '6', 'B',
        '7', '8', '9', 'C',
        '*', '0', '#', 'D'
        };
uint8_t key;

void key_press(void)
{
    uint8_t i;
    key = 0xFF;
    for (i = 0; i < 4; i++)
    {
        row0 = 0x01 >> i;
        row1 = (__bit)(0x02 >> i);
        row2 = (__bit)(0x04 >> i);
        row3 = (__bit)(0x08 >> i);
        if (col0)
        {
            key = (i * 4);
            break;
        }
        if (col1)
        {
            key = (i * 4 + 1);
            break;
        }
        if (col2)
        {
            key = (i * 4 + 2);
            break;
        }
        if (col3)
        {
            key = (i * 4 + 3);
            break;
        }
    }
}

void code_enter(void)
{
    do
        key_press();
    while (key == 0xFF);
    if (key != 0xFF)
    {   
        __delay_ms(5);
        key_press();
        press_button_buzzer();
        code[i] = keys[key];
        LCD_Write_Char(code[i]);
        i++;
        while (key != 0xFF)
        {   
            key_press();
        }
    }
}

// ADC HW 201 sensor 
uint16_t ADC_value;

void ADC_init(){
    //------[There are 2 registers to configure ADCON0 and ADCON1]---------
    // ADCON0 = 0x41
    // Select clock option Fosc/8
    ADCS0 = 1;
    ADCS1 = 0;
    // Turn ADC on 
    ADON = 1;
    
    //ADCON1 = 0x80
    // Result mode: Right justified
    ADFM = 1;
    // Select clock option Fosc/8
    ADCS2 = 0;
    // Configure all 8 channels are analog 
    PCFG0 = 0;
    PCFG1 = 0;
    PCFG2 = 0;
    PCFG3 = 0;
}

uint16_t ADC_read(uint8_t ADC_channel){
    // Check channel number
    if (ADC_channel > 7) return 0;
    
    // Write ADC__channel into register ADCON0
    CHS0 = (ADC_channel & 1) >> 0;
    CHS1 = (ADC_channel & 2) >> 1;
    CHS2 = (ADC_channel & 4) >> 2;
    
    // Wait the Acquisition time 
    __delay_us(25);
    
    // Start A/D conversion
    GO_DONE = 1;
    
    // (Polling) Wait for the conversion to complete
    while(GO_DONE);
    
    // Read the ADC result ("right justified" mode)
    uint16_t result = (uint16_t)((ADRESH << 8) + ADRESL);
    return result;
}

uint16_t icount = 0;
uint16_t duty = max_duty;

void open_door(void){
    uint16_t i;
    for (i = min_duty; i <= max_duty; i++)
    {
        duty = i;
        __delay_ms(200);
    }
}

void close_door(void){
    uint16_t i = max_duty;
    while (i > min_duty)
    {
        ADC_value = ADC_read(ADC_sensor);
        if (ADC_value > threshhold)
        {
            i--;
            __delay_ms(200);
        }
        else
        {
            obstacles_detect_buzzer();
        }
        duty = i;
    }
}

// EEPROM
uint8_t EEPROM_Read(uint8_t Address){
    // Step 1
    EEADR = Address;
    // Step 2
    EEPGD = 0;
    // Step 3
    EECON1bits.RD = 1;
    // Step 4
    return EEDATA;
}

void EEPROM_Write(uint8_t Address, uint8_t Data){
    // Step 1
    while(WR);
    // Step 2
    EEADR = Address;
    // Step 3
    EEDATA = Data;
    // Step 4
    EEPGD = 0;
    // ------------ Step 5
    EECON1bits.WREN = 1;
    // Step 6
    GIE = 0;
    // Step 7
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    // Step 8
    GIE = 1;
    // Step 9
    EECON1bits.WREN = 0;
    // Step 10
    EECON1bits.WR = 0;
}

void code_read(void)  //Read data from EEPROM
{
    __delay_ms(20);
    user[0] = EEPROM_Read(0x00);  // Read data from address 0
    __delay_ms(20);
    user[1] = EEPROM_Read(0x01); 
    __delay_ms(20);
    user[2] = EEPROM_Read(0x02); 
    __delay_ms(20);
    user[3] = EEPROM_Read(0x03); 
    __delay_ms(20);
}

void code_write(void) //Write data to EEPROM
{
    __delay_ms(20);
    EEPROM_Write(0x00,code[0]);               // Write some data at address 00
    __delay_ms(20);
    EEPROM_Write(0x01,code[1]);               // Write some data at address 02
    __delay_ms(20);
    EEPROM_Write(0x02,code[2]);               // Write some data at address 04
    __delay_ms(20);
    EEPROM_Write(0x03,code[3]);               // Write some data at address 08
}

void change_code(void)
{
    i = 0;
    code_enter();
    code_enter();
    code_enter();
    code_enter();
    code_write();
    code_read();
    __delay_ms(200);
}

void main(void)
{
    // Configure I/O
    // SDA SCL for LCD TRISC3-4
    PORTB = 0x01; // pull up RB0
    TRISB = 0x01; // Interrupt RB0 // RB1 for servo // RB2 for buzzer
    PORTA = 0x00; 
    TRISA = 0x01; // ADC AN0 
    PORTD = 0x00;
    TRISD = 0x0F; // keypad
    
    // Configure Timer1
    // Clear the Timer1 register to start counting from 0
    TMR1 = TMR1_base;                   
    // Clear the Timer1 clock select bit to choose local clock source
    TMR1CS = 0;                 
    // Pre scaler ratio 1:8
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    // Switch ON Timer1
    TMR1ON = 1;
    // Clear interrupt flag bit
    TMR1IF = 0;
    TMR1IE = 1;
    PEIE = 1;
    // Interrupt RB0
    INTEDG = 1;                 // Interrupt edge config bit (HIGH value means interrupt occurs every rising edge)
    INTE = 1;                   // IRQ (Interrupt request pin RB0) Interrupt Enable bit
    GIE = 1;                    // Global Interrupt Enable bit
    
    ADC_init();
    //ADC_value = ADC_read(ADC_sensor);
    
    I2C_Master_Init();
    LCD_Init(0x4E);    // Initialize LCD module with I2C address = 0x4E
 
    buzzer(OFF);
    close_door();
    
    LCD_Set_Cursor(1, 1);
    LCD_Write_String("  Welcome To ");
    LCD_Set_Cursor(2, 1);  
    LCD_Write_String("  Smart Door ");
    __delay_ms(500);
    initialization_buzzer();
    
    code_read();
    //If no code is stored then default is 2345
    if(user[0] == 0xFF && user[1] == 0xFF && user[2] == 0xFF && user[3] == 0xFF )
    {
        EEPROM_Write(0x00,'2');               // Write some data at address 00
        __delay_ms(20);
        EEPROM_Write(0x01,'3');               // Write some data at address 02
        __delay_ms(20);
        EEPROM_Write(0x02,'4');               // Write some data at address 04
        __delay_ms(20);
        EEPROM_Write(0x03,'5');               // Write some data at address 08
        __delay_ms(20);
        code_read();
    }

    count = 0;
    while(1)
    {   
        LCD_Clear();
        LCD_Write_String("Press *");
        LCD_Set_Cursor(2,1);
        i = 0;
        code_enter();
        if(code[0] == '*')
        {
            LCD_Clear();
            LCD_Write_String("Enter code");
            LCD_Set_Cursor(2,1);
            i = 0;
            code_enter();
            code_enter();
            code_enter();
            code_enter();
            code_enter();
            __delay_ms(200);
            if(code[0] == '2' && code[1] == '3' && 
                    code[2] == '4' && code[3] == '5' && code[4] == '5')  //check master code = 23455 ? for change the code 
            {
                code_enter();
                if(code[5] == '#')
                {
                    LCD_Clear();
                    LCD_Write_String("Enter new code");
                    LCD_Set_Cursor(2,1);
                    change_code();
                    LCD_Clear();
                    LCD_Write_String("New code set");
                    __delay_ms(1000);
                }
            }
            else if(count < 3 && code[0] == user[0] && code[1] == user[1] && 
                    code[2] == user[2] && code[3] == user[3] && code[4] == '#') //compare code with store one
            {
                LCD_Clear();
                LCD_Write_String("Welcome");
                __delay_ms(500);
                buzzer(ON);
                open_door();
                __delay_ms(2000);
                buzzer(OFF);
                __delay_ms(3000);
                close_door();
                count = 0;
            }
            else
            {   
                LCD_Clear();
                LCD_Write_String("Wrong Password");
                wrong_password_buzzer();
                count++;
            }
            if(count >= 3)
            {   
                do
                {
                    warning_buzzer();
                } while(count);
            }
        }
    }
}

void __interrupt() ISR(void){ 
    if(INTF == 1){               // Check the interrupt flag bit for reset 
        count = 0;
        RB2 = OFF;    // Turn the buzzer off
        INTF = 0; 
    }
    if(TMR1IF == 1){               // Check the timer1 interrupt flag bit 
        icount++;
        if (icount < duty + 1) RB1 = 1;
        else if (icount < 95) RB1 = 0;
        else icount = 0;
        TMR1 = TMR1_base;
        TMR1IF = 0;
    } 
}