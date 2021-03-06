
/*
 * Description: Translate info loaded from an GRC UI file to Allegro
 *              calls.
 *
 * Author: Rodrigo Freitas
 * Created at: Mon Dec 15 11:25:28 2014
 * Project: libgrc
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

#include "libgrc.h"
#include "gui/objects.h"

/*
 * Turn back to text mode.
 */
void gui_reset_resolution(void)
{
    cl_msleep(100);
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    remove_keyboard();
    allegro_exit();
}

/*
 * ------- DIALOG handling functions -------
 */

static void DIALOG_creation_start(DIALOG *dlg, struct grc_s *grc)
{
    DIALOG *p;
    struct grc_object_s *gobj = NULL;

    gobj = new_grc_object(STANDARD_OBJECT);

    if (NULL == gobj)
        return;

    /* Sets the DIALOG object */
    p = gobj->dlg;
    p->proc = d_clear_proc;
    dlg[0] = *p;

    /* Saves this new objects so we can free it later */
    grc->tmp_objects = cl_dll_unshift(grc->tmp_objects, gobj);
}

static void DIALOG_creation_finish(DIALOG *dlg, unsigned int index,
    struct grc_s *grc)
{
    DIALOG *p;
    struct grc_object_s *gobj = NULL;

    gobj = new_grc_object(STANDARD_OBJECT);

    if (NULL == gobj)
        return;

    /* Sets the DIALOG last known object */
    p = gobj->dlg;
    p->proc = d_yield_proc;
    dlg[index] = *p;

    /* Saves this new objects so we can free it later */
    grc->tmp_objects = cl_dll_unshift(grc->tmp_objects, gobj);

    /* Create the real end of the DIALOG */
    gobj = new_grc_object(STANDARD_OBJECT);

    if (NULL == gobj)
        return;

    /* Sets the DIALOG end mark */
    p = gobj->dlg;
    p->proc = NULL;
    dlg[index + 1] = *p;

    /* Saves this new objects so we can free it later */
    grc->tmp_objects = cl_dll_unshift(grc->tmp_objects, gobj);
}

/*
 * Simple callback function to block ESC key to the user.
 */
static int __disable_key(void *arg __attribute__((unused)))
{
    return D_O_K;
}

static int DIALOG_add_default_esc_key(DIALOG *dlg, unsigned int index,
    struct grc_s *grc)
{
    DIALOG *p;
    struct grc_object_s *gobj = NULL;

    /* Was the key defined by the user? */
    if (info_get_value(grc->info, INFO_ESC_KEY_USER_DEFINED) == true)
        return 0;

    /* Did the user ask to ignore the ESC key? */
    if (info_get_value(grc->info, INFO_IGNORE_ESC_KEY) == false)
        return 0;

    gobj = new_grc_object(STANDARD_OBJECT);

    if (NULL == gobj)
        return 0;

    /*
     * Uses the Allegro key object to avoid create an unecessary
     * 'callback_data' structure.
     */
    p = gobj->dlg;
    p->proc = d_keyboard_proc;
    p->d1 = KEY_ESC;
    p->dp = __disable_key;
    dlg[index] = *p;

    /* Saves this new objects so we can free it later */
    grc->tmp_objects = cl_dll_unshift(grc->tmp_objects, gobj);

    /*
     * We need to tell if the item was inserted or not, that's why the
     * return here is a positive value. Thus if a menu is also needed
     * by the DIALOG its index inside it will be correctly calculated.
     */
    return 1;
}

static int create_menu_item(unsigned int index, void *a, void *b)
{
    struct grc_object_s *it = (struct grc_object_s *)a;
    MENU *menu = (MENU *)b;
    MENU *m = NULL;
    struct grc_obj_properties *prop = NULL;
    char *text;

    m = grc_object_get_MENU(it);
    prop = grc_object_get_properties(it);

    /* Set the menu item text */
    text = (char *)PROP_get(prop, text);

    if (strcmp(text, MENU_SEPARATOR)) {
        m->text = text;
        m->dp = (char *)PROP_get(prop, name);
    } else
        m->text = "";

    /* Add the item into the menu */
    menu[index] = *m;

    return 0;
}

static int create_menu(unsigned int index, void *a, void *b)
{
    struct grc_object_s *o = (struct grc_object_s *)a;
    MENU *menu = (MENU *)b;
    MENU *m = NULL;
    struct grc_obj_properties *prop = NULL;

    m = grc_object_get_MENU(o);
    prop = grc_object_get_properties(o);

    /* Set the menu item text */
    menu[index].text = (char *)PROP_get(prop, text);

    if (o->items != NULL) {
        /* Create the menu and insert all its items */
        m = calloc(cl_dll_size(o->items) + 1, sizeof(MENU));

        if (NULL == m)
            return -1;

        cl_dll_map_indexed(o->items, create_menu_item, m);
        grc_object_set_MENU(o, m);
    }

    menu[index].child = m;

    return 0;
}

static void DIALOG_add_menu(DIALOG *dlg, unsigned int index, struct grc_s *grc)
{
    MENU *m = NULL;
    DIALOG *p;
    struct grc_object_s *gobj = NULL;

    gobj = new_grc_object(STANDARD_OBJECT);

    if (NULL == gobj)
        return;

    /*
     * Uses the Allegro key object to avoid create an unnecessary
     * 'callback_data' structure.
     */
    p = gobj->dlg;

    /* Create the main menu */
    m = calloc(cl_dll_size(grc->ui_menu) + 1, sizeof(MENU));

    if (NULL == m)
        goto error_block;

    /* Create all menus and insert them into the main menu */
    cl_dll_map_indexed(grc->ui_menu, create_menu, m);
    grc_object_set_MENU(gobj, m);

    /*
     * Changes the main color so that the menu uses the same that the user
     * defined.
     */
    gui_fg_color = color_get(grc->color, COLOR_FG);
    gui_bg_color = color_get(grc->color, COLOR_BG);

    /* Create the DIALOG menu entry */
    p->proc = d_menu_proc;
    p->dp = m;
    p->x = 0;
    p->y = 0;
    dlg[index] = *p;

    /* Saves this new objects so we can free it later */
    grc->tmp_objects = cl_dll_unshift(grc->tmp_objects, gobj);

    return;

error_block:
    if (gobj != NULL)
        destroy_grc_object(gobj);
}

/*
 * Here we create the Allegro DIALOG array pointing to every previously loaded
 * object.
 */
int DIALOG_create(struct grc_s *grc)
{
    int dlg_items = 0, index = 0;
    DIALOG *d, *q;
    struct grc_object_s *p = NULL;

    /* d_yield_proc */
    dlg_items += 1;

    /* d_clear_proc */
    if (info_get_value(grc->info, INFO_USE_GFX) == true)
        dlg_items += 1;

    /*
     * Even that the user has defined the ESC key, we allocate an extra space.
     * It will not be used...
     */
    if (info_get_value(grc->info, INFO_IGNORE_ESC_KEY) == true)
        dlg_items += 1;

    dlg_items += cl_dll_size(grc->ui_objects);
    dlg_items += cl_dll_size(grc->ui_keys);

    /* We only have one menu */
    dlg_items += (cl_dll_size(grc->ui_menu) > 0 ? 1 : 0);

    /*
     * The total size of the DIALOG will be the amount of keys + the amount of
     * objects + menu + ignored ESC key + d_yield_proc + d_clear_proc +
     * 1 (NULL proc).
     */
    d = calloc(dlg_items + 1, sizeof(DIALOG));

    if (NULL == d) {
        grc_set_errno(GRC_ERROR_MEMORY);
        return -1;
    }

    /* Initializes the DIALOG */
    if (info_get_value(grc->info, INFO_USE_GFX) == true)
        DIALOG_creation_start(d, grc);

    /* Add user defined objects */
    for (p = grc->ui_objects, index = 1; p; p = p->next, index++) {
        q = grc_object_get_DIALOG(p);
        d[index] = *q;
    }
    
    /* Add keys */
    for (p = grc->ui_keys; p; p = p->next, index++) {
        q = grc_object_get_DIALOG(p);
        d[index] = *q;
    }

    /* Ends the DIALOG */
    DIALOG_creation_finish(d, dlg_items - 1, grc);

    /* We ignore the ESC key (if needed) */
    index = DIALOG_add_default_esc_key(d, dlg_items - 2, grc);

    /*
     * Add the menu. Here the item index may vary due an insertion of the
     * default ESC key or not.
     */
    if (grc->ui_menu != NULL)
        DIALOG_add_menu(d, dlg_items - (2 + index), grc);

    /* Points to the new DIALOG */
    grc->dlg = d;

    return 0;
}

int gui_init(struct grc_s *grc)
{
    int w, h;

    if (install_allegro(SYSTEM_AUTODETECT, NULL, NULL)) {
        grc_set_errno(GRC_ERROR_LIB_INIT);
        return -1;
    }

    if (install_keyboard()) {
        grc_set_errno(GRC_ERROR_KEYBOARD_INIT);
        return -1;
    }

    install_timer();
    set_color_depth(info_color_depth(grc));
    w = info_get_value(grc->info, INFO_WIDTH);
    h = info_get_value(grc->info, INFO_HEIGHT);

    if (set_gfx_mode(GFX_XWINDOWS, w, h, 0, 0) != 0) {
        if (set_gfx_mode(GFX_FBCON, w, h, 0, 0) != 0) {
            remove_keyboard();
            allegro_exit();
            grc_set_errno(GRC_ERROR_SET_GFX_MODE);
            return -1;
        }
    } else {
        if (info_get_value(grc->info, INFO_USE_MOUSE) == true) {
            install_mouse();
            gui_mouse_focus = FALSE;
        }
    }

    /* Disable ctrl+alt+end */
    if (info_get_value(grc->info, INFO_BLOCK_KEYS) == false)
        three_finger_flag = FALSE;

    return 0;
}

void run_DIALOG(struct grc_s *grc)
{
    if (info_get_value(grc->info, INFO_USE_GFX) == false)
        centre_dialog(grc->dlg);

    do_dialog(grc->dlg, -1);
}

