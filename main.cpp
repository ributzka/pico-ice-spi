#include "ice_cram.h"
#include "ice_fpga.h"
#include "ice_led.h"

uint8_t bitstream[] = {
#include "bitstream.inc"
};


int main(void) {
    ice_led_init();
    ice_fpga_init(48/*MHz*/);
    ice_fpga_start();

    // Write the whole bitstream to the FPGA CRAM
    ice_cram_open();
    ice_cram_write(bitstream, sizeof(bitstream));
    ice_cram_close();

    while (1);
    return 0;
}
