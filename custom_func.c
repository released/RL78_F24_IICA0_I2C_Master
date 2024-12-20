/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "r_smc_entry.h"
#include "platform.h"

#include "misc_config.h"
#include "custom_func.h"

#include "simple_i2c_driver.h"
#include "CRC8_SAE_J1850_ZERO.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TRIG_BTN1                       	    (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_TRIG_BTN2                 	        (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                    		    (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


#define FLAG_PROJ_TRIG_1                                (flag_PROJ_CTL.bit8)
#define FLAG_PROJ_TRIG_2                                (flag_PROJ_CTL.bit9)
#define FLAG_PROJ_TRIG_3                                (flag_PROJ_CTL.bit10)
#define FLAG_PROJ_TRIG_4                                (flag_PROJ_CTL.bit11)
#define FLAG_PROJ_TRIG_5                                (flag_PROJ_CTL.bit12)
#define FLAG_PROJ_REVERSE13                             (flag_PROJ_CTL.bit13)
#define FLAG_PROJ_REVERSE14                             (flag_PROJ_CTL.bit14)
#define FLAG_PROJ_REVERSE15                             (flag_PROJ_CTL.bit15)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned int counter_tick = 0;
volatile unsigned int btn_counter_tick = 0;

#define BTN_PRESSED_LONG                                (2500)

#define I2C_SLV_ADDR_7BIT                               (0x50)	// 7BIT
#define I2C_SLV_ADDR_8BIT                               (I2C_SLV_ADDR_7BIT << 1)    // 0x28

unsigned char tx_buffer[32] = {0};
unsigned char rx_buffer[32] = {0};
// const unsigned char device_addr = I2C_SLV_ADDR_7BIT;
const unsigned char device_addr = I2C_SLV_ADDR_8BIT;
const unsigned char Reserved = 0x83;

#define ENABLE_I2C_RD
#define ENABLE_I2C_CMD0
#define ENABLE_I2C_CMD1
#define ENABLE_I2C_CMD2

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
extern MD_STATUS IICA0_read(unsigned char device_addr,unsigned char reg_addr,unsigned char* rx_xfer_data,unsigned short rx_num);
extern MD_STATUS IICA0_write(unsigned char device_addr,unsigned char reg_addr,unsigned char* tx_xfer_data,unsigned short tx_num);

unsigned int btn_get_tick(void)
{
	return (btn_counter_tick);
}

void btn_set_tick(unsigned int t)
{
	btn_counter_tick = t;
}

void btn_tick_counter(void)
{
	btn_counter_tick++;
    if (btn_get_tick() >= 60000)
    {
        btn_set_tick(0);
    }
}

unsigned int get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned int t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

void delay_ms(unsigned int ms)
{
	#if 1
    unsigned int tickstart = get_tick();
    unsigned int wait = ms;
	unsigned int tmp = 0;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000 -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
	
	#else
	TIMER_Delay(TIMER0, 1000*ms);
	#endif
}


void IICA0_send_seq_cmd(void)
{
    unsigned char len = 0;
    unsigned char num_of_touch = 0;
    unsigned char CRC = 0x00;

    unsigned char i = 0l;
    unsigned char cnt = 0l;
    unsigned char delay_timing = 50;

    /**********************/

    #if defined (ENABLE_I2C_CMD0)
    num_of_touch = 0;
    len = 2 + 7*num_of_touch;    // cal CRC len
    tx_buffer[0] = 0x83;
    tx_buffer[1] = num_of_touch;

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    #endif

    /**********************/
    
    #if defined (ENABLE_I2C_CMD1)
    num_of_touch = 1;
    len = 2 + 7*num_of_touch;  // cal CRC len
    tx_buffer[0] = 0x83;
    tx_buffer[1] = num_of_touch;

    /***/
    cnt = 0;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x10;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x20;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
        
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x30;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x40;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    #endif

    /**********************/
    
    #if defined (ENABLE_I2C_CMD2)
    num_of_touch = 2;
    len = 2 + 7*num_of_touch;  // cal CRC len
    tx_buffer[0] = 0x83;
    tx_buffer[1] = num_of_touch;

    /***/
    cnt = 0;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x10;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
        
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x20;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x30;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    /***/
    cnt = 0x40;
    for(i = 2 ; i < len ; i++)
    {
        tx_buffer[i] = i-1 + cnt;
    }

    CRC = Crc8_saeJ1850Zero(&tx_buffer[0],len);
    tx_buffer[len] = CRC;

    IICA0_write(device_addr , Reserved , &tx_buffer[1] , len);
    delay_ms(delay_timing);
    
    #if defined (ENABLE_I2C_RD)
    IICA0_read(device_addr , 0x00 , &rx_buffer[0] ,7);
    delay_ms(delay_timing);
    #endif

    #endif
}


void Timer_1ms_IRQ(void)
{
    tick_counter();

    if ((get_tick() % 1000) == 0)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 1;
    }

    if ((get_tick() % 50) == 0)
    {

    }	

    Button_Process_long_counter();
}


/*
    F24 target board
    LED1 connected to P66, LED2 connected to P67
*/
void LED_Toggle(void)
{
    // PIN_WRITE(6,6) = ~PIN_READ(6,6);
    // PIN_WRITE(6,7) = ~PIN_READ(6,7);
    P6_bit.no6 = ~P6_bit.no6;
    P6_bit.no7 = ~P6_bit.no7;
}

void loop(void)
{
	static unsigned int LOG1 = 0;

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;

        printf("log(timer):%4d\r\n",LOG1++);
        LED_Toggle();             
    }

    Button_Process_in_polling();
}


// F24 EVB , P137/INTP0 , set both edge 
void Button_Process_long_counter(void)
{
    if (FLAG_PROJ_TRIG_BTN2)
    {
        btn_tick_counter();
    }
    else
    {
        btn_set_tick(0);
    }
}

void Button_Process_in_polling(void)
{
    static unsigned char cnt = 0;

    if (FLAG_PROJ_TRIG_BTN1)
    {
        FLAG_PROJ_TRIG_BTN1 = 0;
        printf("BTN pressed(%d)\r\n",cnt);
        IICA0_send_seq_cmd();

        if (cnt == 0)   //set both edge  , BTN pressed
        {
            FLAG_PROJ_TRIG_BTN2 = 1;
        }
        else if (cnt == 1)  //set both edge  , BTN released
        {
            FLAG_PROJ_TRIG_BTN2 = 0;
        }

        cnt = (cnt >= 1) ? (0) : (cnt+1) ;
    }

    if ((FLAG_PROJ_TRIG_BTN2 == 1) && 
        (btn_get_tick() > BTN_PRESSED_LONG))
    {         
        printf("BTN pressed LONG\r\n");
        btn_set_tick(0);
        FLAG_PROJ_TRIG_BTN2 = 0;
    }
}

// F24 EVB , P137/INTP0
void Button_Process_in_IRQ(void)    
{
    FLAG_PROJ_TRIG_BTN1 = 1;
}

void UARTx_Process(unsigned char rxbuf)
{    
    if (rxbuf > 0x7F)
    {
        printf("invalid command\r\n");
    }
    else
    {
        printf("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {
            case '1':
                FLAG_PROJ_TRIG_1 = 1;
                break;
            case '2':
                FLAG_PROJ_TRIG_2 = 1;
                break;
            case '3':
                FLAG_PROJ_TRIG_3 = 1;
                break;
            case '4':
                FLAG_PROJ_TRIG_4 = 1;
                break;
            case '5':
                FLAG_PROJ_TRIG_5 = 1;
                break;

            case 'X':
            case 'x':
                RL78_soft_reset(7);
                break;
            case 'Z':
            case 'z':
                RL78_soft_reset(1);
                break;
        }
    }
}

/*
    Reset Control Flag Register (RESF) 
    BIT7 : TRAP
    BIT6 : 0
    BIT5 : 0
    BIT4 : WDCLRF
    BIT3 : 0
    BIT2 : 0
    BIT1 : IAWRF
    BIT0 : LVIRF
*/
void check_reset_source(void)
{
    /*
        Internal reset request by execution of illegal instruction
        0  Internal reset request is not generated, or the RESF register is cleared. 
        1  Internal reset request is generated. 
    */
    uint8_t src = RESF;
    printf("Reset Source <0x%08X>\r\n", src);

    #if 1   //DEBUG , list reset source
    if (src & BIT0)
    {
        printf("0)voltage detector (LVD)\r\n");       
    }
    if (src & BIT1)
    {
        printf("1)illegal-memory access\r\n");       
    }
    if (src & BIT2)
    {
        printf("2)EMPTY\r\n");       
    }
    if (src & BIT3)
    {
        printf("3)EMPTY\r\n");       
    }
    if (src & BIT4)
    {
        printf("4)watchdog timer (WDT) or clock monitor\r\n");       
    }
    if (src & BIT5)
    {
        printf("5)EMPTY\r\n");       
    }
    if (src & BIT6)
    {
        printf("6)EMPTY\r\n");       
    }
    if (src & BIT7)
    {
        printf("7)execution of illegal instruction\r\n");       
    }
    #endif

}

/*
    7:Internal reset by execution of illegal instruction
    1:Internal reset by illegal-memory access
*/
//perform sofware reset
void _reset_by_illegal_instruction(void)
{
    static const unsigned char illegal_Instruction = 0xFF;
    void (*dummy) (void) = (void (*)(void))&illegal_Instruction;
    dummy();
}
void _reset_by_illegal_memory_access(void)
{
    #if 1
    const unsigned char ILLEGAL_ACCESS_ON = 0x80;
    IAWCTL |= ILLEGAL_ACCESS_ON;            // switch IAWEN on (default off)
    *(__far volatile char *)0x00000 = 0x00; //write illegal address 0x00000(RESET VECTOR)
    #else
    signed char __far* a;                   // Create a far-Pointer
    IAWCTL |= _80_CGC_ILLEGAL_ACCESS_ON;    // switch IAWEN on (default off)
    a = (signed char __far*) 0x0000;        // Point to 0x000000 (FLASH-ROM area)
    *a = 0;
    #endif
}

void RL78_soft_reset(unsigned char flag)
{
    switch(flag)
    {
        case 7: // do not use under debug mode
            _reset_by_illegal_instruction();        
            break;
        case 1:
            _reset_by_illegal_memory_access();
            break;
    }
}

// retarget printf
int __far putchar(int c)
{
    // F24 , UART0
    STMK0 = 1U;    /* disable INTST0 interrupt */
    SDR00L = (unsigned char)c;
    while(STIF0 == 0)
    {

    }
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    return c;
}

void hardware_init(void)
{
    // const unsigned char indicator[] = "hardware_init";
    BSP_EI();
    R_Config_UART0_Start();         // UART , P15 , P16
    R_Config_TAU0_1_Start();        // TIMER
    R_Config_INTC_INTP0_Start();    // BUTTON , P137 
    
    // check_reset_source();
    printf("%s finish\r\n\r\n",__func__);
}
