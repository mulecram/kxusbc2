#pragma once

#include "fsc_pd/core.h"
#include <stdint.h>
#include <stdbool.h>

void fsc_pd_init(void);
uint16_t fsc_pd_run(void);
void fsc_pd_notify_interrupt(void);
void fsc_pd_enable_interrupt(void);
bool fsc_pd_interrupt_pending(void);

ConnectionState fsc_pd_get_connection_state(void);
PolicyState_t fsc_pd_get_policy_state(void);
uint16_t fsc_pd_get_advertised_current(void);
bool fsc_pd_policy_has_contract(void);

void fsc_pd_swap_roles(void);
