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
 ************************************************************/
#include <onlp/platformi/base.h>
#include "platform_lib.h"


#define FAN_RESERVED        0
#define FAN_0_ON_MAIN_BOARD 1
#define FAN_1_ON_MAIN_BOARD	2
#define FAN_2_ON_MAIN_BOARD	3
#define FAN_3_ON_MAIN_BOARD	4
#define FAN_4_ON_MAIN_BOARD	5
#define FAN_5_ON_MAIN_BOARD	6
#define FAN_6_ON_MAIN_BOARD	7
#define FAN_7_ON_MAIN_BOARD	8
#define FAN_8_ON_MAIN_BOARD	9
#define FAN_9_ON_MAIN_BOARD	10
#define FAN_ON_PSU1         11
#define FAN_ON_PSU2         12

typedef struct fan_cfg {
    uint speed_addr;
    int max_speed;
    uint16_t presence_addr;
    uint8_t presence_mask;
    uint16_t status_addr;
    uint8_t status_mask;
} fan_cfg_t;

static fan_cfg_t fans_cfg[] =
{
    { },             /* Not Used */
    { 0xf3,  22500, 0x113, 0x01, 0x115, 0x01 }, /* fan-0 */
    { 0xf5,  22500, 0x113, 0x01, 0x115, 0x02 }, /* fan-1 */
    { 0xf7,  22500, 0x113, 0x02, 0x115, 0x04 }, /* fan-2 */
    { 0xf9,  22500, 0x113, 0x02, 0x115, 0x08 }, /* fan-3 */
    { 0xfb,  22500, 0x113, 0x04, 0x115, 0x10 }, /* fan-4 */
    { 0xfd,  22500, 0x113, 0x04, 0x115, 0x20 }, /* fan-5 */
    { 0xff,  22500, 0x113, 0x08, 0x115, 0x40 }, /* fan-6 */
    { 0x101, 22500, 0x113, 0x08, 0x115, 0x80 }, /* fan-7 */
    { 0x103, 22500, 0x113, 0x10, 0x114, 0x01 }, /* fan-8 */
    { 0x105, 22500, 0x113, 0x10, 0x114, 0x02 }, /* fan-9 */
    { 0x23b, 22500, 0x23d, 0x04, 0x23d, 0x02 }, /* psu-fan-0 */
    { 0x274, 22500, 0x276, 0x04, 0x276, 0x02 }, /* psu-fan-1 */
};

#define MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(_id)                           \
    {                                                                   \
        .hdr = {                                                        \
            .id = ONLP_FAN_ID_CREATE(FAN_##_id##_ON_MAIN_BOARD),        \
            .description = "Chassis Fan "#_id,                          \
            .poid = ONLP_OID_CHASSIS,                                   \
        },                                                              \
        .dir = ONLP_FAN_DIR_UNKNOWN,                                    \
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_GET_PERCENTAGE), \
    }

#define MAKE_FAN_INFO_NODE_ON_PSU(psu_id)                               \
    {                                                                   \
        .hdr = {                                                        \
            .id = ONLP_FAN_ID_CREATE(FAN_ON_PSU##psu_id),               \
            .description = "Chassis PSU-"#psu_id " Fan ",               \
            .poid = ONLP_PSU_ID_CREATE(PSU##psu_id##_ID)                \
        },                                                              \
        .dir = ONLP_FAN_DIR_UNKNOWN,                                    \
        .caps = (ONLP_FAN_CAPS_GET_DIR | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_GET_PERCENTAGE), \
    }

/* Static fan information */
onlp_fan_info_t fan_info[] = {
    { }, /* Not used */
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(0),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(1),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(2),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(3),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(4),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(5),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(6),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(7),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(8),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(9),
    MAKE_FAN_INFO_NODE_ON_PSU(1),
    MAKE_FAN_INFO_NODE_ON_PSU(2),
};

/* Read the fan speed */
static int
get_fan_speed(unsigned int addr) {
  uint8_t high, low;

  ONLP_IF_ERROR_RETURN(smbus_set_perms());
  ONLP_IF_ERROR_RETURN(smbus_read_byte(addr, &high));
  ONLP_IF_ERROR_RETURN(smbus_read_byte(addr+1, &low));
  ONLP_IF_ERROR_RETURN(smbus_unset_perms());
  return((int)(high<<8)|(low));
}

int
onlp_fani_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    uint8_t presence;
    uint8_t status;
    int id = ONLP_OID_ID_GET(oid);

    *hdr = fan_info[id].hdr;

    /* set permissions */
    ONLP_IF_ERROR_RETURN(smbus_set_perms());

    /* Check presence */
    ONLP_IF_ERROR_RETURN(smbus_read_byte(fans_cfg[id].presence_addr, &presence));
    if ((presence&fans_cfg[id].presence_mask) == 0) {
      ONLP_OID_STATUS_FLAG_SET(hdr, PRESENT);

      /* Check status */
      ONLP_IF_ERROR_RETURN(smbus_read_byte(fans_cfg[id].status_addr, &status));
      if ((status&fans_cfg[id].status_mask) != 0) {
        ONLP_OID_STATUS_FLAG_SET(hdr, FAILED);
      }
    }

    /* unset permissions */
    /* Note: we don't really care too much about unsetting perms so we're not
             concerned about error situations above where we return early. */
    ONLP_IF_ERROR_RETURN(smbus_unset_perms());

    return ONLP_STATUS_OK;
}

int
onlp_fani_info_get(onlp_oid_t oid, onlp_fan_info_t* info)
{
    int id = ONLP_OID_ID_GET(oid);
    *info = fan_info[id];

    /* Get hdr */
    ONLP_IF_ERROR_RETURN(onlp_fani_hdr_get(oid, &info->hdr));

    /* If present check speed */
    if (ONLP_OID_STATUS_FLAG_IS_SET(&info->hdr, PRESENT)) {

      /* Grab speed */
      info->rpm = get_fan_speed(fans_cfg[id].speed_addr);
      if (info->rpm < 0) {
        return info->rpm;
      }
      info->percentage = (info->rpm * 100) / fans_cfg[id].max_speed;
    }

    return ONLP_STATUS_OK;
}

/*
 * This function sets the speed of the given fan in RPM.
 *
 * This function will only be called if the fan supprots the RPM_SET
 * capability.
 *
 * It is optional if you have no fans at all with this feature.
 */
int
onlp_fani_rpm_set(onlp_oid_t id, int rpm)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}

/*
 * This function sets the fan speed of the given OID as a percentage.
 *
 * This will only be called if the OID has the PERCENTAGE_SET
 * capability.
 *
 * It is optional if you have no fans at all with this feature.
 */
int
onlp_fani_percentage_set(onlp_oid_t id, int p)
{
    return ONLP_STATUS_E_UNSUPPORTED;
}
