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
#include <onlp/platformi/base.h>
#include "platform_lib.h"

#define PSU_PRESENCE_MASK     0x01
#define PSU_POWER_TYPE_MASK   0x02
#define PSU_STATUS_MASK       0x08

#define PSU_FAN_AIRFLOW_MASK  0x01
#define PSU_FAN_STATUS_MASK   0x02
#define PSU_FAN_PRESENCE_MASK 0x04

/* SF IO Bus addresses */
#define PSU0_STATUS_ADDR     0x237 /* psu0 presence, pwr-type, status */
#define PSU0_FAN_STATUS_ADDR 0x23d /* psu0 fan air flow, status, presence */
#define PSU1_STATUS_ADDR     0x270 /* psu1 presence, pwr-type, status */
#define PSU1_FAN_STATUS_ADDR 0x276 /* psu1 fan air flow, status, presence */

/*
 * Get all information about the given PSU oid.
 */
static onlp_psu_info_t pinfo[] =
{
    { }, /* Not used */
    {
        { ONLP_PSU_ID_CREATE(PSU1_ID), "PSU-1", ONLP_OID_CHASSIS },
    },
    {
        { ONLP_PSU_ID_CREATE(PSU2_ID), "PSU-2", ONLP_OID_CHASSIS },
    }
};

int
onlp_psui_info_get(onlp_oid_t id, onlp_psu_info_t* info)
{
    int ret    = ONLP_STATUS_OK;
    int index  = ONLP_OID_ID_GET(id);
    uint16_t addr   = 0;
    uint8_t data = 0;

    *info = pinfo[index]; /* Set the onlp_oid_hdr_t */

    /* Get the present state */
    if (PSU1_ID == id) {
      addr = PSU0_STATUS_ADDR;
    } else {
      addr = PSU1_STATUS_ADDR;
    }

    /* read the register from the CPLD */
    ONLP_IF_ERROR_RETURN(smbus_set_perms());
    ONLP_IF_ERROR_RETURN(smbus_read_byte(addr, &data));
    ONLP_IF_ERROR_RETURN(smbus_unset_perms());

    /* See if present and status ok */
    if ((data&PSU_PRESENCE_MASK) != 0) {
      ONLP_OID_STATUS_FLAG_CLR(info, PRESENT);
      return ONLP_STATUS_OK;
    }
    ONLP_OID_STATUS_FLAG_SET(info, PRESENT);

    /* Status ok */
    if ((data&PSU_STATUS_MASK) == 0) {
      ONLP_OID_STATUS_FLAG_SET(info, FAILED);
    }

    /* Get PSU type     */
    if ((data&PSU_POWER_TYPE_MASK) == 0) {
      info->caps = ONLP_PSU_CAPS_GET_TYPE;
      info->type = ONLP_PSU_TYPE_AC;
    } else {
      info->caps = ONLP_PSU_CAPS_GET_TYPE;
      info->type = ONLP_PSU_TYPE_DC48;
    }

    /* Can't get voltage, amps, power on Z9100 */

    return ret;
}
