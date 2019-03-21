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
 * Fan Platform Implementation Defaults.
 *
 ***********************************************************/
#include <onlp/platformi/fani.h>
#include <unistd.h>
#include <onlplib/file.h>
#include <fcntl.h>
#include "platform_lib.h"

#define PREFIX_PATH_ON_MAIN_BOARD  "/sys/devices/platform/as5712_54x_fan/"
#define PREFIX_PATH_ON_PSU  "/sys/bus/i2c/devices/"
#define LOCAL_DEBUG 0


#define FAN_RESERVED        0
#define FAN_1_ON_MAIN_BOARD 1
#define FAN_2_ON_MAIN_BOARD	2
#define FAN_3_ON_MAIN_BOARD	3
#define FAN_4_ON_MAIN_BOARD	4
#define FAN_5_ON_MAIN_BOARD	5
#define FAN_1_ON_PSU1       6
#define FAN_1_ON_PSU2       7

#define PROJECT_NAME

#define LEN_FILE_NAME 50

typedef struct last_path_S
{
    char status[LEN_FILE_NAME];
    char speed[LEN_FILE_NAME];
    char direction[LEN_FILE_NAME];
    char ctrl_speed[LEN_FILE_NAME];
    char r_status[LEN_FILE_NAME];
    char r_speed[LEN_FILE_NAME];
}last_path_T;

#define _MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(prj,id) \
    { #prj"fan"#id"_fault", #prj"fan"#id"_speed_rpm", #prj"fan"#id"_direction",\
      #prj"fan"#id"_duty_cycle_percentage", #prj"fanr"#id"_fault", #prj"fanr"#id"_speed_rpm" }

#define MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(prj,id) _MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(prj,id)


#define MAKE_FAN_LAST_PATH_ON_PSU(folder) \
    {#folder"/psu_fan1_fault",  #folder"/psu_fan1_speed_rpm", "",\
     #folder"/psu_fan1_duty_cycle_percentage", "", ""  }

static last_path_T last_path[] =  /* must map with onlp_fan_id */
{
    MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(PROJECT_NAME, FAN_RESERVED),
    MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(PROJECT_NAME, FAN_1_ON_MAIN_BOARD),
    MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(PROJECT_NAME, FAN_2_ON_MAIN_BOARD),
    MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(PROJECT_NAME, FAN_3_ON_MAIN_BOARD),
    MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(PROJECT_NAME, FAN_4_ON_MAIN_BOARD),
    MAKE_FAN_LAST_PATH_ON_MAIN_BOARD(PROJECT_NAME, FAN_5_ON_MAIN_BOARD),

    MAKE_FAN_LAST_PATH_ON_PSU(57-003c),
    MAKE_FAN_LAST_PATH_ON_PSU(58-003f),
};

#define MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(id) \
    { \
        { ONLP_FAN_ID_CREATE(FAN_##id##_ON_MAIN_BOARD), "Chassis Fan "#id, ONLP_OID_CHASSIS }, \
        0x0, \
        ONLP_FAN_CAPS_SET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_GET_PERCENTAGE, \
        0, \
        0, \
     }

#define MAKE_FAN_INFO_NODE_ON_PSU(psu_id, fan_id) \
    { \
    { ONLP_FAN_ID_CREATE(FAN_##fan_id##_ON_PSU##psu_id), "Chassis PSU-"#psu_id " Fan "#fan_id, ONLP_PSU_ID_CREATE(psu_id) }, \
        0x0, \
        0, \
        0, \
        0, \
    }


/* Static fan information */
onlp_fan_info_t linfo[] = {
    { }, /* Not used */
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(1),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(2),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(3),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(4),
    MAKE_FAN_INFO_NODE_ON_MAIN_BOARD(5),
    MAKE_FAN_INFO_NODE_ON_PSU(1,1),
    MAKE_FAN_INFO_NODE_ON_PSU(2,1),
};

#define VALIDATE(_id)                           \
    do {                                        \
        if(!ONLP_OID_IS_FAN(_id)) {             \
            return ONLP_STATUS_E_INVALID;       \
        }                                       \
    } while(0)

#define OPEN_READ_FILE(fd,fullpath,data,nbytes,len) \
    if (LOCAL_DEBUG)                            \
        printf("[Debug][%s][%d][openfile: %s]\n", __FUNCTION__, __LINE__, fullpath); \
    if ((fd = open(fullpath, O_RDONLY)) == -1)  \
       return ONLP_STATUS_E_INTERNAL;           \
    if ((len = read(fd, r_data, nbytes)) <= 0){ \
        close(fd);                              \
        return ONLP_STATUS_E_INTERNAL;}         \
    if (LOCAL_DEBUG)                            \
        printf("[Debug][%s][%d][read data: %s]\n", __FUNCTION__, __LINE__, r_data); \
    if (close(fd) == -1)                        \
        return ONLP_STATUS_E_INTERNAL


/* PSU relative marco */
#define SET_PSU_TYPE_AC_F2B_FAN(info)                                   \
    do {                                                                \
        ONLP_OID_STATUS_FLAG_SET(info, PRESENT);                        \
        info->dir = ONLP_FAN_DIR_F2B;                                   \
        info->caps  |=  ONLP_FAN_CAPS_SET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_GET_PERCENTAGE; \
    } while(0)

#define SET_PSU_TYPE_AC_B2F_FAN(info)                                   \
    do {                                                                \
        ONLP_OID_STATUS_FLAG_SET(info, PRESENT);                        \
        info->dir = ONLP_FAN_DIR_B2F;                                   \
        info->caps |=  ONLP_FAN_CAPS_SET_PERCENTAGE | ONLP_FAN_CAPS_GET_RPM | ONLP_FAN_CAPS_GET_PERCENTAGE; \
    } while(0)

#define SET_PSU_TYPE_UM400D_F2B_FAN(info)               \
    do {                                                \
        ONLP_OID_STATUS_FLAG_SET(info, PRESENT);        \
        info->dir = ONLP_FAN_DIR_F2B;                   \
    } while(0)

#define SET_PSU_TYPE_UM400D_B2F_FAN(info) \
    do { \
        ONLP_OID_STATUS_FLAG_SET(info, PRESENT); \
        info->dir = ONLP_FAN_DIR_B2F; \
    } while(0)


static int
_onlp_fani_info_get_fan(int local_id, onlp_fan_info_t* info)
{
    int   fd, len, nbytes = 10;
    char  r_data[10]   = {0};
    char  fullpath[65] = {0};

    /* get fan/fanr fault status (turn on when any one fails)
     */
    sprintf(fullpath, "%s%s", PREFIX_PATH_ON_MAIN_BOARD, last_path[local_id].status);
    OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);
    if (atoi(r_data) > 0)
        ONLP_OID_STATUS_FLAG_SET(info, FAILED);

    sprintf(fullpath, "%s%s", PREFIX_PATH_ON_MAIN_BOARD, last_path[local_id].r_status);
    OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);
    if (atoi(r_data) > 0)
        ONLP_OID_STATUS_FLAG_SET(info, FAILED);

    /* get fan/fanr direction (both : the same)
     */
    sprintf(fullpath, "%s%s", PREFIX_PATH_ON_MAIN_BOARD, last_path[local_id].direction);
    OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);

    if (atoi(r_data) == 0) /*F2B*/
        info->dir = ONLP_FAN_DIR_F2B;
    else
        info->dir = ONLP_FAN_DIR_B2F;

    /* get fan/fanr speed (take the average of two speeds)
     */
    sprintf(fullpath, "%s%s", PREFIX_PATH_ON_MAIN_BOARD, last_path[local_id].speed);
    OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);
    info->rpm = atoi(r_data);

    sprintf(fullpath, "%s%s", PREFIX_PATH_ON_MAIN_BOARD, last_path[local_id].r_speed);
    OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);
    info->rpm = (info->rpm + atoi(r_data))/2;

    /* get speed percentage from rpm */
    info->percentage = (info->rpm * 100)/16800;

    /* check present */
    if (info->rpm > 0)
        ONLP_OID_STATUS_FLAG_SET(info, PRESENT);

    return ONLP_STATUS_OK;
}

static int
_onlp_fani_info_get_fan_on_psu_ym2401(int pid, onlp_fan_info_t* info)
{
    int val = 0;

    /* get fan status
     */
    if (psu_ym2401_pmbus_info_get(pid, "psu_fan1_speed_rpm", &val) == ONLP_STATUS_OK) {
        ONLP_OID_STATUS_FLAG_SET_VALUE(info, FAILED, val <= 0);
        info->rpm = val;
        info->percentage = (info->rpm * 100) / 21600;
    }

    return ONLP_STATUS_OK;
}

static int
_onlp_fani_info_get_fan_on_psu(int local_id, onlp_fan_info_t* info)
{
    int   psu_id;
    int   fd, len, nbytes = 10;
    char  r_data[10]   = {0};
    char  fullpath[50] = {0};
    psu_type_t psu_type;

    /* get fan other cap status according to psu type
     */
    psu_id    = (local_id-FAN_1_ON_PSU1) + 1;
    DEBUG_PRINT("[psu_id: %d]", psu_id);

    psu_type  = get_psu_type(psu_id, NULL, 0); /* psu_id = 1 , present PSU1. pus_id =2 , present PSU2 */
    DEBUG_PRINT("[psu_type: %d]", psu_type);

    switch (psu_type) {
        case PSU_TYPE_AC_COMPUWARE_F2B:
        case PSU_TYPE_AC_3YPOWER_F2B:
            SET_PSU_TYPE_AC_F2B_FAN(info);
            break;
        case PSU_TYPE_AC_COMPUWARE_B2F:
        case PSU_TYPE_AC_3YPOWER_B2F:
            SET_PSU_TYPE_AC_B2F_FAN(info);
            break;
        case PSU_TYPE_DC_48V_F2B:
            SET_PSU_TYPE_UM400D_F2B_FAN(info);
            break;
        case PSU_TYPE_DC_48V_B2F:
            SET_PSU_TYPE_UM400D_B2F_FAN(info);
            break;
        default:
            if (LOCAL_DEBUG)
                printf("[Debug][%s][%d][psu_type=%d]\n", __FUNCTION__, __LINE__, psu_type);

            break;
    }

    if (psu_type == PSU_TYPE_AC_COMPUWARE_F2B ||
        psu_type == PSU_TYPE_AC_COMPUWARE_B2F  )
    {
        /* get fan fault status
         */
        sprintf(fullpath, "%s%s", PREFIX_PATH_ON_PSU, last_path[local_id].status);
        OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);
        if (atoi(r_data) > 0)
            ONLP_OID_STATUS_FLAG_SET(info, FAILED);

        /* get fan speed
         */
        sprintf(fullpath, "%s%s", PREFIX_PATH_ON_PSU, last_path[local_id].speed);
        OPEN_READ_FILE(fd,fullpath,r_data,nbytes,len);
        info->rpm = atoi(r_data);

        /* get speed percentage from rpm */
        info->percentage = (info->rpm * 100)/19328;
    }
    else if (psu_type == PSU_TYPE_AC_3YPOWER_F2B ||
            psu_type == PSU_TYPE_AC_3YPOWER_B2F  )
    {
        return _onlp_fani_info_get_fan_on_psu_ym2401(psu_id, info);
    }

    return ONLP_STATUS_OK;
}


int
onlp_fani_info_get(onlp_oid_t id, onlp_fan_info_t* info)
{
    int rc = 0;
    int local_id;

    VALIDATE(id);

    local_id = ONLP_OID_ID_GET(id);

    *info = linfo[local_id];

    if (LOCAL_DEBUG)
        printf("\n[Debug][%s][%d][local_id: %d]", __FUNCTION__, __LINE__, local_id);

    switch (local_id)
    {
	    case FAN_1_ON_PSU1:
        case FAN_1_ON_PSU2:
            rc = _onlp_fani_info_get_fan_on_psu(local_id, info);
            break;

        default:
            rc =_onlp_fani_info_get_fan(local_id, info);
            break;
    }

    return rc;
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
    int  fd, len, nbytes=10, local_id;
    char data[10] = {0};
    char fullpath[70] = {0};

    VALIDATE(id);

    local_id = ONLP_OID_ID_GET(id);

    /* reject p=0 (p=0, stop fan) */
    if (p == 0){
        return ONLP_STATUS_E_INVALID;
    }

    /* get fullpath */
    switch (local_id)
	{
        case FAN_1_ON_PSU1:
        case FAN_1_ON_PSU2:
        {
            int psu_id;
            psu_type_t psu_type;

            psu_id   = local_id - FAN_1_ON_PSU1 + 1;
            psu_type = get_psu_type(psu_id, NULL, 0);

            if (psu_type == PSU_TYPE_AC_3YPOWER_F2B ||
                psu_type == PSU_TYPE_AC_3YPOWER_B2F  ) {
                return psu_ym2401_pmbus_info_set(psu_id, "psu_fan1_duty_cycle_percentage", p);
            }

            sprintf(fullpath, "%s%s", PREFIX_PATH_ON_PSU, last_path[local_id].ctrl_speed);
            break;
        }

        default:
            sprintf(fullpath, "%s%s", PREFIX_PATH_ON_MAIN_BOARD, last_path[local_id].ctrl_speed);
            break;
    }

    sprintf(data, "%d", p);

    if (LOCAL_DEBUG)
        printf("[Debug][%s][%d][openfile: %s][data=%s]\n", __FUNCTION__, __LINE__, fullpath, data);

    /* Create output file descriptor */
    fd = open(fullpath, O_WRONLY, 0644);
    if(fd == -1){
        return ONLP_STATUS_E_INTERNAL;
    }

    len = write (fd, data, (ssize_t) nbytes);
    if(len != nbytes){
        close(fd);
        return ONLP_STATUS_E_INTERNAL;
    }

    close(fd);
    return ONLP_STATUS_OK;
}
