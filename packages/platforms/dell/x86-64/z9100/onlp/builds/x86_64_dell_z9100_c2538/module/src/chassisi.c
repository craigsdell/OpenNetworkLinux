/************************************************************
 * <bsn.cl fy=2018 v=onl>
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
 ************************************************************/
#include <onlp/platformi/chassisi.h>
#include "platform_lib.h"

int
onlp_chassisi_hdr_get(onlp_oid_id_t id, onlp_oid_hdr_t* hdr)
{
    int i;
    onlp_oid_t* e = hdr->coids;

    ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);
    ONLP_OID_STATUS_FLAG_SET(hdr, OPERATIONAL);

    /* 4 Thermal sensors on the chassis */
    for (i = 1; i <= NUM_OF_THERMAL_ON_MAIN_BOARD; i++)
    {
        *e++ = ONLP_THERMAL_ID_CREATE(i);
    }

    /* 5 LEDs on the chassis */
    for (i = 1; i <= NUM_OF_LED_ON_MAIN_BOARD; i++)
    {
        *e++ = ONLP_LED_ID_CREATE(i);
    }

    /* 2 PSUs on the chassis */
    for (i = 1; i <= NUM_OF_PSU_ON_MAIN_BOARD; i++)
    {
        *e++ = ONLP_PSU_ID_CREATE(i);
    }

    /* 4 Fans on the chassis */
    for (i = 1; i <= NUM_OF_FAN_ON_MAIN_BOARD; i++)
    {
        *e++ = ONLP_FAN_ID_CREATE(i);
    }

    /* 32 SFPs */
    for(i = 1; i <= 32; i++) {
        *e++ = ONLP_SFP_ID_CREATE(i);
    }
    return 0;
}

int
onlp_chassisi_info_get(onlp_oid_id_t id, onlp_chassis_info_t* info)
{
    return onlp_chassisi_hdr_get(id, &info->hdr);
}
