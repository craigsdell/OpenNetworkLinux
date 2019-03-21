/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2015 Accton Technology Corporation.
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
#include <onlp/platformi/psui.h>
#include <stdio.h>
#include <string.h>
#include "platform_lib.h"

#define PSU_STATUS_PRESENT    1
#define PSU_STATUS_POWER_GOOD 1

#define PSU_NODE_MAX_INT_LEN  8
#define PSU_NODE_MAX_PATH_LEN 64

static int
psu_status_info_get(int id, int is_ac, char *node, int *value)
{
    int ret = 0;
    char buf[PSU_NODE_MAX_INT_LEN + 1] = {0};
    char node_path[PSU_NODE_MAX_PATH_LEN] = {0};

    *value = 0;

    if (PSU1_ID == id) {
        sprintf(node_path, "%s%s", is_ac ? PSU1_AC_EEPROM_PREFIX : PSU1_DC_EEPROM_PREFIX, node);
    }
    else if (PSU2_ID == id) {
        sprintf(node_path, "%s%s", is_ac ? PSU2_AC_EEPROM_PREFIX : PSU2_DC_EEPROM_PREFIX, node);
    }

    ret = deviceNodeReadString(node_path, buf, sizeof(buf), 0);

    if (ret == 0) {
        *value = atoi(buf);
    }

    return ret;
}

static int
psu_cpr_4011_pmbus_info_get(int id, char *node, int *value)
{
    int  ret = 0;
    char buf[PSU_NODE_MAX_INT_LEN + 1]    = {0};
    char node_path[PSU_NODE_MAX_PATH_LEN] = {0};

    *value = 0;

    if (PSU1_ID == id) {
        sprintf(node_path, "%s%s", PSU1_AC_PMBUS_PREFIX, node);
    }
    else {
        sprintf(node_path, "%s%s", PSU2_AC_PMBUS_PREFIX, node);
    }

    ret = deviceNodeReadString(node_path, buf, sizeof(buf), 0);

    if (ret == 0) {
        *value = atoi(buf);
    }

    return ret;
}

int
onlp_psui_init(void)
{
    return ONLP_STATUS_OK;
}

static int
psu_cpr_4011_info_get(onlp_psu_info_t* info)
{
    int val   = 0;
    int index = ONLP_OID_ID_GET(info->hdr.id);

    /* Set capability
     */
    info->type = ONLP_PSU_TYPE_AC;

    if (ONLP_OID_FAILED(info)) {
        return ONLP_STATUS_OK;
    }

    /* Set the associated oid_table */
    info->hdr.coids[0] = ONLP_FAN_ID_CREATE(index + CHASSIS_FAN_COUNT);
    info->hdr.coids[1] = ONLP_THERMAL_ID_CREATE(index + CHASSIS_THERMAL_COUNT);

    /* Read voltage, current and power */
    if (psu_cpr_4011_pmbus_info_get(index, "psu_v_out", &val) == 0) {
        info->mvout = val;
        info->caps |= ONLP_PSU_CAPS_GET_VOUT;
    }

    if (psu_cpr_4011_pmbus_info_get(index, "psu_v_in", &val) == 0) {
        info->mvin = val;
        info->caps |= ONLP_PSU_CAPS_GET_VIN;
    }

    if (psu_cpr_4011_pmbus_info_get(index, "psu_i_out", &val) == 0) {
        info->miout = val;
        info->caps |= ONLP_PSU_CAPS_GET_IOUT;
    }

    if (psu_cpr_4011_pmbus_info_get(index, "psu_i_in", &val) == 0) {
        info->miin = val;
        info->caps |= ONLP_PSU_CAPS_GET_IIN;
    }

    if (psu_cpr_4011_pmbus_info_get(index, "psu_p_out", &val) == 0) {
        info->mpout = val;
        info->caps |= ONLP_PSU_CAPS_GET_POUT;
    }

    if (psu_cpr_4011_pmbus_info_get(index, "psu_p_in", &val) == 0) {
        info->mpin = val;
        info->caps |= ONLP_PSU_CAPS_GET_PIN;
    }

    return ONLP_STATUS_OK;
}

int
psu_um400d_info_get(onlp_psu_info_t* info)
{
    int index = ONLP_OID_ID_GET(info->hdr.id);

    /* Set capability
     */
    info->type = ONLP_PSU_TYPE_DC48;

    if (ONLP_OID_FAILED(info)) {
        return ONLP_STATUS_OK;
    }

    /* Set the associated oid_table */
    info->hdr.coids[0] = ONLP_FAN_ID_CREATE(index + CHASSIS_FAN_COUNT);

    return ONLP_STATUS_OK;
}

static int
psu_ym2401_info_get(onlp_psu_info_t* info)
{
    int val   = 0;
    int index = ONLP_OID_ID_GET(info->hdr.id);

    /* Set capability
     */
    info->type = ONLP_PSU_TYPE_AC;

    if (ONLP_OID_FAILED(info)) {
        return ONLP_STATUS_OK;
    }

    /* Set the associated oid_table */
    info->hdr.coids[0] = ONLP_FAN_ID_CREATE(index + CHASSIS_FAN_COUNT);
    info->hdr.coids[1] = ONLP_THERMAL_ID_CREATE(index + CHASSIS_THERMAL_COUNT);

    /* Read voltage, current and power */
    if (psu_ym2401_pmbus_info_get(index, "psu_v_out", &val) == 0) {
        info->mvout = val;
        info->caps |= ONLP_PSU_CAPS_GET_VOUT;
    }

    if (psu_ym2401_pmbus_info_get(index, "psu_i_out", &val) == 0) {
        info->miout = val;
        info->caps |= ONLP_PSU_CAPS_GET_IOUT;
    }

    if (psu_ym2401_pmbus_info_get(index, "psu_p_out", &val) == 0) {
        info->mpout = val;
        info->caps |= ONLP_PSU_CAPS_GET_POUT;
    }

    return ONLP_STATUS_OK;
}

/*
 * Get all information about the given PSU oid.
 */
static onlp_psu_info_t pinfo[] =
{
    { }, /* Not used */
    {
        { ONLP_PSU_ID_CREATE(PSU1_ID), "PSU-1", 0 },
    },
    {
        { ONLP_PSU_ID_CREATE(PSU2_ID), "PSU-2", 0 },
    }
};

int
onlp_psui_id_validate(onlp_oid_id_t id)
{
    return ONLP_OID_ID_VALIDATE_RANGE(id, 1, AIM_ARRAYSIZE(pinfo) - 1);
}

int
onlp_psui_info_get(onlp_oid_id_t id, onlp_psu_info_t* info)
{
    int val   = 0;
    int   ret = ONLP_STATUS_OK;
    int is_ac=1;
    psu_type_t psu_type;

    memset(info, 0, sizeof(onlp_psu_info_t));
    *info = pinfo[id]; /* Set the onlp_oid_hdr_t */

    /* Get the present state */
    if (psu_status_info_get(id, 1, "psu_present", &val) != 0) {
        printf("Unable to read PSU(%d) node(psu_present)\r\n", id);
    }

    if (val != PSU_STATUS_PRESENT) {
        ONLP_OID_STATUS_FLAG_CLR(info, PRESENT);
        return ONLP_STATUS_OK;
    }
    ONLP_OID_STATUS_FLAG_SET(info, PRESENT);


    /* Get power good status */
    if (psu_status_info_get(id, 1, "psu_power_good", &val) != 0) {
        printf("Unable to read PSU(%d) node(psu_power_good)\r\n", id);
    }

    if (val != PSU_STATUS_POWER_GOOD) {
        ONLP_OID_STATUS_FLAG_SET(info, FAILED);
        return 0;
    }


    /* Get PSU type
     */
    psu_type = get_psu_type(id, info->model, sizeof(info->model));

    switch (psu_type) {
        case PSU_TYPE_AC_COMPUWARE_F2B:
        case PSU_TYPE_AC_COMPUWARE_B2F:
            ret = psu_cpr_4011_info_get(info);
            break;
        case PSU_TYPE_AC_3YPOWER_F2B:
        case PSU_TYPE_AC_3YPOWER_B2F:
            ret = psu_ym2401_info_get(info);
            break;
        case PSU_TYPE_DC_48V_F2B:
        case PSU_TYPE_DC_48V_B2F:
            is_ac=0;
            ret = psu_um400d_info_get(info);
            break;
        default:
            ret = ONLP_STATUS_E_UNSUPPORTED;
            break;
    }
    psu_serial_number_get(id, is_ac, info->serial, sizeof(info->serial));

    return ret;
}
