/*
 * Copyright (c) 2004 Nokia. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Nokia nor the names of its contributors may be
 * used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __gtk_khtml_marshal_MARSHAL_H__
#define __gtk_khtml_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:VOID (gtk-khtml-marshal.list:1) */
#define gtk_khtml_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID

/* POINTER:POINTER (gtk-khtml-marshal.list:2) */
extern void gtk_khtml_marshal_POINTER__POINTER (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

/* VOID:POINTER (gtk-khtml-marshal.list:3) */
#define gtk_khtml_marshal_VOID__POINTER	g_cclosure_marshal_VOID__POINTER

/* VOID:POINTER,POINTER,POINTER,POINTER (gtk-khtml-marshal.list:4) */
extern void gtk_khtml_marshal_VOID__POINTER_POINTER_POINTER_POINTER (GClosure     *closure,
                                                                     GValue       *return_value,
                                                                     guint         n_param_values,
                                                                     const GValue *param_values,
                                                                     gpointer      invocation_hint,
                                                                     gpointer      marshal_data);

/* VOID:INT,INT (gtk-khtml-marshal.list:5) */
extern void gtk_khtml_marshal_VOID__INT_INT (GClosure     *closure,
                                             GValue       *return_value,
                                             guint         n_param_values,
                                             const GValue *param_values,
                                             gpointer      invocation_hint,
                                             gpointer      marshal_data);

/* VOID:INT (gtk-khtml-marshal.list:6) */
#define gtk_khtml_marshal_VOID__INT	g_cclosure_marshal_VOID__INT

G_END_DECLS

#endif /* __gtk_khtml_marshal_MARSHAL_H__ */

