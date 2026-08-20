#include <stdint.h>
#include <stdbool.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/hw/io.h>
#endif

#include <drvs/ps2/misc.h>
#include <drvs/ps2/ps2.h>

#include <misc/debug.h>
#include <misc/bitflags.h>

void ps2_init(void)
{
    // TODO: USB Init
    // TODO: Check if PS2 controller even exists
    ps2_writeCmd(PS2_DISABLE_FIRST_PORT);
    ps2_writeCmd(PS2_DISABLE_SECOND_PORT);
    
    while ((ps2_readStatus() & PS2_OUTPUT_BUFFER_STATUS) != PS2_BUFFER_EMPTY) ps2_readData();
    
    ps2_writeCmd(PS2_READ_CONTROLLER_CONFIG);
    
    ps2_controller_config_t data = ps2_readData();
    data &= ~(PS2_FIRST_PORT_INTERRUPT_CONFIG);
    data &= ~(PS2_SECOND_PORT_INTERRUPT_CONFIG);
    data &= ~(PS2_FIRST_PORT_TRANSLATION_CONFIG);
    
    ps2_writeCmd(PS2_WRITE_CONTROLLER_CONFIG);
    ps2_writeData(data);    

    ps2_writeCmd(PS2_TEST_CONTROLLER);
    data = ps2_readData();
    if (data != PS2_TEST_CONTROLLER_SUCCESS) return;

    ps2_writeCmd(PS2_ENABLE_SECOND_PORT);
    ps2_writeCmd(PS2_READ_CONTROLLER_CONFIG);
    data = ps2_readData();

    bool dual_channel = false;
    if ((data & (PS2_SECOND_PORT_CLOCK_CONFIG)) == 0) dual_channel = true;
    if (dual_channel)
    {
        ps2_writeCmd(PS2_DISABLE_SECOND_PORT);
        data &= ~(PS2_SECOND_PORT_INTERRUPT_CONFIG);
        data &= ~(PS2_SECOND_PORT_CLOCK_CONFIG);
        ps2_writeCmd(PS2_WRITE_CONTROLLER_CONFIG);
        ps2_writeData(data);    
    }

    ps2_writeCmd(PS2_TEST_FIRST_PORT);
    data = ps2_readData();
    if (data != PS2_TEST_PORT_SUCCESS) return;
    ps2_writeCmd(PS2_TEST_SECOND_PORT);
    data = ps2_readData();
    if (data != PS2_TEST_PORT_SUCCESS) return;

    ps2_writeCmd(PS2_READ_CONTROLLER_CONFIG);
    data = ps2_readData();
    data = 0;
    data |= 0b00000100;

    data |= (PS2_FIRST_PORT_INTERRUPT_CONFIG);
    data |= (PS2_SECOND_PORT_INTERRUPT_CONFIG);

    ps2_writeCmd(PS2_WRITE_CONTROLLER_CONFIG);
    ps2_writeData(data);    

    ps2_writeCmd(PS2_ENABLE_FIRST_PORT);
    ps2_writeCmd(PS2_ENABLE_SECOND_PORT);
    
    ps2_writeData(0xFF);
    data = ps2_readData();

    info("Initalized!");
}

void detect_ps2_device_type()
{
    ps2_writeData(0xF5);
    int v = ps2_readData();
    if (v != PS2_RES_ACK) return;
    ps2_writeData(0xF2);
    v = ps2_readData();
    if (v != PS2_RES_ACK) return;

    int first_byte = ps2_readData();
    int second_byte = ps2_readData();
    debug("f: %x; s: %x", first_byte, second_byte);
        
    ps2_writeData(0xF4);
}
