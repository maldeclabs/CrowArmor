#pragma once

#include "crowarmor/datacrow.h"
#include <linux/ioctl.h>

#define MAJOR_NUM 0x101

#define IOCTL_READ_CROW _IOR(MAJOR_NUM, 0, struct crow *)
