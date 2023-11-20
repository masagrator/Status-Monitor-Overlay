/*
Functions taken from Switch-OC-Suite source code made by KazushiMe
Original repository link (Deleted, last checked 15.04.2023): https://github.com/KazushiMe/Switch-OC-Suite
*/

#include "max17050.h"

constexpr float max17050SenseResistor = MAX17050_BOARD_SNS_RESISTOR_UOHM / 1000; // in uOhm
constexpr float max17050CGain = 1.99993;

Result I2cReadRegHandler(u8 reg, I2cDevice dev, u16 *out)
{
	struct readReg {
        u8 send;
        u8 sendLength;
        u8 sendData;
        u8 receive;
        u8 receiveLength;
    };

	I2cSession _session;

	Result res = i2cOpenSession(&_session, dev);
	if (res)
		return res;

	u16 val;

    struct readReg readRegister = {
        .send = 0 | (I2cTransactionOption_Start << 6),
        .sendLength = sizeof(reg),
        .sendData = reg,
        .receive = 1 | (I2cTransactionOption_All << 6),
        .receiveLength = sizeof(val),
    };

	res = i2csessionExecuteCommandList(&_session, &val, sizeof(val), &readRegister, sizeof(readRegister));
	if (res)
	{
		i2csessionClose(&_session);
		return res;
	}

	*out = val;
	i2csessionClose(&_session);
	return 0;
}

Result Max17050ReadReg(u8 reg, u16 *out)
{
	u16 data = 0;
	Result res = I2cReadRegHandler(reg, I2cDevice_Max17050, &data);

	if (R_FAILED(res))
	{
		return res;
	}

	*out = data;
	return res;
}
