#ifndef LIBRETRO_CORE_OPTIONS_H__
#define LIBRETRO_CORE_OPTIONS_H__

#include <stdlib.h>
#include <string.h>

#include "libretro.h"
#include "retro_inline.h"

#ifndef HAVE_NO_LANGEXTRA
#include "libretro_core_options_intl.h"
#endif

/*
 ********************************
 * VERSION: 2.0
 ********************************
 *
 * - 2.0: Add support for core options v2 interface
 * - 1.3: Move translations to libretro_core_options_intl.h
 *        - libretro_core_options_intl.h includes BOM and utf-8
 *          fix for MSVC 2010-2013
 *        - Added HAVE_NO_LANGEXTRA flag to disable translations
 *          on platforms/compilers without BOM support
 * - 1.2: Use core options v1 interface when
 *        RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION is >= 1
 *        (previously required RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION == 1)
 * - 1.1: Support generation of core options v0 retro_core_option_value
 *        arrays containing options with a single value
 * - 1.0: First commit
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
 ********************************
 * Core Option Definitions
 ********************************
*/

/* RETRO_LANGUAGE_ENGLISH */

/* Default language:
 * - All other languages must include the same keys and values
 * - Will be used as a fallback in the event that frontend language
 *   is not available
 * - Will be used as a fallback for any missing entries in
 *   frontend language definition */

struct retro_core_option_v2_category option_cats_us[] = {
   {
      "system",
      "System",
      "Change system settings."
   },
   {
      "video",
      "Video",
      "Change video settings."
   },
   {
      "audio",
      "Audio",
      "Change audio settings."
   },
   {
      "input",
      "Input",
      "Change input settings."
   },
   { NULL, NULL, NULL },
};

struct retro_core_option_v2_definition option_defs_us[] = {
   {
      "mesen_overclock",
      "Overclock",
      NULL,
      "Use this to overclock or underclock the CPU. NOTE: Overclocking can cause issues in some games.",
      NULL,
      "system",
      {
         { "None",      NULL },
         { "Low",       NULL },
         { "Medium",    NULL },
         { "High",      NULL },
         { "Very High", NULL },
         { NULL, NULL },
      },
      "None"
   },
   {
      "mesen_overclock_type",
      "Overclock Type",
      NULL,
      "Increases the number of scanlines in the PPU, before or after the NMI signal is triggered at the end of the visible frame. This effectively gives more time for games to perform calculations, which can reduce slowdowns in games. 'Before NMI' is the preferred option for overclocking, 'After NMI' should only be used if it does not work as expected.",
      NULL,
      "system",
      {
         { "Before NMI (Recommended)", NULL },
         { "After NMI",                NULL },
         { NULL, NULL },
      },
      "Before NMI (Recommended)"
   },
   {
      "mesen_region",
      "Region",
      NULL,
      "When set to Auto, the emulator will try to detect the game's region (NTSC or PAL), however this is not always possible. When there is nothing to suggest a game is for the PAL region, the NTSC region will be used by default. Dendy is used to mimic a number of different NES clones, in particular, the Dendy, which was a popular clone in Russia.",
      NULL,
      "system",
      {
         { "Auto",  NULL },
         { "NTSC",  NULL },
         { "PAL",   NULL },
         { "Dendy", NULL },
         { NULL, NULL },
      },
      "Auto"
   },
   {
      "mesen_ramstate",
      "Default power-on state for RAM",
      NULL,
      "On a console, the RAM's state at power on is undetermined and relatively random. This option lets you select Mesen's behavior when initializing RAM: set all bits to 0, set all bits to 1, or randomize the value of each bit.",
      NULL,
      "system",
      {
         { "All 0s (Default)", NULL },
         { "All 1s",           NULL },
         { "Random Values",    NULL },
         { NULL, NULL },
      },
      "All 0s (Default)"
   },
   {
      "mesen_fdsautoinsertdisk",
      "FDS: Automatically insert disk",
      NULL,
      "By default, the FDS boots with no disk inserted in the drive. This option makes it so the player does not need to manually insert disk 1, side A manually.",
      NULL,
      "system",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "mesen_fdsfastforwardload",
      "FDS: Fast forward while loading",
      NULL,
      "FDS games contain a large number of load screens due to their data being stored on floppy drives. Mesen needs to emulate this floppy drive's speed to ensure accurate emulation. This option makes it so Mesen runs the emulation as fast as it can when a game is loading data from the disk, which greatly reduces the time spent on loading screens.",
      NULL,
      "system",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "mesen_fdsdiskmessagestyle",
      "FDS: Disk change message style",
      NULL,
      "How disk changes are shown. 'Progress bar' shows a bar that fills over the two-second swap. 'Text only' shows the text alone: 'Ejecting' the side that comes out, then 'Inserting' the side that goes in.",
      NULL,
      "system",
      {
         { "Progress bar", NULL },
         { "Text only",    NULL },
         { NULL, NULL },
      },
      "Progress bar"
   },
   {
      "mesen_fdsdiskmessages",
      "FDS: Disk change messages",
      NULL,
      "Which disk changes are announced on screen. 'Disk buttons only' announces swaps made with the Insert Next Disk and Switch Disk Side buttons or the frontend's disk control (Disc Control menu, Next Disc hotkey). 'Verbose' also announces the disk inserted at boot and swaps that finish after loading a state.",
      NULL,
      "system",
      {
         { "Disk buttons only", NULL },
         { "Verbose",           NULL },
         { NULL, NULL },
      },
      "Disk buttons only"
   },
   {
      "mesen_ntsc_filter",
      "NTSC filter",
      NULL,
      "Select a filter to apply to the picture. Blargg filters are fast, Bisqwit filters are slower.",
      NULL,
      "video",
      {
         { "Disabled",            NULL },
         { "Composite (Blargg)",  NULL },
         { "S-Video (Blargg)",    NULL },
         { "RGB (Blargg)",        NULL },
         { "Monochrome (Blargg)", NULL },
         { "Bisqwit 2x",          NULL },
         { "Bisqwit 4x",          NULL },
         { "Bisqwit 8x",          NULL },
         { NULL, NULL },
      },
      "Disabled"
   },
   {
      "mesen_palette",
      "Palette",
      NULL,
      NULL,
      NULL,
      "video",
      {
         { "Default",                           NULL },
         { "Composite Direct (by FirebrandX)",  NULL },
         { "Nes Classic",                       NULL },
         { "Nestopia (RGB)",                    NULL },
         { "Original Hardware (by FirebrandX)", NULL },
         { "PVM Style (by FirebrandX)",         NULL },
         { "Sony CXA2025AS",                    NULL },
         { "Unsaturated v6 (by FirebrandX)",    NULL },
         { "YUV v3 (by FirebrandX)",            NULL },
         { "Wavebeam (by nakedarthur)",         NULL },
         { "Custom",                            NULL },
         { "Raw",                               NULL },
         { NULL, NULL },
      },
      "Default"
   },
   {
      "mesen_nospritelimit",
      "Remove sprite limit",
      NULL,
      "The NES can normally only draw up to 8 sprites per line, this limitation is indirectly responsible for some of the flickering seen in games at times. When this option is enabled, the limit is disabled, allowing up to 64 sprites to be drawn on the same line.",
      NULL,
      "video",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "mesen_overscan_left",
      "Left Overscan",
      NULL,
      "This overscan setting allow you to cut out pixels on the left side of the screen.",
      NULL,
      "video",
      {
         { "None", NULL },
         { "4px",  NULL },
         { "8px",  NULL },
         { "12px", NULL },
         { "16px", NULL },
         { NULL, NULL },
      },
      "None"
   },
   {
      "mesen_overscan_right",
      "Right Overscan",
      NULL,
      "This overscan setting allow you to cut out pixels on the right side of the screen.",
      NULL,
      "video",
      {
         { "None", NULL },
         { "4px",  NULL },
         { "8px",  NULL },
         { "12px", NULL },
         { "16px", NULL },
         { NULL, NULL },
      },
      "None"
   },
   {
      "mesen_overscan_up",
      "Top Overscan",
      NULL,
      "This overscan setting allow you to cut out pixels on the top of the screen.",
      NULL,
      "video",
      {
         { "None", NULL },
         { "4px",  NULL },
         { "8px",  NULL },
         { "12px", NULL },
         { "16px", NULL },
         { "20px", NULL },
         { "24px", NULL },
         { NULL, NULL },
      },
      "None"
   },
   {
      "mesen_overscan_down",
      "Bottom Overscan",
      NULL,
      "This overscan setting allow you to cut out pixels on the bottom of the screen.",
      NULL,
      "video",
      {
         { "None", NULL },
         { "4px",  NULL },
         { "8px",  NULL },
         { "12px", NULL },
         { "16px", NULL },
         { "20px", NULL },
         { "24px", NULL },
         { NULL, NULL },
      },
      "None"
   },
   {
      "mesen_aspect_ratio",
      "Aspect Ratio",
      NULL,
      NULL,
      NULL,
      "video",
      {
         { "Auto",             NULL },
         { "No Stretching",    NULL },
         { "NTSC",             NULL },
         { "PAL",              NULL },
         { "4:3",              NULL },
         { "4:3 (Preserved)",  NULL },
         { "16:9",             NULL },
         { "16:9 (Preserved)", NULL },
         { NULL, NULL },
      },
      "Auto"
   },
   {
      "mesen_hdpacks",
      "Enable HD Packs",
      NULL,
      NULL,
      NULL,
      "video",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "mesen_screenrotation",
      "Screen Rotation",
      NULL,
      "Rotates the display by the specified angle. This is useful to play games (generally homebrew games) designed for a vertical display.",
      NULL,
      "video",
      {
         { "None",        NULL },
         { "90 degrees",  NULL },
         { "180 degrees", NULL },
         { "270 degrees", NULL },
         { NULL, NULL },
      },
      "None"
   },
   {
      "mesen_fake_stereo",
      "Enable fake stereo effect",
      NULL,
      NULL,
      NULL,
      "audio",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "mesen_mute_triangle_ultrasonic",
      "Reduce popping on Triangle channel",
      NULL,
      "This option mutes the triangle channel under certain conditions, which prevents it from causing popping sounds.",
      NULL,
      "audio",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "mesen_reduce_dmc_popping",
      "Reduce popping on DMC channel",
      NULL,
      "This option prevents games from changing the output of the DMC channel too abruptly, which often causes popping sounds.",
      NULL,
      "audio",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "enabled"
   },
   {
      "mesen_swap_duty_cycle",
      "Swap Square channel duty cycles",
      NULL,
      "This option is to mimic some older NES clones that had incorrect sound output for both of the square channels. It greatly alters the sound in some games, and shouldn't be enabled unless you want this behavior.",
      NULL,
      "audio",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "mesen_disable_noise_mode_flag",
      "Disable Noise channel mode flag",
      NULL,
      "Very early Famicom models did not implement this feature, so this option is available to mimic early Famicom consoles. It changes the sound output of the noise channel in some games, and shouldn't be enabled unless you want this behavior.",
      NULL,
      "audio",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   {
      "mesen_audio_sample_rate",
      "Sound Output Sample Rate",
      NULL,
      NULL,
      NULL,
      "audio",
      {
         { "11025", NULL },
         { "22050", NULL },
         { "44100", NULL },
         { "48000", NULL },
         { "96000", NULL },
         { NULL, NULL },
      },
      "48000"
   },
   {
      "mesen_controllerturbospeed",
      "Controller Turbo Speed",
      NULL,
      "Configure the controller turbo buttons speed.",
      NULL,
      "input",
      {
         { "Slow",      NULL },
         { "Normal",    NULL },
         { "Fast",      NULL },
         { "Very Fast", NULL },
         { "Disabled",  NULL },
         { NULL, NULL },
      },
      "Fast"
   },
   {
      "mesen_shift_buttons_clockwise",
      "Shift A/B/X/Y clockwise",
      NULL,
      NULL,
      NULL,
      "input",
      {
         { "disabled", NULL },
         { "enabled",  NULL },
         { NULL, NULL },
      },
      "disabled"
   },
   { NULL, NULL, NULL, NULL, NULL, NULL, {{0}}, NULL },
};

struct retro_core_options_v2 options_us = {
   option_cats_us,
   option_defs_us
};

/*
 ********************************
 * Language Mapping
 ********************************
*/

#ifndef HAVE_NO_LANGEXTRA
struct retro_core_options_v2 *options_intl[RETRO_LANGUAGE_LAST] = {
   &options_us, /* RETRO_LANGUAGE_ENGLISH */
   NULL,        /* RETRO_LANGUAGE_JAPANESE */
   NULL,        /* RETRO_LANGUAGE_FRENCH */
   NULL,        /* RETRO_LANGUAGE_SPANISH */
   NULL,        /* RETRO_LANGUAGE_GERMAN */
   NULL,        /* RETRO_LANGUAGE_ITALIAN */
   NULL,        /* RETRO_LANGUAGE_DUTCH */
   NULL,        /* RETRO_LANGUAGE_PORTUGUESE_BRAZIL */
   NULL,        /* RETRO_LANGUAGE_PORTUGUESE_PORTUGAL */
   NULL,        /* RETRO_LANGUAGE_RUSSIAN */
   NULL,        /* RETRO_LANGUAGE_KOREAN */
   NULL,        /* RETRO_LANGUAGE_CHINESE_TRADITIONAL */
   NULL,        /* RETRO_LANGUAGE_CHINESE_SIMPLIFIED */
   NULL,        /* RETRO_LANGUAGE_ESPERANTO */
   NULL,        /* RETRO_LANGUAGE_POLISH */
   NULL,        /* RETRO_LANGUAGE_VIETNAMESE */
   NULL,        /* RETRO_LANGUAGE_ARABIC */
   NULL,        /* RETRO_LANGUAGE_GREEK */
   NULL,        /* RETRO_LANGUAGE_TURKISH */
   NULL,        /* RETRO_LANGUAGE_SLOVAK */
   NULL,        /* RETRO_LANGUAGE_PERSIAN */
   NULL,        /* RETRO_LANGUAGE_HEBREW */
   NULL,        /* RETRO_LANGUAGE_ASTURIAN */
   NULL,        /* RETRO_LANGUAGE_FINNISH */
   NULL,        /* RETRO_LANGUAGE_INDONESIAN */
   NULL,        /* RETRO_LANGUAGE_SWEDISH */
   NULL,        /* RETRO_LANGUAGE_UKRAINIAN */
   NULL,        /* RETRO_LANGUAGE_CZECH */
   NULL,        /* RETRO_LANGUAGE_CATALAN_VALENCIA */
   NULL,        /* RETRO_LANGUAGE_CATALAN */
   NULL,        /* RETRO_LANGUAGE_BRITISH_ENGLISH */
   NULL,        /* RETRO_LANGUAGE_HUNGARIAN */
   NULL,        /* RETRO_LANGUAGE_BELARUSIAN */
};
#endif

/*
 ********************************
 * Functions
 ********************************
*/

/* Handles configuration/setting of core options.
 * Should be called as early as possible - ideally inside
 * retro_set_environment(), and no later than retro_load_game()
 * > We place the function body in the header to avoid the
 *   necessity of adding more .c files (i.e. want this to
 *   be as painless as possible for core devs)
 */

static INLINE void libretro_set_core_options(retro_environment_t environ_cb,
      bool *categories_supported)
{
   unsigned version  = 0;
#ifndef HAVE_NO_LANGEXTRA
   unsigned language = 0;
#endif

   if (!environ_cb || !categories_supported)
      return;

   *categories_supported = false;

   if (!environ_cb(RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION, &version))
      version = 0;

   if (version >= 2)
   {
#ifndef HAVE_NO_LANGEXTRA
      struct retro_core_options_v2_intl core_options_intl;

      core_options_intl.us    = &options_us;
      core_options_intl.local = NULL;

      if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
          (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH))
         core_options_intl.local = options_intl[language];

      *categories_supported = environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL,
            &core_options_intl);
#else
      *categories_supported = environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2,
            &options_us);
#endif
   }
   else
   {
      size_t i, j;
      size_t option_index              = 0;
      size_t num_options               = 0;
      struct retro_core_option_definition
            *option_v1_defs_us         = NULL;
#ifndef HAVE_NO_LANGEXTRA
      size_t num_options_intl          = 0;
      struct retro_core_option_v2_definition
            *option_defs_intl          = NULL;
      struct retro_core_option_definition
            *option_v1_defs_intl       = NULL;
      struct retro_core_options_intl
            core_options_v1_intl;
#endif
      struct retro_variable *variables = NULL;
      char **values_buf                = NULL;

      /* Determine total number of options */
      while (true)
      {
         if (option_defs_us[num_options].key)
            num_options++;
         else
            break;
      }

      if (version >= 1)
      {
         /* Allocate US array */
         option_v1_defs_us = (struct retro_core_option_definition *)
               calloc(num_options + 1, sizeof(struct retro_core_option_definition));

         /* Copy parameters from option_defs_us array */
         for (i = 0; i < num_options; i++)
         {
            struct retro_core_option_v2_definition *option_def_us = &option_defs_us[i];
            struct retro_core_option_value *option_values         = option_def_us->values;
            struct retro_core_option_definition *option_v1_def_us = &option_v1_defs_us[i];
            struct retro_core_option_value *option_v1_values      = option_v1_def_us->values;

            option_v1_def_us->key           = option_def_us->key;
            option_v1_def_us->desc          = option_def_us->desc;
            option_v1_def_us->info          = option_def_us->info;
            option_v1_def_us->default_value = option_def_us->default_value;

            /* Values must be copied individually... */
            while (option_values->value)
            {
               option_v1_values->value = option_values->value;
               option_v1_values->label = option_values->label;

               option_values++;
               option_v1_values++;
            }
         }

#ifndef HAVE_NO_LANGEXTRA
         if (environ_cb(RETRO_ENVIRONMENT_GET_LANGUAGE, &language) &&
             (language < RETRO_LANGUAGE_LAST) && (language != RETRO_LANGUAGE_ENGLISH) &&
             options_intl[language])
            option_defs_intl = options_intl[language]->definitions;

         if (option_defs_intl)
         {
            /* Determine number of intl options */
            while (true)
            {
               if (option_defs_intl[num_options_intl].key)
                  num_options_intl++;
               else
                  break;
            }

            /* Allocate intl array */
            option_v1_defs_intl = (struct retro_core_option_definition *)
                  calloc(num_options_intl + 1, sizeof(struct retro_core_option_definition));

            /* Copy parameters from option_defs_intl array */
            for (i = 0; i < num_options_intl; i++)
            {
               struct retro_core_option_v2_definition *option_def_intl = &option_defs_intl[i];
               struct retro_core_option_value *option_values           = option_def_intl->values;
               struct retro_core_option_definition *option_v1_def_intl = &option_v1_defs_intl[i];
               struct retro_core_option_value *option_v1_values        = option_v1_def_intl->values;

               option_v1_def_intl->key           = option_def_intl->key;
               option_v1_def_intl->desc          = option_def_intl->desc;
               option_v1_def_intl->info          = option_def_intl->info;
               option_v1_def_intl->default_value = option_def_intl->default_value;

               /* Values must be copied individually... */
               while (option_values->value)
               {
                  option_v1_values->value = option_values->value;
                  option_v1_values->label = option_values->label;

                  option_values++;
                  option_v1_values++;
               }
            }
         }

         core_options_v1_intl.us    = option_v1_defs_us;
         core_options_v1_intl.local = option_v1_defs_intl;

         environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL, &core_options_v1_intl);
#else
         environ_cb(RETRO_ENVIRONMENT_SET_CORE_OPTIONS, option_v1_defs_us);
#endif
      }
      else
      {
         /* Allocate arrays */
         variables  = (struct retro_variable *)calloc(num_options + 1,
               sizeof(struct retro_variable));
         values_buf = (char **)calloc(num_options, sizeof(char *));

         if (!variables || !values_buf)
            goto error;

         /* Copy parameters from option_defs_us array */
         for (i = 0; i < num_options; i++)
         {
            const char *key                        = option_defs_us[i].key;
            const char *desc                       = option_defs_us[i].desc;
            const char *default_value              = option_defs_us[i].default_value;
            struct retro_core_option_value *values = option_defs_us[i].values;
            size_t buf_len                         = 3;
            size_t default_index                   = 0;

            values_buf[i] = NULL;

            if (desc)
            {
               size_t num_values = 0;

               /* Determine number of values */
               while (true)
               {
                  if (values[num_values].value)
                  {
                     /* Check if this is the default value */
                     if (default_value)
                        if (strcmp(values[num_values].value, default_value) == 0)
                           default_index = num_values;

                     buf_len += strlen(values[num_values].value);
                     num_values++;
                  }
                  else
                     break;
               }

               /* Build values string */
               if (num_values > 0)
               {
                  buf_len += num_values - 1;
                  buf_len += strlen(desc);

                  values_buf[i] = (char *)calloc(buf_len, sizeof(char));
                  if (!values_buf[i])
                     goto error;

                  strcpy(values_buf[i], desc);
                  strcat(values_buf[i], "; ");

                  /* Default value goes first */
                  strcat(values_buf[i], values[default_index].value);

                  /* Add remaining values */
                  for (j = 0; j < num_values; j++)
                  {
                     if (j != default_index)
                     {
                        strcat(values_buf[i], "|");
                        strcat(values_buf[i], values[j].value);
                     }
                  }
               }
            }

            variables[option_index].key   = key;
            variables[option_index].value = values_buf[i];
            option_index++;
         }

         /* Set variables */
         environ_cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
      }

error:
      /* Clean up */

      if (option_v1_defs_us)
      {
         free(option_v1_defs_us);
         option_v1_defs_us = NULL;
      }

#ifndef HAVE_NO_LANGEXTRA
      if (option_v1_defs_intl)
      {
         free(option_v1_defs_intl);
         option_v1_defs_intl = NULL;
      }
#endif

      if (values_buf)
      {
         for (i = 0; i < num_options; i++)
         {
            if (values_buf[i])
            {
               free(values_buf[i]);
               values_buf[i] = NULL;
            }
         }

         free(values_buf);
         values_buf = NULL;
      }

      if (variables)
      {
         free(variables);
         variables = NULL;
      }
   }
}

#ifdef __cplusplus
}
#endif

#endif
