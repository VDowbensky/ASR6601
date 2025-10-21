#include "uc1601_interface.h"


void UC1601_interface_init(void)
{
  i2c0_init();
}

//*****************************************************************************
// \brief Send a data to the device.
// \param ucData The data to send.
// This function is to send a data to the device.
// \return None.
//*****************************************************************************
void UC1601DataWrite(uint8_t ucData)
{
  i2c0_write(UC1601_DATA_ADDR, &ucData, 1);
}

//*****************************************************************************
// \brief Send a command to the device.
// \param ucCmd The command to send.
// This function is to send a command to the device.
// \return None.
//*****************************************************************************
void UC1601CmdWrite(uint8_t ucCmd)
{
  i2c0_write(UC1601_CMD_ADDR, &ucCmd, 1);
}

//*****************************************************************************
// \brief Send a double-byte command to the device.
// \param ucCmd The command to send.
// \param ucData The data to send.
// This function is to send a double-byte command to the device.
// \return None.
//*****************************************************************************
void UC1601DoubleCmdWrite(uint8_t ucCmd, uint8_t ucData)
{
  uint8_t buf[2];

  buf[0] = ucCmd;
  buf[1] = ucData;
  i2c0_write(UC1601_CMD_ADDR, buf, 2);
}
