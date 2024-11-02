#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#define _BSD_SOURCE

#define GPIO_DIP_OFFSET 2
#define GPIO_LED_OFFSET 0 
#define DIPS_AND_LEDS_BASEADDR 0x41200000

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}


// Function to set LED colors based on switch input
void set_led_color(volatile unsigned int *dipsandleds_ptr, unsigned int switch_value)
{
    unsigned int led_value = 0;
    
    switch(switch_value)
    {
        case 0b000: // OFF
            led_value = 0;
            break;
        case 0b001: // Red
            led_value = (1 << 4);
            break;
        case 0b010: // Green
            led_value = (1 << 5);
            break;
        case 0b100: // Blue
            led_value = (1 << 6);
            break;
        case 0b011: // Red + Green 
            led_value = (1 << 4) | (1 << 5);
            break;
        case 0b101: // Red + Blue 
            led_value = (1 << 4) | (1 << 6);
            break;
        case 0b110: // Blue + Green 
            led_value = (1 << 5) | (1 << 6);
            break;
        case 0b111: // Red + Blue + Green 
            led_value = (1 << 4) | (1 << 5) | (1 << 6);
            break;
        default:
            led_value = 0;
            break;
    }
    
    *(dipsandleds_ptr + GPIO_LED_OFFSET) = led_value;
}

int main()
{

// first, get a pointer to the peripheral base address using /dev/mem and the function mmap
    volatile unsigned int *dipsandleds_ptr = get_a_pointer(DIPS_AND_LEDS_BASEADDR);	
    printf("\r\n\r\n\r\nLab 6 Example Program\n\r");
    while (1)
    {
		unsigned int switch_value = *(dipsandleds_ptr + GPIO_DIP_OFFSET) & 0x7;
	    printf("Current Switches = %d\r\n",*(dipsandleds_ptr+GPIO_DIP_OFFSET));
		set_led_color(dipsandleds_ptr, switch_value);
		usleep(500000); // Delay for a visible change
    }
    return 0;
}
