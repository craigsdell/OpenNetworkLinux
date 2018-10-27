/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2018 Dell EMC.
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
 * Thermal Sensor Platform Implementation.
 *
 ***********************************************************/
#include <onlp/platformi/base.h>
#include "platform_lib.h"

enum onlp_thermal_id
{
    THERMAL_RESERVED = 0,
    THERMAL_0,
    THERMAL_1,
    THERMAL_2,
    THERMAL_3,
    THERMAL_4,
    THERMAL_5,
    THERMAL_6,
    THERMAL_7
};

typedef struct temp_address {
    uint16_t value_addr;
    uint16_t status_addr;
} temp_address_t;

/* Temperature sensor addresses map */
/* Note: update this table when onlp_thermal_id enum changes */
temp_address_t temp_addresses[] = {
    { 0x00,  0x00 },    /* THERMAL_RESERVED */
    { 0x14,  0xdc },    /* THERMAL_0 */
    { 0x16,  0xdd },    /* THERMAL_1 */
    { 0x18,  0xde },    /* THERMAL_2 */
    { 0x1a,  0xdf },    /* THERMAL_3 */
    { 0x1e,  0xe1 },    /* THERMAL_4 */
    { 0x24,  0xe4 },    /* THERMAL_5 */
    { 0x239, 0x00 },    /* THERMAL_6 */
    { 0x272, 0x00 }     /* THERMAL_7 */
};

#define VALID_THERMAL_ID(id) \
    ((0 <= id && id < sizeof(temp_addresses)/sizeof(temp_addresses[0])) ? 1: 0)

/* Static values */
static onlp_thermal_info_t thermal_info_table__[] = {
    { }, /* Not used */
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_0, "Chassis Thermal Sensor 0"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_1, "Chassis Thermal Sensor 1"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_2, "Chassis Thermal Sensor 2"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_3, "Chassis Thermal Sensor 3"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_4, "Chassis Thermal Sensor 4"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_4, "Chassis Thermal Sensor 5"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_4, "Chassis Thermal Sensor 6"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_4, "Chassis Thermal Sensor 7"),
};

int
get_temp_value(int id) {

  uint8_t high;
  uint8_t low;
  uint16_t value_addr = temp_addresses[id].value_addr;

  ONLP_IF_ERROR_RETURN(smbus_set_perms());
  ONLP_IF_ERROR_RETURN(smbus_read_byte(value_addr, &high));
  ONLP_IF_ERROR_RETURN(smbus_read_byte(value_addr+1, &low));
  ONLP_IF_ERROR_RETURN(smbus_unset_perms());

  return(((high<<8)|(low))*100);
}

int
onlp_thermali_info_get(onlp_oid_id_t id, onlp_thermal_info_t* info)
{
    int local_id = ONLP_OID_ID_GET(id);
    ONLP_OID_INFO_ASSIGN(id, thermal_info_table__, info);

    /* Check id is ok */
    if (!VALID_THERMAL_ID(local_id)) {
        AIM_LOG_ERROR("invalid thermal id %d\n", id);
        return ONLP_STATUS_E_INTERNAL;
    }
    info->mcelsius = get_temp_value(local_id);
    return ONLP_STATUS_OK;
}

int
onlp_thermali_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* hdr)
{
    *hdr = thermal_info_table__[id].hdr;
    return ONLP_STATUS_OK;
}
