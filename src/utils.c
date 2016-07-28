
/*
 * Description: Utility functions inside the library.
 *
 * Author: Rodrigo Freitas
 * Created at: Thu Jul 28 09:01:31 2016
 * Project: libalex
 *
 * Copyright (c) 2014 Rodrigo Freitas
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "libalex.h"

/* Used to map possible objects from a DIALOG */
struct grc_dlg_object {
    char                name[256];
    enum al_grc_object  type;
};

/* Supported objects from a DIALOG */
static struct grc_dlg_object __dlg_objects [] = {
    { DLG_OBJ_KEY,              AL_GRC_OBJ_KEY                  },
    { DLG_OBJ_BOX,              AL_GRC_OBJ_BOX                  },
    { DLG_OBJ_DIGITAL_CLOCK,    AL_GRC_OBJ_DIGITAL_CLOCK        },
    { DLG_OBJ_IMAGE,            AL_GRC_OBJ_IMAGE                },
    { DLG_OBJ_MESSAGES_LOG_BOX, AL_GRC_OBJ_MESSAGES_LOG_BOX     },
    { DLG_OBJ_VAR_TEXT,         AL_GRC_OBJ_VAR_TEXT             },
    { DLG_OBJ_FIXED_TEXT,       AL_GRC_OBJ_FIXED_TEXT           },
    { DLG_OBJ_CUSTOM,           AL_GRC_OBJ_CUSTOM               },
    { DLG_OBJ_BUTTON,           AL_GRC_OBJ_BUTTON               },
    { DLG_OBJ_EDIT,             AL_GRC_OBJ_EDIT                 },
    { DLG_OBJ_LIST,             AL_GRC_OBJ_LIST                 },
    { DLG_OBJ_CHECKBOX,         AL_GRC_OBJ_CHECK                },
    { DLG_OBJ_RADIO,            AL_GRC_OBJ_RADIO                },
    { DLG_OBJ_SLIDER,           AL_GRC_OBJ_SLIDER               },
    { DLG_OBJ_LIVE_IMAGE,       AL_GRC_OBJ_LIVE_IMAGE           },
    { DLG_OBJ_MULTLIVE_IMAGE,   AL_GRC_OBJ_MULTLIVE_IMAGE       },
    { DLG_OBJ_VT_KEYBOARD,      AL_GRC_OBJ_VT_KEYBOARD          },
    { DLG_OBJ_ICON,             AL_GRC_OBJ_ICON                 },
    { DLG_OBJ_TEXTBOX,          AL_GRC_OBJ_TEXTBOX              }
};

#define MAX_DLG_SUPPORTED_OBJECTS   \
    (sizeof(__dlg_objects) / sizeof(__dlg_objects[0]))

/* 'messages_log_box' line break types */
#define LBREAK_RAW_STR      "raw"
#define LBREAK_SMART_STR    "smart"

/* Radio button types */
#define RADIO_CIRCLE        "circle"
#define RADIO_SQUARE        "square"

/* Horizontal positions */
#define POS_H_LEFT          "left"
#define POS_H_RIGHT         "right"

/*
 * This is an original Allegro function. We put it here because there it
 * is declared as static and we need it.
 */
void dotted_rect(int x1, int y1, int x2, int y2, int fg, int bg)
{
    BITMAP *gui_bmp = gui_get_screen();
    int x = ((x1 + y1) & 1) ? 1 : 0;
    int c;

    /* two loops to avoid bank switches */
    for (c = x1; c <= x2; c++)
        putpixel(gui_bmp, c, y1, (((c + y1) & 1) == x) ? fg : bg);

    for (c = x1; c <= x2; c++)
        putpixel(gui_bmp, c, y2, (((c + y2) & 1) == x) ? fg : bg);

    for (c = y1 + 1; c < y2; c++) {
        putpixel(gui_bmp, x1, c, (((c + x1) & 1) == x) ? fg : bg);
        putpixel(gui_bmp, x2, c, (((c + x2) & 1) == x) ? fg : bg);
    }
}

/*
 * Search for an object able to be used in a DIALOG through its name. This
 * name is the same loaded from the GRC file.
 *
 * Returns an int to allow negative values without compile warnings.
 *
 * @type_name: Object name which will be searched.
 */
int tr_str_type_to_grc_type(const char *type_name)
{
    unsigned int i;
    int ret = -1;

    if (NULL == type_name)
        return -1;

    for (i = 0; i < MAX_DLG_SUPPORTED_OBJECTS; i++)
        if (!strcmp(__dlg_objects[i].name, type_name)) {
            ret = __dlg_objects[i].type;
            break;
        }

    return ret;
}

/*
 * Translate a string into a line break mode.
 */
int tr_line_break(const char *mode)
{
    /* default option in case there is no key */
    if ((NULL == mode) || !strcmp(mode, LBREAK_SMART_STR))
        return AL_GRC_LBREAK_SMART;

    if (!strcmp(mode, LBREAK_RAW_STR))
        return AL_GRC_LBREAK_RAW;

    return -1;
}

/*
 * Translate a string to the radio button type.
 */
int tr_radio_type(const char *type)
{
    /* default option in case there is no key */
    if ((NULL == type) || !strcmp(type, RADIO_CIRCLE))
        return AL_GRC_RADIO_CIRCLE;

    if (!strcmp(type, RADIO_SQUARE))
        return AL_GRC_RADIO_SQUARE;

    return -1;
}

/*
 * Translate a string pointing to a horizontal position from an object.
 */
int tr_horizontal_position(const char *pos)
{
    /* default option in case there is no key */
    if ((NULL == pos) || !strcmp(pos, POS_H_RIGHT))
        return AL_GRC_H_POS_RIGHT;

    if (!strcmp(pos, POS_H_LEFT))
        return AL_GRC_H_POS_LEFT;

    return -1;
}

const char *str_line_break(enum al_grc_line_break lbreak)
{
    switch (lbreak) {
        case AL_GRC_LBREAK_SMART:
            return LBREAK_SMART_STR;

        case AL_GRC_LBREAK_RAW:
            return LBREAK_RAW_STR;
    }

    return NULL;
}

const char *str_radio_type(enum al_grc_radio_button_fmt radio)
{
    switch (radio) {
        case AL_GRC_RADIO_CIRCLE:
            return RADIO_CIRCLE;

        case AL_GRC_RADIO_SQUARE:
            return RADIO_SQUARE;
    }

    return NULL;
}

const char *str_horizontal_position(enum al_grc_hpos hpos)
{
    switch (hpos) {
        case AL_GRC_H_POS_RIGHT:
            return POS_H_RIGHT;

        case AL_GRC_H_POS_LEFT:
            return POS_H_LEFT;
    }

    return NULL;
}

const char *str_grc_obj_type(enum al_grc_object obj)
{
    unsigned int i;
    char *n = NULL;

    for (i = 0; i < MAX_DLG_SUPPORTED_OBJECTS; i++)
        if (__dlg_objects[i].type == obj) {
            n = __dlg_objects[i].name;
            break;
        }

    return n;
}
