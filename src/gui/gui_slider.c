
/*
 * Description:
 *
 * Author: Rodrigo Freitas
 * Created at: Sun Dec 14 00:24:40 2014
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

static int internal_callback(void *arg, int pos)
{
    struct callback_data *acd = (struct callback_data *)arg;

    callback_set_int(acd, pos);

    return run_callback(acd, D_O_K);
}

int gui_d_slider_proc(int msg, DIALOG *d, int c)
{
    switch (msg) {
        case MSG_START:
            /*
             * Initiate the callback function (supported by Allegro) in case
             * the user installed its own function.
             */
            if (d->dp3 != NULL)
                d->dp2 = internal_callback;

            break;
    }

    return d_slider_proc(msg, d, c);
}

