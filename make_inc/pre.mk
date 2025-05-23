# Copyright 2018 Oticon A/S
# Copyright 2023 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

# BSIM_BASE_PATH must point to the bsim base component folder (typically placed directly in 
# bsim/components).
# And BSIM_COMPONENTS_PATH must point to the bsim/components folder.
# if these HW models were fetched inside the bsim/components/ folder, it is very likely we will just
# find them one level up. Otherwise these must be properly set in the environment:
BSIM_BASE_PATH?=$(abspath ../ )
BSIM_COMPONENTS_PATH?=${BSIM_BASE_PATH}

include ${BSIM_BASE_PATH}/common/pre.make.inc

2G4_libPhyComv1_COMP_PATH?=$(abspath ${BSIM_COMPONENTS_PATH}/ext_2G4_libPhyComv1)

EXTRA_CC_FLAGS?=

CC:=ccache gcc

DEBUG:=-g
OPT:=
ARCH:=-m32
WARNINGS:=-Wall -Wpedantic
COVERAGE:=
COMMON_CC_FLAGS:=\
  ${ARCH} ${DEBUG} ${OPT} ${WARNINGS} -MMD -MP -std=gnu11 \
  -fdata-sections -ffunction-sections -D_XOPEN_SOURCE=500 -fno-pie -fno-pic ${EXTRA_CC_FLAGS}
