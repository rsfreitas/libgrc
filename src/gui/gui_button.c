
/*
 * Description: Objeto customizado para um botao.
 *
 * Author: Rodrigo Freitas
 * Created at: Wed Dec 10 16:06:37 2014
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

/*
 * O objeto permite que uma funcao seja executada quando o botao for
 * ativado. A funcao deve ser indicada em d->dp2, sendo seu argumento
 * definido em d->dp3.
 *
 * Caso nenhuma funcao seja indicada, o botao fara com que o DIALOG
 * seja fechado.
 */
int gui_d_button_proc(int msg, DIALOG *d, int c)
{
    struct al_callback_data *acd = d->dp3;
    int ret;

    ret = d_button_proc(msg, d, c);

    if (ret == D_CLOSE)
        if (d->dp3 != NULL)
            ret = run_callback(acd);

    return ret;
}

