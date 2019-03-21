/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2013 Accton Technology Corporation.
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

enum onlp_thermal_id {
    THERMAL_RESERVED = 0,
    THERMAL_CPU_CORE,
    THERMAL_1_ON_MAIN_BROAD,
    THERMAL_2_ON_MAIN_BROAD,
    THERMAL_3_ON_MAIN_BROAD,
    THERMAL_1_ON_PSU1,
    THERMAL_1_ON_PSU2,
};

static char* devfiles[] = { /* must map with onlp_thermal_id */
    "reserved",
    NULL,                  /* CPU_CORE files */
    "/sys/bus/i2c/devices/61-0048*temp1_input",
    "/sys/bus/i2c/devices/62-0049*temp1_input",
    "/sys/bus/i2c/devices/63-004a*temp1_input",
    "/sys/bus/i2c/devices/57-003c*psu_temp1_input",
    "/sys/bus/i2c/devices/58-003f*psu_temp1_input",
};

static char* cpu_coretemp_files[] =
    {
        "/sys/devices/platform/coretemp.0*temp2_input",
        "/sys/devices/platform/coretemp.0*temp3_input",
        "/sys/devices/platform/coretemp.0*temp4_input",
        "/sys/devices/platform/coretemp.0*temp5_input",
        NULL,
    };

static onlp_thermal_info_t thermal_info_table__[] = {
    { }, /* Not used */
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_CPU_CORE, "CPU Core"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_1_ON_MAIN_BROAD, "Chassis Thermal Sensor 1 (Front middle)"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_2_ON_MAIN_BROAD, "Chassis Thermal Sensor 2 (Rear right)"),
    ONLP_CHASSIS_THERMAL_INFO_ENTRY_INIT(THERMAL_3_ON_MAIN_BROAD, "Chassis Thermal Sensor 3 (Front right)"),
    ONLP_PSU_THERMAL_INFO_ENTRY_INIT(THERMAL_1_ON_PSU1, "PSU-1 Thermal Sensor 1", 1),
    ONLP_PSU_THERMAL_INFO_ENTRY_INIT(THERMAL_1_ON_PSU2, "PSU-2 Thermal Sensor 1", 2),
};

int
onlp_thermali_info_get(onlp_oid_t id, onlp_thermal_info_t* info)
{
    int psu_id;
    psu_type_t psu_type;

    ONLP_OID_INFO_ASSIGN(id, thermal_info_table__, info);

    switch(ONLP_OID_TYPE_GET(info->hdr.poid))
        {
        case ONLP_OID_TYPE_CHASSIS:
            {
                if(id == THERMAL_CPU_CORE) {
                    return onlp_file_read_int_max(&info->mcelsius, cpu_coretemp_files);
                }
                else {
                    return onlp_file_read_int(&info->mcelsius, devfiles[id]);
                }
                break;
            }
        case ONLP_OID_TYPE_PSU:
            {
                psu_id = ONLP_OID_ID_GET(info->hdr.poid);
                psu_type = get_psu_type(psu_id, NULL, 0);
                if (psu_type == PSU_TYPE_AC_3YPOWER_F2B || psu_type == PSU_TYPE_AC_3YPOWER_B2F  ) {
                    int rv = psu_ym2401_pmbus_info_get(psu_id, "psu_temp1_input", &info->mcelsius);
                    return rv;
                }
                break;
            }
        default:
            break;
        }
    return ONLP_STATUS_E_PARAM;
}
