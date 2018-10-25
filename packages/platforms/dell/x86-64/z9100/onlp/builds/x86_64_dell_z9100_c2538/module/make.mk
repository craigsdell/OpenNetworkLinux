###############################################################################
#
# 
#
###############################################################################
THIS_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
x86_64_dell_z9100_c2538_INCLUDES := -I $(THIS_DIR)inc
x86_64_dell_z9100_c2538_INTERNAL_INCLUDES := -I $(THIS_DIR)src
x86_64_dell_z9100_c2538_DEPENDMODULE_ENTRIES := init:x86_64_dell_z9100_c2538 ucli:x86_64_dell_z9100_c2538

