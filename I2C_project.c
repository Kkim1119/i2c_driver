/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "simplei2c.h" 
#include <stdlib.h> 
#include <abdrive.h>

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


struct I2C_EEPROM {   //creates structure so that code is more simplified(combines scl,sda, and dev_add into one variable)
  int scl_gpio;
  int sda_gpio;
  unsigned int dev_add;
  int page;
};

fdserial *xbee;

i2c *st_eeprom;
int st_eeInitFlag;

int main()                                    // Main function
{
  int data;
  int read_page[READ_PAGE_SIZE] = {0};         //stores sequential random read values
  int write_page[WRITE_PAGE_SIZE] = {0};  
  struct I2C_EEPROM i2c_eep;    //creation of struct object)
  
  i2c_eep.scl_gpio  = SCL;    //values for i2c_eep
  i2c_eep.sda_gpio  = SDA;
  i2c_eep.dev_add   = EEP_BASE_ADD;
  i2c_eep.page      = PAGE_0;

  
  xbee = fdserial_open(9, 8, 0, 9600);      //open serial(for confirming results, testing, validation, etc)
  dprint(xbee, "Start\n");
  dprint(xbee, "I2C\n");
  
  init_gpio(SCL, SDA);    //make sure that SDA is high before anything(if low, there is a problem)

  /*                    
  for(int i=0; i<256; i++) {    //validation code(for readpage function)
    i2c_writebyte(i2c_eep,i,i);
  }
 
  for(int i=0; i<256; i++) {
    if(i%16 == 0){
      dprint(xbee, "\n");
    }
    i2c_readbyte(i2c_eep, i, &data);
    dprint(xbee, "%x ",data);  //print data read
  }
  */

  for(int i=0; i<WRITE_PAGE_SIZE; i++) {      //validation/example code for writepage function
    write_page[i] = i;
  }
  
  i2c_writepage(i2c_eep, 0x0, write_page, WRITE_PAGE_SIZE);
 
  i2c_readpage(i2c_eep, 0x0, read_page, READ_PAGE_SIZE);
  for(int i=0; i<READ_PAGE_SIZE; i++) {
    if(i%16 == 0){
      dprint(xbee, "\n");
    }
    dprint(xbee, "%x ",read_page[i]);
  }
}

int i2c_writebyte (struct I2C_EEPROM i2c_eep, int byte_add, int data_in)    //write a byte using i2c communication to a specific device(address)
{  
  
  init_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio);     //reset
  start_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio); //start
  dev_sel_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, i2c_eep.dev_add + i2c_eep.page, WRITE);    //device address(with 0 as last bit)
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio, COUNTER_VAL)){     //check if ack is received
    return FALSE;
  }     
  
  byte_add_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, byte_add);     //byte address(address we want to write in)
  data_in_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, data_in);   //data that we will write in the address
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  }   
  
  stop_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio);    //stop
  
  return TRUE;
}

int i2c_writepage (struct I2C_EEPROM i2c_eep, int byte_add, int *data_in, int count)
{
  init_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio);     //reset
  start_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio); //start
  dev_sel_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, i2c_eep.dev_add + i2c_eep.page, WRITE);    //device address(with 0 as last bit)
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio, COUNTER_VAL)){     //check if ack is received
    return FALSE;
  }     
  byte_add_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, byte_add);     //byte address(address we want to write in)
  
  //create for loop for continuous writing
  for(int i=0; i<count; i++) {
    data_in_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio,*(data_in+i));   //read data according to CLK movement, store in address of *data_out(local)
    if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
      return FALSE;
    }  
  }
  stop_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio);    //stop
  return TRUE;
}

int i2c_readbyte (struct I2C_EEPROM i2c_eep, int byte_add, int *data_out)
{
  
  init_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio);     //reset
  start_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio); //start
  dev_sel_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, i2c_eep.dev_add + i2c_eep.page, WRITE);    //device address(with 0 as last bit)
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  }     
  
  byte_add_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, byte_add);     //byte address(address we want to write in)
  
  start_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio); //start
  dev_sel_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, i2c_eep.dev_add + i2c_eep.page, READ);    //device address(with 0 as last bit)
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  } 
  
  *data_out = read_data_out(i2c_eep.scl_gpio, i2c_eep.sda_gpio);    //read data according to CLK movement, store in address of *data_out(local)
  
  if(!return_nack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  } 
  stop_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio);    //stop
  return TRUE;
}

int i2c_readpage (struct I2C_EEPROM i2c_eep, int byte_add, int *data_out, int count)    //sequential random read function
{
  
  
  init_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio);     //reset
  start_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio); //start
  dev_sel_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, i2c_eep.dev_add + i2c_eep.page, WRITE);    //device address(with 0 as last bit)
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  }     
  
  byte_add_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, byte_add);     //byte address(address we want to write in)
  
  start_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio); //start
  dev_sel_i2c(i2c_eep.scl_gpio, i2c_eep.sda_gpio, i2c_eep.dev_add + i2c_eep.page, READ);    //device address(with 0 as last bit)
  
  if(!return_ack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  } 
  for(int i=0; i<count; i++) {
    *(data_out+i) = read_data_out(i2c_eep.scl_gpio, i2c_eep.sda_gpio);    //read data according to CLK movement, store in address of *data_out(local)
    if(i == count - 1){
      sda_control(i2c_eep.sda_gpio, HIGH);
      if(!return_nack(i2c_eep.scl_gpio, i2c_eep.sda_gpio,COUNTER_VAL)){     //check if ack is received
        return FALSE;
      } 
      stop_signal(i2c_eep.scl_gpio, i2c_eep.sda_gpio);    //stop
    }
    else {
      sda_control(i2c_eep.sda_gpio, LOW);       //Give ack from master to slave(EEPROM)
      scl_control(i2c_eep.scl_gpio, HIGH);
      scl_control(i2c_eep.scl_gpio, LOW);
      sda_control(i2c_eep.sda_gpio, HIGH);       //set back to high for EEPROM 
    }
  }
  return TRUE;
}

int init_gpio (int scl_gpio, int sda_gpio)
{
  sda_control(sda_gpio, HIGH);
  scl_control(scl_gpio, HIGH);    //makes sure that both pins are high before creating start signal(not a stop signal)
  
  return TRUE;
}

int init_i2c (int scl_gpio, int sda_gpio)   //checks if SDA is high before anything happens. If low, stop process
{
  unsigned long mask;
  int counter;
  
  pause(DELAY_VAL);      // delay time for back-to-back WR operations
  counter = COUNTER_VAL;
  mask = 0x01 << sda_gpio;
  while((!(INA & mask))){
    if(counter == 0) {
      dprint(xbee, "Error! SDA stuck to low\n");
      return FALSE;
    }
    counter--;
  }
  stop_signal(scl_gpio, sda_gpio);
  return TRUE;
}

int stop_signal (int scl_gpio, int sda_gpio)    //function for creating stop signal for i2c communication(to shorten code)
{
  scl_control(scl_gpio, LOW);
  sda_control(sda_gpio, LOW);     //makes sure both pins are low before stop signal(not a start signal)
  
  scl_control(scl_gpio, HIGH);
  sda_control(sda_gpio, HIGH);     //stop(reset): SCL goes from low to high before SDA

  return TRUE;
}

int start_signal (int scl_gpio, int sda_gpio)   //creates start signal for i2c communication
{
  sda_control(sda_gpio, HIGH);
  scl_control(scl_gpio, HIGH);    //makes sure that both pins are high before creating start signal(not a stop signal)
  
  sda_control(sda_gpio, LOW);
  scl_control(scl_gpio, LOW);     //start: SDA goes from high to low before SCL  
  
  return TRUE;
}

#define OUTA_pins 32    //pins P31 to P0, set at 31 for for loop calculations

int sda_control (int sda_gpio, int state) {   //sda_gpio = pin number for SDA; state = driving high or low (0 or 1)

  unsigned long mask;
  unsigned long state_mask;
  
  mask = 0x01 << sda_gpio;
  state_mask = state << sda_gpio;
  
  // drive 0 = OUTA:0, DIRA:1 / drive 1 = OUTA:ignore, DIRA:0 
  OUTA = OUTA & (~mask);    //drive OUTA low
  DIRA = ((DIRA & (~mask)) | (mask & (~state_mask)));     // 1:OUTA, 0:input; switch direction to OUTA
 
  return TRUE;
}

int scl_control (int scl_gpio, int state) {   //scl_gpio = pin number for SCL; state = driving high or low (0 or 1)
  unsigned long mask;
  unsigned long state_mask;
  
  mask = 0x01 << scl_gpio;
  state_mask = state << scl_gpio;
  
  // drive 0 = OUTA:0, DIRA:1 / drive 1 = OUTA:ignore, DIRA:0 
  OUTA = OUTA & (~mask);    //drive OUTA low
  DIRA = ((DIRA & (~mask)) | (mask & (~state_mask)));     // 1:OUTA, 0:input; switch direction to OUTA
 
  return TRUE;
}

int return_ack (int scl_gpio, int sda_gpio, int counter)     //acknowledge check and creation of ack CLK
{
  unsigned long input;
  unsigned long mask;
  int sda_read;
  
  mask = (0x01 << sda_gpio);
  
  sda_control(sda_gpio, HIGH);
  scl_control(scl_gpio, HIGH);      //create clock for ack reception
 
  while(counter){
       input = INA;                 //check INA every count to see if SDA is changed
       sda_read = (input & mask);   //makes whatever pin SDA is linked to to be either 1 or 0
       if(sda_read == 0){           //if SDA was driven low by EEPROM(ack signal)
#ifdef DEBUG_ON       
          dprint(xbee, "counter: %d\n", counter);
#endif
          break;
       }
       counter = counter - 1;
  }
  if(counter){
#ifdef DEBUG_ON
      dprint(xbee, "Ack received: %x\n", sda_read);
#endif
      scl_control(scl_gpio, LOW);       //Before the function ends, put SCL low so ack can be received
      return TRUE;
  }
  else {
      dprint(xbee, "Error! no Ack: %x\n", sda_read);
      scl_control(scl_gpio, LOW);       //Before the function ends, put SCL low so ack can be received
      return FALSE;
  }
  
}

int return_nack (int scl_gpio, int sda_gpio, int counter)     //acknowledge check and creation of ack CLK
{
  unsigned long input;
  unsigned long mask;
  int sda_read;
  
  mask = (0x01 << sda_gpio);
  
  sda_control(sda_gpio, HIGH);
  scl_control(scl_gpio, HIGH);      //create clock for ack reception
 
  while(counter){
       input = INA;                 //check INA every count to see if SDA is changed
       sda_read = (input & mask);   //makes whatever pin SDA is linked to to be either 1 or 0
       if(sda_read == 0){           //if SDA was driven low by EEPROM(ack signal)
#ifdef DEBUG_ON       
          dprint(xbee, "counter: %d\n", counter);
#endif
          break;
       }
       counter = counter - 1;
  }
  if(!counter){
#ifdef DEBUG_ON
      dprint(xbee, "Nack received: %x\n", sda_read);
#endif
      scl_control(scl_gpio, LOW);       //Before the function ends, put SCL low so ack can be received
      return TRUE;
  }
  else {
      dprint(xbee, "Error! Ack received: %x\n", sda_read);
      scl_control(scl_gpio, LOW);       //Before the function ends, put SCL low so ack can be received
      return FALSE;
  }
  
}

int dev_sel_i2c (int scl_gpio, int sda_gpio, int hex_num, int rw)  //rw = read/write status. 0 is write, 1 is read
{  
  for(int i = 7; i>0; i--)
  {
    sda_control(sda_gpio, (hex_num >> i)&0x01);
    
    scl_control(scl_gpio, HIGH);
    scl_control(scl_gpio, LOW);
  } 
  
  sda_control(sda_gpio, rw);      //creates last bit for read/write manually
  scl_control(scl_gpio, HIGH);
  scl_control(scl_gpio, LOW);
  
  return TRUE;
}

int byte_add_i2c (int scl_gpio, int sda_gpio, int hex_num)
{
  for(int i = 7 ; i>=0; i--)
  {
    sda_control(sda_gpio, (hex_num >> i)&0x01);
    
    scl_control(scl_gpio, HIGH);
    scl_control(scl_gpio, LOW);
  } 
  if(!return_ack(scl_gpio, sda_gpio,COUNTER_VAL)){     //check if ack is received
    return FALSE;
  }  
  return TRUE;
}

int data_in_i2c (int scl_gpio, int sda_gpio, int hex_num)     //Used for DATA IN
{
  for(int i = 7; i>=0; i--)
  {
    sda_control(sda_gpio, (hex_num >> i)&0x01);
    
    scl_control(scl_gpio, HIGH);
    scl_control(scl_gpio, LOW);
  }
  
  return TRUE; 
}

int read_data_out (int scl_gpio, int sda_gpio)
{
  unsigned int data_out = 0;
  
  for(int i=7; i>=0; i--) {
    scl_control(scl_gpio, HIGH);
    data_out = ((data_out & (~i)) | (((INA >> sda_gpio)&0x01) << i));
    scl_control(scl_gpio, LOW);
  }
  return data_out;
}