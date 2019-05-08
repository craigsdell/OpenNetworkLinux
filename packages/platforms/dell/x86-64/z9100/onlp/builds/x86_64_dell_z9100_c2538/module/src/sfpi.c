/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2014 Accton Technology Corporation.
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
#include <onlp/platformi/base.h>
#include "platform_lib.h"

#define NUM_OF_SFP_PORT 32

typedef struct port_map {
    onlp_oid_id_t id;
    struct presence {
      int     bus;
      uint8_t addr;
      uint8_t offset;
      uint8_t mask;
    } p;
} port_map_t;

static const port_map_t portmap[NUM_OF_SFP_PORT] = {
    { 42, { 14, 0x3e, 0x16, 0x01 } },     /* port 0 */
    { 43, { 14, 0x3e, 0x16, 0x02 } },     /* port 1 */
    { 44, { 14, 0x3e, 0x16, 0x04 } },     /* port 2 */
    { 45, { 14, 0x3e, 0x16, 0x08 } },     /* port 3 */
    { 46, { 14, 0x3e, 0x16, 0x10 } },     /* port 4 */
    { 47, { 14, 0x3e, 0x16, 0x20 } },     /* port 5 */
    { 48, { 14, 0x3e, 0x16, 0x40 } },     /* port 6 */
    { 49, { 14, 0x3e, 0x16, 0x80 } },     /* port 7 */
    { 34, { 14, 0x3e, 0x17, 0x01 } },     /* port 8 */
    { 35, { 14, 0x3e, 0x17, 0x02 } },     /* port 9 */
    { 36, { 14, 0x3e, 0x17, 0x04 } },     /* port 10 */
    { 37, { 14, 0x3e, 0x17, 0x08 } },     /* port 11 */
    { 39, { 15, 0x3e, 0x16, 0x01 } },     /* port 12 */
    { 38, { 15, 0x3e, 0x16, 0x02 } },     /* port 13 */
    { 41, { 15, 0x3e, 0x16, 0x04 } },     /* port 14 */
    { 40, { 15, 0x3e, 0x16, 0x08 } },     /* port 15 */
    { 26, { 15, 0x3e, 0x16, 0x10 } },     /* port 16 */
    { 27, { 15, 0x3e, 0x16, 0x20 } },     /* port 17 */
    { 28, { 15, 0x3e, 0x16, 0x40 } },     /* port 18 */
    { 29, { 15, 0x3e, 0x16, 0x80 } },     /* port 19 */
    { 30, { 15, 0x3e, 0x17, 0x01 } },     /* port 20 */
    { 31, { 15, 0x3e, 0x17, 0x02 } },     /* port 21 */
    { 32, { 16, 0x3e, 0x16, 0x01 } },     /* port 22 */
    { 33, { 16, 0x3e, 0x16, 0x02 } },     /* port 23 */
    { 18, { 16, 0x3e, 0x16, 0x04 } },     /* port 24 */
    { 19, { 16, 0x3e, 0x16, 0x08 } },     /* port 25 */
    { 20, { 16, 0x3e, 0x16, 0x10 } },     /* port 26 */
    { 21, { 16, 0x3e, 0x16, 0x20 } },     /* port 27 */
    { 22, { 16, 0x3e, 0x16, 0x40 } },     /* port 28 */
    { 23, { 16, 0x3e, 0x16, 0x80 } },     /* port 29 */
    { 24, { 16, 0x3e, 0x17, 0x01 } },     /* port 30 */
    { 25, { 16, 0x3e, 0x17, 0x02 } }      /* port 31 */
};

#define VALID_ID(id) \
    ((0 <= id && id < sizeof(portmap)/sizeof(portmap[0])) ? 1: 0)

#define PORT_BUS_INDEX(port) (portmap[port].id)

int
onlp_sfpi_type_get(onlp_oid_id_t port, onlp_sfp_type_t* rtype)
{
    *rtype = ONLP_SFP_TYPE_QSFP28;
    return 0;
}

int
onlp_sfpi_bitmap_get(onlp_sfp_bitmap_t* bmap)
{
    /*
     * Ports {0, 32}
     */
    int p;
    AIM_BITMAP_CLR_ALL(bmap);

    for(p = 0; p < NUM_OF_SFP_PORT; p++) {
        AIM_BITMAP_SET(bmap, p);
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_is_present(onlp_oid_id_t port)
{
    /*
     * Return 1 if present.
     * Return 0 if not present.
     * Return < 0 if error.
     */
    uint8_t data;
    const port_map_t *pm;

    /* check port and then get portmap entry */
    if (!VALID_ID(port)) {
      return ONLP_STATUS_E_PARAM;
    }
    pm = &portmap[port];

    /* Grab presence byte */
    if (i2c_cpld_read(pm->p.bus, pm->p.addr, pm->p.offset,
                      1, &data, ONLP_I2C_F_FORCE) < 0) {
        AIM_LOG_ERROR("Unable to read present status from port(%d)\r\n", port);
        return ONLP_STATUS_E_INTERNAL;
    }

    /* Check against mask - inverted */
    return (((pm->p.mask & data) == 0) ? 1: 0);
}

int
onlp_sfpi_presence_bitmap_get(onlp_sfp_bitmap_t* dst)
{
    int i = 0;
    uint32_t presence_all = 0;
    uint8_t data[2];

    /* grab cpld4 presence bits */
    if (i2c_cpld_read(16, 0x3e, 0x16, 2, &data[0], ONLP_I2C_F_FORCE) < 0) {
        AIM_LOG_ERROR("Unable to read present status from cpld2\n");
        return ONLP_STATUS_E_INTERNAL;
    }
    presence_all |= data[1]&0x03;
    presence_all <<= 8;
    presence_all |= data[0];

    /* Grab cpld3 presence bits */
    if (i2c_cpld_read(15, 0x3e, 0x16, 2, &data[0], ONLP_I2C_F_FORCE) < 0) {
        AIM_LOG_ERROR("Unable to read present status from cpld3\n");
        return ONLP_STATUS_E_INTERNAL;
    }
    presence_all <<= 2;
    presence_all |= data[1]&0x03;
    presence_all <<= 8;
    presence_all |= data[0];

    /* cpld2 */
    if (i2c_cpld_read(14, 0x3e, 0x16, 2, &data[0], ONLP_I2C_F_FORCE) < 0) {
        AIM_LOG_ERROR("Unable to read present status from cpld4\n");
        return ONLP_STATUS_E_INTERNAL;
    }
    presence_all <<= 4;
    presence_all |= data[1]&0x0f;
    presence_all <<= 8;
    presence_all |= data[0];

    /* Populate bitmap */
    for(i = 0; presence_all; i++) {
        AIM_BITMAP_MOD(dst, i, ((presence_all&1) ? 0: 1));
        presence_all >>= 1;
    }

    return ONLP_STATUS_OK;
}

int
onlp_sfpi_dev_read(onlp_oid_id_t port, int devaddr, int addr,
                   uint8_t* dst, int size)
{
    int bus = PORT_BUS_INDEX(port);
    return onlp_i2c_block_read(bus, devaddr, addr, size, dst, ONLP_I2C_F_FORCE);
}

int
onlp_sfpi_dev_readb(onlp_oid_id_t port, int devaddr, int addr)
{
    int bus = PORT_BUS_INDEX(port);
    return onlp_i2c_readb(bus, devaddr, addr, ONLP_I2C_F_FORCE);
}

int
onlp_sfpi_dev_writeb(onlp_oid_id_t port, int devaddr, int addr, uint8_t value)
{
    int bus = PORT_BUS_INDEX(port);
    return onlp_i2c_writeb(bus, devaddr, addr, value, ONLP_I2C_F_FORCE);
}

int
onlp_sfpi_dev_readw(onlp_oid_id_t port, int devaddr, int addr)
{
    int bus = PORT_BUS_INDEX(port);
    return onlp_i2c_readw(bus, devaddr, addr, ONLP_I2C_F_FORCE);
}

int
onlp_sfpi_dev_writew(onlp_oid_id_t port, int devaddr, int addr, uint16_t value)
{
    int bus = PORT_BUS_INDEX(port);
    return onlp_i2c_writew(bus, devaddr, addr, value, ONLP_I2C_F_FORCE);
}
