/************************************************************
 * <bsn.cl fy=2018 v=onl>
 *
 *           Copyright 2018 Big Switch Networks, Inc.
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
 ************************************************************/
#include <onlp/platformi/attributei.h>
#include <onlp/stdattrs.h>
#include <onlplib/file.h>
#include <onlplib/i2c.h>

#include "platform_lib.h"

int
onlp_attributei_onie_info_get(onlp_oid_t oid, onlp_onie_info_t* rp)
{
    if(oid != ONLP_OID_CHASSIS) {
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    if(rp == NULL) {
        return 1;
    }

    /* Grab the onie sys eeprom */
    uint8_t data[MAX_SYS_EEPROM_SIZE+1];
    int dlen = MAX_SYS_EEPROM_SIZE;
    ONLP_IF_ERROR_RETURN(onlp_i2c_read(SYS_EEPROM_BUS, SYS_EEPROM_ADDR,
        SYS_EEPROM_OFFSET, dlen, data, ONLP_I2C_F_FORCE));

    return onlp_onie_decode(rp, data, dlen);
}

int
onlp_attributei_asset_info_get(onlp_oid_t oid, onlp_asset_info_t* rp)
{
    if(oid != ONLP_OID_CHASSIS) {
        return ONLP_STATUS_E_UNSUPPORTED;
    }

    if(rp == NULL) {
        return 1;
    }

    int rv;
    uint8_t v[5] = {0};

    rp->oid = oid;
    ONLP_IF_ERROR_RETURN(smbus_set_perms());
    if(ONLP_SUCCESS(rv = smbus_read_byte(SMBUS_FIRMWARE_ADDR, v)) &&
       ONLP_SUCCESS(rv = smbus_read_byte(CPLD1_ADDR, v+1)) &&
       ONLP_SUCCESS(rv = i2c_cpld_read(CPLD2_BUS, CPLD_ADDR, CPLD_VER_OFFSET, 1, v+2, ONLP_I2C_F_FORCE)) &&
       ONLP_SUCCESS(rv = i2c_cpld_read(CPLD3_BUS, CPLD_ADDR, CPLD_VER_OFFSET, 1, v+3, ONLP_I2C_F_FORCE)) &&
       ONLP_SUCCESS(rv = i2c_cpld_read(CPLD4_BUS, CPLD_ADDR, CPLD_VER_OFFSET, 1, v+4, ONLP_I2C_F_FORCE))) {

        rp->firmware_revision =
            aim_fstrdup("%2.2x.%2.2x.%2.2x.%2.2x.%2.2x", v[0], v[1], v[2], v[3], v[4]);
    }
    ONLP_IF_ERROR_RETURN(smbus_unset_perms());
    return rv;
}
