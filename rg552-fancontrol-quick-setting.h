/*
 * Copyright (C) 2024
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Maxim Slipenko <maxim@slipenko.com>
 */


#include <gtk/gtk.h>

#include "quick-setting.h"

#pragma once

G_BEGIN_DECLS

#define PHOSH_TYPE_RG552_FANCONTROL_QUICK_SETTING (phosh_rg552_fancontrol_quick_setting_get_type ())
G_DECLARE_FINAL_TYPE (PhoshRG552FanControlQuickSetting,
                      phosh_rg552_fancontrol_quick_setting,
                      PHOSH, RG552_FANCONTROL_QUICK_SETTING, PhoshQuickSetting)

G_END_DECLS
