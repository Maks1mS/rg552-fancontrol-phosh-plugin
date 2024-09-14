/*
 * Copyright (C) 2024
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Author: Maxim Slipenko <maxim@slipenko.com>
 */

#include "rg552-fancontrol-quick-setting.h"
#include "quick-setting.h"
#include "config.h"

#include <glib/gi18n.h>

/**
 * PhoshRG552FanControlQuickSetting:
 *
 */
struct _PhoshRG552FanControlQuickSetting
{
  PhoshQuickSetting parent;

  PhoshStatusIcon *info;
  gboolean changing;
  GFile *mode_file;
};

G_DEFINE_TYPE(PhoshRG552FanControlQuickSetting, phosh_rg552_fancontrol_quick_setting, PHOSH_TYPE_QUICK_SETTING);

static const gchar *modes[] = {"auto", "low", "medium", "high"};

static void
change_fan_mode(PhoshRG552FanControlQuickSetting *self, const gchar *mode)
{
  GError *error = NULL;
  GOutputStream *output_stream;
  gsize bytes_written;

  output_stream = G_OUTPUT_STREAM(g_file_replace(self->mode_file, NULL, FALSE, G_FILE_CREATE_NONE, NULL, &error));
  if (error)
  {
    g_warning("Failed to open file for writing: %s", error->message);
    g_clear_error(&error);
    return;
  }

  if (!g_output_stream_write_all(output_stream, mode, strlen(mode), &bytes_written, NULL, &error))
  {
    g_warning("Failed to write to file: %s", error->message);
    g_clear_error(&error);
  }

  g_object_unref(output_stream);
}


static void
update_info(PhoshRG552FanControlQuickSetting *self)
{
  char *current_mode_str = NULL;
  const char *info;
  const char *icon_name;
  gsize length;

  if (g_file_load_contents(self->mode_file, NULL, &current_mode_str, &length, NULL, NULL))
  {
    const char *current_mode = g_strstrip(current_mode_str);

    if (g_strcmp0(current_mode, "high") == 0)
    {
      info = _("Fan High");
      icon_name = "power-profile-performance-symbolic";
    }
    else if (g_strcmp0(current_mode, "medium") == 0)
    {
      info = _("Fan Medium");
      icon_name = "power-profile-balanced-symbolic";
    }
    else if (g_strcmp0(current_mode, "low") == 0)
    {
      info = _("Fan Low");
      icon_name = "power-profile-power-saver-symbolic";
    }
    else
    {
      info = _("Fan Auto");
      icon_name = "format-text-plaintext-symbolic";
    }

    g_free(current_mode_str);
  }
  else
  {
    info = _("Fan Auto");
    icon_name = "format-text-plaintext-symbolic";
  }
  
  phosh_status_icon_set_info(self->info, info);
  phosh_status_icon_set_icon_name(self->info, icon_name);
}

static void
on_clicked(PhoshRG552FanControlQuickSetting *self)
{
  guint current_mode_index;
  const gchar *current_mode;

  if (self->changing)
    return;

  self->changing = TRUE;

  /* Read current mode from file */
  gchar *current_mode_str = NULL;
  gsize length;
  if (g_file_load_contents(self->mode_file, NULL, &current_mode_str, &length, NULL, NULL))
  {
    current_mode = g_strstrip(current_mode_str);
  }
  else
  {
    current_mode = "auto";
  }

  /* Find the next mode in the array */
  for (current_mode_index = 0; current_mode_index < G_N_ELEMENTS(modes); current_mode_index++)
  {
    if (g_strcmp0(current_mode, modes[current_mode_index]) == 0)
    {
      break;
    }
  }

  current_mode_index = (current_mode_index + 1) % G_N_ELEMENTS(modes);
  change_fan_mode(self, modes[current_mode_index]);

  g_free(current_mode_str);
  update_info(self);
  self->changing = FALSE;
}

static void
phosh_rg552_fancontrol_quick_setting_class_init(PhoshRG552FanControlQuickSettingClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(widget_class,
                                              "/mobi/phosh/plugins/rg552-fancontrol-quick-setting/qs.ui");

  gtk_widget_class_bind_template_child(widget_class, PhoshRG552FanControlQuickSetting, info);

  gtk_widget_class_bind_template_callback(widget_class, on_clicked);
}

static void
phosh_rg552_fancontrol_quick_setting_init(PhoshRG552FanControlQuickSetting *self)
{
  textdomain (GETTEXT_PACKAGE);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);

  GtkWidget *widget = GTK_WIDGET(self);

  gtk_widget_init_template(widget);

  self->mode_file = g_file_new_for_path("/run/rg552-fancontrol/mode");
  if (self->mode_file == NULL)
  {
    g_warning("Failed to create file object for mode");
    return;
  }

  update_info(self);

  g_signal_connect(widget,
                   "notify::sensitive",
                   G_CALLBACK(update_info),
                   self);
}