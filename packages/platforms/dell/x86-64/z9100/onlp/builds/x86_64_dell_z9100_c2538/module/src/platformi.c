/************************************************************
 * <bsn.cl fy=2014 v=onl>
 *
 *           Copyright 2014 Big Switch Networks, Inc.
 *           Copyright 2018 Dell Inc.
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

#include "x86_64_dell_z9100_c2538_int.h"
#include "x86_64_dell_z9100_c2538_log.h"


const char*
onlp_platformi_get(void)
{
    return "x86-64-dell-z9100-c2538-r0";
}

int
onlp_platformi_sw_init(void)
{
    return 0;
}

int
onlp_platformi_manage_fans(void)
{
    /* Z9100 manages it's own fans */
    return ONLP_STATUS_OK;
}

int
onlp_platformi_manage_leds(void)
{
    return ONLP_STATUS_OK;
}
