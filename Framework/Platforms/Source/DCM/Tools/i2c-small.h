#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>

#ifndef I2C_FUNC_SMBUS_READ_BYTE_DATA
#include <linux/i2c.h>

// this is the bare minimum of I2C code required to build this thing without
// the extended i2c.h header from i2c-tools, copied straight from version 3.0.1.

static inline __s32
i2c_smbus_access(int file, char read_write, __u8 command,
int size, union i2c_smbus_data *data)
{
struct i2c_smbus_ioctl_data args;

args.read_write = read_write;
args.command = command;
args.size = size;
args.data = data;
return ioctl(file,I2C_SMBUS,&args);
}

static inline __s32
i2c_smbus_read_byte_data(int file, __u8 command)
{
union i2c_smbus_data data;
if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
I2C_SMBUS_BYTE_DATA,&data))
return -1;
else
return 0x0FF & data.byte;
}

static inline __s32
i2c_smbus_write_block_data(int file, __u8 command,
__u8 length, __u8 *values)
{
union i2c_smbus_data data;
int i;
if (length > 32)
length = 32;
for (i = 1; i <= length; i++)
data.block[i] = values[i-1];
data.block[0] = length;
return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,
I2C_SMBUS_BLOCK_DATA, &data);
}

#endif
