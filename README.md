# RL78_F24_IICA0_I2C_Master
 RL78_F24_IICA0_I2C_Master

1. initial F24 EVB , to test below function 

- UART : P03 , P04

- BUTTON : P137 

- IICA0 MASTER (400K) : SCLA0/P62 ,  SDSA0/P63

	- use RL78_F13_IICA0_I2C_Slave , for I2C slave

- use P41 for GPIO output to sync with slave if necessary, check define : ENALBE_SPECIFIC_FUNCTION

- press button in EVB , will SEND I2C data (address 7BIT:0x50 , 8BIT:0xA0 , check IICA0_send_seq_cmd

	- write reg : 0x83 , below is cmd format

	- cmd 0 : 50h,83h,00h,crc
	
	- cmd 1~5 : 50h,83h,01h,data x 7,crc
	
	- cmd 6~10: 50h,83h,02h,data x 14,crc

- enable I2C read function if necessary , check define : ENABLE_I2C_RD

	- read reg : 0x00 , below is cmd format
	
	- slave will return : 00h,00h,5Ah,A5h,counter,00h,crc

2. below is LA capture 

WR command 0 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_0_cmd_0.jpg)

RD command

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_RD_cmd.jpg)

WR command 1 , send 01 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_1_cmd_01.jpg)


WR command 1 , send 11 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_1_cmd_11.jpg)


WR command 1 , send 21 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_1_cmd_21.jpg)



![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_RD_cmd.jpg)

WR command 2 , send 01 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_2_cmd_01.jpg)


WR command 2 , send 11 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_2_cmd_11.jpg)


WR command 12 , send 21 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/LA_WR_2_cmd_21.jpg)

3. below is slave side , receive log capture 

![image](https://github.com/released/RL78_F24_IICA0_I2C_Master/blob/main/slave_rcv_I2C_log.jpg)


