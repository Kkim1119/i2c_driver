/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "simplei2c.h" 
#include <stdlib.h> 
#include <abdrive.h>
#include "I2C.h"

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

