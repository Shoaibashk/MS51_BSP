/*===========================================================================
 * @file    ms51_services.h
 * @brief   MS51 Service Layer Master Include
 * @version 2.0.0
 * @date    2025-11-30
 * 
 * @details Include this single file to access all MS51 service layer APIs.
 * 
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2025 Nuvoton Technology Corp. All rights reserved.
 *===========================================================================*/

#ifndef MS51_SERVICES_H
#define MS51_SERVICES_H

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
 * Service Layer Includes
 *---------------------------------------------------------------------------*/
#include "svc_system.h"     /* System service (clock, reset, WDT, device info) */
#include "svc_delay.h"      /* Delay service (blocking delays, tick) */
#include "svc_console.h"    /* Console service (printf, debug output) */
#include "svc_eeprom.h"     /* EEPROM emulation service (data flash) */
#include "svc_power.h"      /* Power management service (sleep modes) */

/*---------------------------------------------------------------------------
 * Version Information
 *---------------------------------------------------------------------------*/
#define SVC_VERSION_MAJOR   2
#define SVC_VERSION_MINOR   0
#define SVC_VERSION_PATCH   0
#define SVC_VERSION         ((SVC_VERSION_MAJOR << 16) | \
                             (SVC_VERSION_MINOR << 8) | \
                             SVC_VERSION_PATCH)

#ifdef __cplusplus
}
#endif

#endif /* MS51_SERVICES_H */
