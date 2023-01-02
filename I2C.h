/**
 * @file I2C.h
 *
 * @author Kibum Kim
 *
 * @brief This header file provides information about the functions in I2C.c.
**/
#define SDA 12       //Port P12, original: 29
#define SCL 11       //Port P11, original: 28

#define EEP_BASE_ADD 0xA0    //EEPROM address
#define PAGE_0 0    //0xA0
#define PAGE_1 2    //0xA2
#define PAGE_2 4    //0xA4


#define COUNTER_VAL 100
#define DELAY_VAL 10
#define TRUE 1
#define FALSE 0

#define WRITE_PAGE_SIZE 16
#define READ_PAGE_SIZE 256
#define HIGH  1
#define LOW   0
#define WRITE 0   //active low
#define READ  1   //active high
#define OUTA_pins 32    //pins P31 to P0, set at 31 for for loop calculations


/**
 * @brief  initializes structure object I2C_EEPROM
 * @member scl_gpio gpio number for SCL
 * @member sda_gpio gpio number for SDA
 * @member dev_add  address of EEPROM in use
 * @member page     page offset
 * example: scl_gpio is 11, sda_gpio is 12, EEPROM device address is 0xA0, page is 2, which indicates the address 0xA2
 * scl_gpio = 11;
 * sda_gpio = 12;
 * dev_add  = 0xA0;
 * page     = 2;
**/
struct I2C_EEPROM {   //creates structure so that code is more simplified(combines scl,sda, and dev_add into one variable)
  int scl_gpio;
  int sda_gpio;
  unsigned int dev_add;
  int page;
};



/**
 * @brief   write a byte to a specific device address
 *
 * @details The function addresses the EEPROM that is specified. Then, at the memory address specified,
 * the master writes the data using the I2C protocol. 
 *
 * @param i2c_eep  I2C_EEPROM struct object
 * @param byte_add 8-bit data word address
 * @param data_in  8-bit data word to write
 *
 * @returns 1 or 0 (true or false), indicates function success or failure, respectively.
**/
int i2c_writebyte (struct I2C_EEPROM i2c_eep, int byte_add, int data_in);



/**
 * @brief   write more than a byte to a specific device address
 *
 * @details The function starts at the given byte address and writes a maximum of 16 bytes to the designated EERPROM address. 
 *          The maximum page write differs by EEPROM type. 1K/2K EERPROM can only do up to an 8-byte page write while the 4K,
 *          8K and 16K are capable of 16-byte page writes.
 *
 * @param i2c_eep  I2C_EEPROM struct object
 * @param byte_add 8-bit data word address, starting address of page write
 * @param data_in  address of data words to write
 * @param count    number of bytes to page write
 *
 * @returns 1 or 0 (true or false), indicates function success or failure, respectively.
**/
int i2c_writepage (struct I2C_EEPROM i2c_eep, int byte_add, int *data_in, int count);



/**
 * @brief   reads a byte of a specific device address
 *
 * @details The function addresses the EEPROM that is specified. Then, at the memory address specified,
 *          the master read the data at the indicated byte address and stores it in its own address
 *
 * @param i2c_eep  I2C_EEPROM struct object
 * @param byte_add 8-bit data word address
 * @param data_out address of 8-bit data word to store data read
 *
 * @returns 1 or 0 (true or false), indicates function success or failure, respectively.
**/
int i2c_readbyte (struct I2C_EEPROM i2c_eep, int byte_add, int *data_out);



/**
 * @brief   reads continuous bytes of information of a specific device address
 *
 * @details The function addresses the EEPROM that is specified. Then, at the memory address specified,
 *          the master reads bytes of data at the indicated byte address and stores it in its own address. The number of bytes
 *          read is determined by the count parameter.
 *
 * @param i2c_eep  I2C_EEPROM struct object
 * @param byte_add 8-bit data word address
 * @param data_out address of 8-bit data word to store data read
 * @param count    number of bytes to be read
 *
 * @returns 1 or 0 (true or false), indicates function success or failure, respectively.
**/
int i2c_readpage (struct I2C_EEPROM i2c_eep, int byte_add, int *data_out, int count);



/**
 * @brief   initializes both gpio number of SCL and SDA
 *
 * @details Makes sure that both SCL and SDA gpio is set to high by the master. Created for use at the beginning of the code
 *          (before any i2c communication). Prevents miscommunication after reset.
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 *
 * @returns true, or 1.
**/
int init_gpio (int scl_gpio, int sda_gpio);



/**
 * @brief   checks if SDA gpio is high before any i2c communication
 *
 * @details The function makes sure that the i2c bus is not being controlled by the EEPROM before 
 *          any new i2c protocols are given. If EEPROM is controlling bus, the function will produce
 *          a stop signal to indicate to the EEPROM that the communication is finished.
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 *
 * @returns true, or 1.
**/
int init_i2c (int scl_gpio, int sda_gpio);



/**
 * @brief   creates a stop signal for i2c communication
 *
 * @details The function makes sure both SCL and SDA are low before creating the stop signal. After checking,
 *          it drives SCL high before driving SDA high, which is recognized by the EEPROM as a communication stop.
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 *
 * @returns true(1) if SDA is high, false(0) if SDA is stuck to low.
**/
int stop_signal (int scl_gpio, int sda_gpio);



/**
 * @brief   creates a start signal for i2c communication
 *
 * @details The function makes sure both SCL and SDA are high before creating the start signal. After checking,
 *          it drives SDA low before driving SCL low, which is recognized by the EEPROM as a communication start.
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 *
 * @returns true, or 1.
**/
int start_signal (int scl_gpio, int sda_gpio); 



/**
 * @brief   Allows the control of the specific gpio number for SDA
 *
 * @details The function drives only the gpio number of SDA high or low, depending on the value of state.
 *
 * @param sda_gpio gpio number of SDA
 * @param state    high(1) or low(0)
 *
 * @returns true, or 1.
**/
int sda_control (int sda_gpio, int state);  



/**
 * @brief   Allows the control of the specific gpio number for SCL
 *
 * @details The function drives only the gpio number of SCL high or low, depending on the value of state.
 *
 * @param scl_gpio gpio number of SCL
 * @param state    high(1) or low(0)
 *
 * @returns true, or 1.
**/
int scl_control (int scl_gpio, int state);  //scl_gpio = pin number for SCL; state = driving high or low (0 or 1)



/**
 * @brief   Checks for an acknowledge return from EERPROM
 *
 * @details The function is used for instances during the i2c communication where an acknowledge needs to be given by
 *          the EERPOM. It creates an instance of the clock(low-high-low) and checks for an acknowledge signal during that time.
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 * @param counter  timeout for acknowledge check(condition inside a while loop)
 *
 * @returns true(1) if ack is received and false(0) if ack is not received.
**/
int return_ack (int scl_gpio, int sda_gpio, int counter);



/**
 * @brief   Checks if no acknowledge return was given from EERPROM
 *
 * @details The function is used for instances during the i2c communication where a no acknowledge signal needs to be given by
 *          the EERPOM for a communication stop.
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 * @param counter  timeout for acknowledge check(condition inside a while loop)
 *
 * @returns true(1) if nack(no ack) is received and false(0) if nack is not received.
**/
int return_nack (int scl_gpio, int sda_gpio, int counter);



/**
 * @brief  Creates the i2c signal for the device address 
 *
 * @details The function creates clock while controling the SDA based on the device address given
 *          to the function in the form of hexadecimal.
 *
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 * @param hex_num  address of the EEPROM device in hexadecimal
 * @param rw       read/write, has a value of either 0(write protocol) or 1(read protocol)
 *
 * @returns true, or 1.
**/
int dev_sel_i2c (int scl_gpio, int sda_gpio, int hex_num, int rw);  //rw = read/write status. 0 is write, 1 is read



/**
 * @brief  Creates the i2c signal for the byte address 
 *
 * @details The function creates clock movement while controling the SDA based on the byte address given
 *          to the function in the form of hexadecimal. 
 *
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 * @param hex_num  8-bit data word address in hexadecimal
 *
 * @returns true, or 1.
**/
int byte_add_i2c (int scl_gpio, int sda_gpio, int hex_num);



/**
 * @brief  Creates the i2c signal for 8-bit data word(to be used in write operations)
 *
 * @details The function creates clock movement while controling the SDA based on the 8-bit data word given
 *          to the function in the form of hexadecimal. 
 *
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 * @param hex_num  8-bit data word
 *
 * @returns true, or 1.
**/
int data_in_i2c (int scl_gpio, int sda_gpio, int hex_num);     



/**
 * @brief  reads the data sent by the EEPROM during read operations and stores it.
 *
 * @details The function creates clock movement for the 8-bit data word to be sent out by the EEPROM to the master. 
 *          The 8 bits received are then stored in a variable and returned.
 *
 *
 * @param scl_gpio gpio number of SCL
 * @param sda_gpio gpio number of SDA
 *
 * @returns 8-bit data word received 
**/
int read_data_out (int scl_gpio, int sda_gpio);



