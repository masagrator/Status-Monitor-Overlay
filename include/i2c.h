/*
Functions taken from Switch-OC-Suite source code made by KazushiMe
Original repository link (Deleted, last checked 15.04.2023): https://github.com/KazushiMe/Switch-OC-Suite
*/

#include "max17050.h"

constexpr float max17050SenseResistor = MAX17050_BOARD_SNS_RESISTOR_UOHM / 1000; // in uOhm
constexpr float max17050CGain = 1.99993;

Result I2cReadRegHandler(u8 reg, I2cDevice dev, u16 *out)
{
	// I2C Bus Communication Reference: https://www.ti.com/lit/an/slva704/slva704.pdf
	struct { u8 reg;  } __attribute__((packed)) cmd;
	struct { u16 val; } __attribute__((packed)) rec;

	I2cSession _session;

	Result res = i2cOpenSession(&_session, dev);
	if (res)
		return res;

	cmd.reg = reg;
	res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
	if (res)
	{
		i2csessionClose(&_session);
		return res;
	}

	res = i2csessionReceiveAuto(&_session, &rec, sizeof(rec), I2cTransactionOption_All);
	if (res)
	{
		i2csessionClose(&_session);
		return res;
	}

	*out = rec.val;
	i2csessionClose(&_session);
	return 0;
}

bool Max17050ReadReg(u8 reg, u16 *out)
{
	u16 data = 0;
	Result res = I2cReadRegHandler(reg, I2cDevice_Max17050, &data);

	if (res)
	{
		*out = res;
		return false;
	}

	*out = data;
	return true;
}