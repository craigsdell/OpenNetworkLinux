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

/* LED related data */
/* Note: keep in sync with busaddr_map table below */
typedef enum onlp_led_id
{
    LED_RESERVED = 0,
    LED_SYS,
    LED_LOC,
    LED_STK,
    STK_IND,
} onlp_led_id_t;

typedef struct led_light_mode_map {
    onlp_led_id_t id;
    uint8_t driver_led_mode;
    enum onlp_led_mode_e onlp_led_mode;
} led_light_mode_map_t;

led_light_mode_map_t led_map[] = {
{LED_SYS,  0x00,  ONLP_LED_MODE_GREEN},
{LED_SYS,  0x02,  ONLP_LED_MODE_ORANGE},
{LED_SYS,  0x08,  ONLP_LED_MODE_ORANGE_BLINKING},
{LED_LOC,  0x01,  ONLP_LED_MODE_OFF},
{LED_LOC,  0x02,  ONLP_LED_MODE_BLUE_BLINKING},
{LED_STK,  0x01,  ONLP_LED_MODE_OFF},
{LED_STK,  0x02,  ONLP_LED_MODE_GREEN}
};

typedef struct id_2_busaddr_map {
  onlp_led_id_t id;     // Local Id
  uint16_t addr;      // Bus address
} id_2_busaddr_map_t;

/* Id to Bus Address Map */
/* Note: keep this table in sync with onlp_led_id enum above */
id_2_busaddr_map_t busaddr_map[] = {
  { LED_RESERVED, 0x00 },
  { LED_SYS, 0x4d9},
  { LED_LOC, 0x4da},
  { LED_STK, 0x4db},
  { STK_IND, 0x131}
};
#define VALID_ID(id) \
    ((0 <= id && id < sizeof(busaddr_map)/sizeof(busaddr_map[0])) ? 1: 0)

/* Stack Indicator LED - not currently used */
typedef struct stack_indicator_map {
  uint8_t ch;     // Indicator character
  uint8_t out;    // Byte you need to output to cpld1
} stack_indicator_map_t;

stack_indicator_map_t indicator_map[] = {
  { '0', 0x01 },
  { '1', 0x4f },
  { '2', 0x12 },
  { '3', 0x06 },
  { '4', 0x4c },
  { '5', 0x24 },
  { '6', 0x20 },
  { '7', 0x0f },
  { '8', 0x00 },
  { '9', 0x04 },
  { 'A', 0x08 },
  { 'B', 0x60 },
  { 'C', 0x31 },
  { 'D', 0x42 },
  { 'E', 0x30 },
  { 'F', 0x38 },
  { 'a', 0x08 },
  { 'b', 0x60 },
  { 'c', 0x31 },
  { 'd', 0x42 },
  { 'e', 0x30 },
  { 'f', 0x38 }
};

/*
 * Get the information for the given LED OID.
 */
static onlp_led_info_t linfo[] =
{
    { }, /* Not used */
    {
        { ONLP_LED_ID_CREATE(LED_SYS), "System LED", ONLP_OID_CHASSIS,
          .status = ONLP_OID_STATUS_FLAG_PRESENT },
        ONLP_LED_CAPS_GREEN | ONLP_LED_CAPS_ORANGE | ONLP_LED_CAPS_ORANGE_BLINKING,
    },
    {
        { ONLP_LED_ID_CREATE(LED_LOC), "Locator LED", ONLP_OID_CHASSIS,
          .status = ONLP_OID_STATUS_FLAG_PRESENT },
        ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_BLUE_BLINKING,
    },
    {
        { ONLP_LED_ID_CREATE(LED_STK), "Stacking LED", ONLP_OID_CHASSIS,
          .status = ONLP_OID_STATUS_FLAG_PRESENT},
        ONLP_LED_CAPS_OFF | ONLP_LED_CAPS_GREEN,
    },
    {
        { ONLP_LED_ID_CREATE(STK_IND), "Stack Indicator", ONLP_OID_CHASSIS,
          .status = ONLP_OID_STATUS_FLAG_PRESENT},
        ONLP_LED_CAPS_CHAR,
    }
};

static enum onlp_led_mode_e onlp_get_led_mode(int id)
{
    uint8_t data;

    /* Grab the led data */
    ONLP_IF_ERROR_RETURN(smbus_set_perms());
    ONLP_IF_ERROR_RETURN(smbus_read_byte(busaddr_map[id].addr, &data));
    ONLP_IF_ERROR_RETURN(smbus_unset_perms());

    /* convert it to ONLP mode */
    int i;
    for (i = 0; i < sizeof(led_map)/sizeof(led_map[0]); i++)
    {
        if (id == led_map[i].id && data == led_map[i].driver_led_mode)
        {
            return led_map[i].onlp_led_mode;
        }
    }
    AIM_LOG_ERROR("onlp_get_led_mode can't find %d:%2.2x", id, data);
    return ONLP_LED_MODE_INVALID;
}

static int onlp_set_led_mode(int id, enum onlp_led_mode_e mode)
{
    int i;
    /* convert it to driver mode */
    for(i = 0; i < sizeof(led_map)/sizeof(led_map[0]); i++)
    {
        if (id == led_map[i].id && mode == led_map[i].onlp_led_mode)
        {
            ONLP_IF_ERROR_RETURN(smbus_set_perms());
            ONLP_IF_ERROR_RETURN(smbus_write_byte(busaddr_map[id].addr, led_map[i].driver_led_mode));
            ONLP_IF_ERROR_RETURN(smbus_unset_perms());
            return 0;
        }
    }
    return -1;
}

static char onlp_get_led_char(int lid)
{
    uint8_t data;
    int i;

    /* Grab the stack indicator data */
    ONLP_IF_ERROR_RETURN(smbus_set_perms());
    ONLP_IF_ERROR_RETURN(smbus_read_byte(busaddr_map[lid].addr, &data));
    ONLP_IF_ERROR_RETURN(smbus_unset_perms());

    /* convert it to a char */
    for (i=0; i < sizeof(indicator_map)/sizeof(indicator_map[0]); i++) {
      if (indicator_map[i].out == data) {
        return(indicator_map[i].ch);
      }
    }
    printf("onlp_get_led_char got invalid data %2.2x\n", data);
    return -1;
}

static int onlp_set_led_char(int lid, char c)
{
    int i;
    /* Convert to indicator out */
    for (i=0; i < sizeof(indicator_map)/sizeof(indicator_map[0]); i++) {
      if (indicator_map[i].ch == c) {
        ONLP_IF_ERROR_RETURN(smbus_set_perms());
        ONLP_IF_ERROR_RETURN(smbus_write_byte(busaddr_map[lid].addr, indicator_map[i].out));
        ONLP_IF_ERROR_RETURN(smbus_unset_perms());
      }
    }
    printf("onlp_set_led_char invalid character %c\n", c);
    return -1;
}

int
onlp_ledi_hdr_get(onlp_oid_t oid, onlp_oid_hdr_t* hdr)
{
    *hdr = linfo[ONLP_OID_ID_GET(oid)].hdr;
    return 0;
}

int
onlp_ledi_info_get(onlp_oid_t id, onlp_led_info_t* info)
{
    int  local_id;

    local_id = ONLP_OID_ID_GET(id);
    if (!VALID_ID(local_id)) {
        printf("invalid id: %d\n", local_id);
        return ONLP_STATUS_E_INTERNAL;
    }

	/* Set the onlp_oid_hdr_t and capabilities */
    *info = linfo[local_id];

    if (local_id == STK_IND) {
      info->character = onlp_get_led_char(local_id);
    } else {
      info->mode = onlp_get_led_mode(local_id);
    }

    return ONLP_STATUS_OK;
}

/*
 * This function puts the LED into the given mode. It is a more functional
 * interface for multimode LEDs.
 *
 * Only modes reported in the LED's capabilities will be attempted.
 */
int
onlp_ledi_mode_set(onlp_oid_t id, onlp_led_mode_t mode)
{
    int  local_id;

    local_id = ONLP_OID_ID_GET(id);

    if (!VALID_ID(local_id) || onlp_set_led_mode(local_id, mode) != 0)
    {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ONLP_STATUS_OK;
}

/*
 * This function sets the LED character.
 */
int
onlp_ledi_char_set(onlp_oid_t id, char c)
{
    int  local_id;

    local_id = ONLP_OID_ID_GET(id);

    if (!VALID_ID(local_id) || onlp_set_led_char(local_id, c) != 0)
    {
        return ONLP_STATUS_E_INTERNAL;
    }

    return ONLP_STATUS_OK;
}
