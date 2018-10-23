/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2018 Dell EMC Inc.
 *
 * Licensed under the Eclipse Public License, Version 1.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *        http://www.eclipse.org/legal/epl-v10.html
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the
 * License.
 *
 * </bsn.cl>
 ************************************************************
 *
 *
 *
 ***********************************************************/
#ifndef __PLATFORM_LIB_H__
#define __PLATFORM_LIB_H__

#include "x86_64_dell_z9100_c2538_log.h"

#define NUM_OF_THERMAL_ON_MAIN_BOARD  8
#define NUM_OF_LED_ON_MAIN_BOARD      3
#define NUM_OF_PSU_ON_MAIN_BOARD      2
#define NUM_OF_FAN_ON_MAIN_BOARD      10
#define MAX_SYS_EEPROM_SIZE           256

#define SMBUS_FIRMWARE_ADDR           0x200
#define CPLD1_ADDR                    0x100
#define CPLD2_BUS                     14
#define CPLD3_BUS                     15
#define CPLD4_BUS                     16
#define CPLD_ADDR                     0x3e
#define CPLD_VER_OFFSET               0x00
#define SYS_EEPROM_BUS                2
#define SYS_EEPROM_ADDR               0x50
#define SYS_EEPROM_OFFSET             0x00

/* SF_IO_BUS info */
#define SF_BUS_RAM_ADDR_HIGH   0x210
#define SF_BUS_RAM_ADDR_LOW    0x211
#define SF_BUS_READ_DATA_ADDR  0x212
#define SF_BUS_WRITE_DATA_ADDR 0x213

int smbus_set_perms();
int smbus_unset_perms();
int smbus_read_byte(uint16_t addr, uint8_t *data);
int smbus_write_byte(uint16_t addr, uint8_t data);

#define PSU1_ID   1
#define PSU2_ID   2

#endif  /* __PLATFORM_LIB_H__ */
