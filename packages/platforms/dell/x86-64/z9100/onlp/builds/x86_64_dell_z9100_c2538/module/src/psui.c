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

typedef struct psu_cfg {
  uint16_t addr;
  uint8_t presence_mask;
  uint16_t power_type_mask;
  uint8_t status_mask;
  uint8_t data;
} psu_cfg_t;

psu_cfg_t psus_cfg[] = {
    { },                                /* Not Uses */
    { 0x237, 0x01, 0x02, 0x08, 0x00 },  /* PSU 0 */
    { 0x270, 0x01, 0x02, 0x08, 0x00 }   /* PSU 1 */
};

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

#define VALID_ID(id) \
    ((0 <= id && id < sizeof(pinfo)/sizeof(pinfo[0])) ? 1: 0)

int
onlp_psui_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    int id  = ONLP_OID_ID_GET(oid);

    /* Check id is ok */
    if (!VALID_ID(id)) {
        return ONLP_STATUS_E_PARAM;
    }
    *hdr = pinfo[id].hdr; /* Set the onlp_oid_hdr_t */

    /* read the register from the CPLD */
    ONLP_IF_ERROR_RETURN(smbus_set_perms());
    ONLP_IF_ERROR_RETURN(smbus_read_byte(psus_cfg[id].addr, &psus_cfg[id].data));
    ONLP_IF_ERROR_RETURN(smbus_unset_perms());

    /* See if present and status ok */
    if ((psus_cfg[id].data&psus_cfg[id].presence_mask) != 0) {
      ONLP_OID_STATUS_FLAG_CLR(hdr, PRESENT);
      return ONLP_STATUS_OK;
    }
    ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);

    /* Status ok */
    if ((psus_cfg[id].data&psus_cfg[id].status_mask) == 0) {
      ONLP_OID_STATUS_FLAG_SET(hdr, FAILED);
    } else {
      ONLP_OID_STATUS_FLAG_CLR(hdr, FAILED);
    }

    return ONLP_STATUS_OK;
}

int
onlp_psui_info_get(onlp_oid_t oid, onlp_psu_info_t* info)
{
    int id  = ONLP_OID_ID_GET(oid);

    /* Check id is ok */
    if (!VALID_ID(id)) {
        return ONLP_STATUS_E_PARAM;
    }
    *info = pinfo[id]; /* Set the onlp_oid_hdr_t */

    /* Get hdr */
    ONLP_IF_ERROR_RETURN(onlp_psui_hdr_get(oid, &info->hdr));

    /* Get PSU type     */
    if ((psus_cfg[id].data&psus_cfg[id].power_type_mask) == 0) {
      info->caps = ONLP_PSU_CAPS_GET_TYPE;
      info->type = ONLP_PSU_TYPE_AC;
    } else {
      info->caps = ONLP_PSU_CAPS_GET_TYPE;
      info->type = ONLP_PSU_TYPE_DC48;
    }

    /* Can't get voltage, amps, power on Z9100 */

    return ONLP_STATUS_OK;
}
