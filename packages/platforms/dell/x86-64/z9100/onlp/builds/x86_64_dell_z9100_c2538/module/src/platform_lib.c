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
#include <onlp/onlp.h>
#include <onlplib/file.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <AIM/aim.h>
#include "platform_lib.h"

int
smbus_set_perms() {
    /* Get access to the ports */
    if (ioperm(SF_BUS_RAM_ADDR_HIGH, 4, 1)) {
      AIM_LOG_ERROR("set ioperm ram_addr_high failed\n");
      return(ONLP_STATUS_E_INTERNAL);
    }
    return ONLP_STATUS_OK;
}

int
smbus_unset_perms() {
    /* Get access to the ports */
    if (ioperm(SF_BUS_RAM_ADDR_HIGH, 4, 0)) {
      AIM_LOG_ERROR("unset ioperm ram_addr_high failed\n");
      return(ONLP_STATUS_E_INTERNAL);
    }
    return ONLP_STATUS_OK;
}

/* Read a byte from a bus device */
int
smbus_read_byte(uint16_t addr, uint8_t *data) {

  /* Set the addr of the device we want to read */
  outb(((addr>>8)&0xff), SF_BUS_RAM_ADDR_HIGH);
  outb((addr&0xff), SF_BUS_RAM_ADDR_LOW);
  /* now read and return it */
  *data = inb(SF_BUS_READ_DATA_ADDR);
  return ONLP_STATUS_OK;
}

int
smbus_write_byte(uint16_t addr, uint8_t data) {

  outb(((addr>>8)&0xff), SF_BUS_RAM_ADDR_HIGH);
  outb((addr&0xff), SF_BUS_RAM_ADDR_LOW);
  outb(data, SF_BUS_WRITE_DATA_ADDR);
  return ONLP_STATUS_OK;
}
