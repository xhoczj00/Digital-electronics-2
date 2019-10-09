#include "gpio.h"


/*functions*/
void GPIO_config_output(volatile  uint8_t *reg_name , uint8_t  pin_num)                 //configures reg_name as output in pin_num position 
{
    *reg_name |= _BV(pin_num);
}

void GPIO_config_input_nopull(volatile uint8_t *ddr_reg_name, volatile uint8_t *port_reg_name, uint8_t pin_num)
{
    *ddr_reg_name &= ~_BV(pin_num);
    *port_reg_name &= ~_BV(pin_num);
}

void GPIO_config_input_pullup(volatile uint8_t *ddr_reg_name, volatile uint8_t *port_reg_name, uint8_t pin_num)
{
    *ddr_reg_name &= ~_BV(pin_num);
    *port_reg_name |= _BV(pin_num);
}

void GPIO_write(volatile uint8_t *reg_name, uint8_t pin_num, uint8_t pin_val)
{
    if(pin_val == 0)
    {
        *reg_name &= ~_BV(pin_num);
    }
    else
    {
        *reg_name |= _BV(pin_num);
    }
    
}

void GPIO_toggle(volatile uint8_t *reg_name, uint8_t pin_num)
{
    *reg_name ^= _BV(pin_num);
}

uint8_t GPIO_read(volatile uint8_t *reg_name, uint8_t pin_num)
{
    // todo
    
    //if (bit_is_clear(PINB , 5)) {...} 
  
    //return *reg_name &= _BV(pin_num); 
}