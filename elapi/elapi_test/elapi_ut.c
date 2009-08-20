/*
    ELAPI

    Unit test for the ELAPI event interface.

    Copyright (C) Dmitri Pal <dpal@redhat.com> 2009

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdarg.h>
#define TRACE_HOME
#include "trace.h"
#include "elapi.h"
#include "collection_tools.h"

/* THIS IS A PRIVATE HEADER - included for debugging purposes only! */
#include "elapi_priv.h"

int simple_event_test(void)
{
    int error = 0;
    struct collection_item *event;
    char bin[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

    printf("Simple test START:\n");

    error = elapi_set_default_template(
        E_BASE_DEFV1 | E_BASE_HOSTEXT,
        "%n( bin )", bin, 8,
        " %sb( logical1 )", "false",
        "%sb( logical2   )", "YES",
        " %db(logical3)", 1,
        "%d(int_number),", -200,
        "%u(unsigned_number)", 300,
        "%ld(long_number)", -1234567,
        "%lu(long_unsigned_number)", 123456789,
        "%s(just_string)", "string",
        "%*s(sub_string)", "truncated string", 10, /* Expect word truncated */
        "%e(double_number)", 3.141592 * 3,
        "simple", "value",
        "-" E_UTCTIME, /* Remove UTCTIME from the list */
        E_MESSAGE,
        "%(stamp), %s(sub_string), %(int_number), %(unsigned_number), %(long_unsigned_number), %(bin), %e(double_number)",
        E_EOARG);

    if (error) {
        printf("Failed to set default template! %d\n", error);
        return error;
    }

    error = elapi_create_simple_event(
        &event,
        " %db(foo_logical)", 0,
        "%d(foo_int_number),", -2000,
        "%u(foo_unsigned_number)", 3000,
        "%ld(foo_long_number)", -7654321,
        E_EOARG);

    if (error) {
        printf("Failed to create simple event! %d\n", error);
        return error;
    }

    error = ELAPI_EVT_DEBUG(event);
    if (error)  {
        printf("Failed to log event to debug ! %d\n", error);
        elapi_destroy_event(event);
        return error;
    }

    error = ELAPI_EVT_LOG(event);
    if (error)  {
        printf("Failed to log event to log ! %d\n", error);
        elapi_destroy_event(event);
        return error;
    }

    error = ELAPI_EVT_AUDIT(event);

    if (error)  {
        printf("Failed to log event to audit ! %d\n", error);
        elapi_destroy_event(event);
        return error;
    }

    elapi_destroy_event(event);

    error = elapi_msg(E_TARGET_DEBUG, NULL, "a", "b", "c", "d", E_EOARG);
    if (error) {
        printf("Failed to log \"debug\" event! %d\n", error);
        return error;
    }

    error = elapi_msg(E_TARGET_LOG, NULL, "a", "b", "c", "d", E_EOARG);
    if (error) {
        printf("Failed to log \"log\" event! %d\n", error);
        return error;
    }

    error = elapi_msg(E_TARGET_AUDIT, NULL, "a", "b", "c", "d", E_EOARG);
    if (error) {
        printf("Failed to log \"audit\" event! %d\n", error);
        return error;
    }

    /* Internal function to print dispatcher guts */
    elapi_internal_print_dispatcher(elapi_get_dispatcher());

    printf("Simple test success!\n");

    return error;
}

int complex_event_test(void)
{
    int error = 0;
    struct collection_item *template = NULL;
    struct collection_item *event = NULL, *event_copy = NULL;
    char bin[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    struct collection_item *col = NULL;
    struct elapi_dispatcher *dispatcher = NULL;

    printf("Complex test START:\n");

    error = elapi_create_event_template(
        &template,
        E_BASE_DEFV1 | E_BASE_HOSTEXT,
        "%lu(long_unsigned_number)", 123456789,
        "%s(just_string)", "string",
        "%*s(sub_string)", "truncated string", 10, /* Expect word truncated */
        "%e(double_number)", 3.141592 * 3,
        "simple", "value",
        "-" E_UTCTIME, /* Remove UTCTIME from the list */
        E_MESSAGE,
        "%(stamp), %s(sub_string), %(int_number), %(unsigned_number), %(long_unsigned_number), %(bin), %e(double_number)",
        E_EOARG);

    if (error) {
        printf("Failed to set create template %d\n", error);
        return error;
    }

    error = elapi_create_event(
        &event,
        template,
        NULL,
        0,
        " %db(evt_logical)", 0,
        "%d(evt_int_number),", -2000,
        "%u(evt_unsigned_number)", 3000,
        "%ld(evt_long_number)", -7654321,
        E_EOARG);

    if (error) {
        printf("Failed to set create template %d\n", error);
        elapi_destroy_event_template(template);
        return error;
    }

    col_debug_collection(template, COL_TRAVERSE_FLAT);
    col_debug_collection(event, COL_TRAVERSE_FLAT);

    error = elapi_log(E_TARGET_DEBUG, event);

    elapi_destroy_event(event);

    if (error) {
        printf("Failed to log event! %d\n", error);
        return error;
    }


    elapi_destroy_event_template(template);

    error = elapi_create_event_template(
        &template,
        E_BASE_DEFV1 | E_BASE_HOSTEXT,
        "%n( bin )", bin, 8,
        " %sb( logical1 )", "false",
        "%sb( logical2   )", "YES",
        " %db(logical3)", 1,
        "%d(int_number),", -200,
        "%u(unsigned_number)", 300,
        "%ld(long_number)", -1234567,
        E_MESSAGE,
        "%(stamp), %s(sub_string), %(int_number), %(unsigned_number), %(long_unsigned_number), %(bin), %e(double_number)",
        E_EOARG);

    if (error) {
        printf("Failed to set create template %d\n", error);
        return error;
    }

    if ((error = col_create_collection(&col, "test", 0)) ||
        /* We are forcing overwrite with different type */
        (error = col_add_int_property(col, NULL, "unsigned_number", 1)) ||
        (error = col_add_long_property(col, NULL, "bin", 100000000L))) {
        elapi_destroy_event_template(template);
        printf("Failed to add property. Error %d\n", error);
        return error;
    }

    error = elapi_create_event(
        &event,
        template,
        col,
        COL_ADD_MODE_FLAT,
        E_MESSAGE,
        "%(stamp) a good message",
        "-int_number",
        E_EOARG);

    if (error) {
        printf("Failed to set create template %d\n", error);
        elapi_destroy_event_template(template);
        col_destroy_collection(col);
        return error;
    }

    col_destroy_collection(col);

    col_debug_collection(template, COL_TRAVERSE_FLAT);
    col_debug_collection(event, COL_TRAVERSE_FLAT);


    if ((error = col_create_collection(&col, "test", 0)) ||
        /* We are forsing overwrite with different type */
        (error = col_add_int_property(col, NULL, "zzz", 1)) ||
        (error = col_add_long_property(col, NULL, "zzz2", 100000000L))) {
        elapi_destroy_event_template(template);
        printf("Failed to add property. Error %d\n", error);
        elapi_destroy_event(event);
        return error;
    }

    error = elapi_modify_event(
        event,
        col,
        COL_ADD_MODE_REFERENCE,
        "-"E_MESSAGE,
        "bin", "bin-string",
        E_EOARG);

    if (error) {
        printf("Failed to set create template %d\n", error);
        elapi_destroy_event(event);
        elapi_destroy_event_template(template);
        col_destroy_collection(col);
        return error;
    }

    col_destroy_collection(col);

    error = elapi_copy_event(&event_copy, event);
    if (error) {
        printf("Failed to set create template %d\n", error);
        elapi_destroy_event(event);
        elapi_destroy_event_template(template);
        return error;
    }

    error = elapi_create_dispatcher(&dispatcher, "elapi_ut", "./sdfdsdf");
    if (error) {
        elapi_destroy_event(event);
        elapi_destroy_event(event_copy);
        elapi_destroy_event_template(template);
        printf("Failed to create dispatcher %d\n", error);
        return error;
    }

    error = elapi_dsp_log(E_TARGET_DEBUG, dispatcher, event);

    elapi_destroy_event(event);

    if (error) {
        elapi_destroy_event(event_copy);
        elapi_destroy_event_template(template);
        printf("Failed to log event! %d\n", error);
        return error;
    }

    error = elapi_dsp_log(E_TARGET_DEBUG, dispatcher, event_copy);

    elapi_destroy_event(event_copy);

    if (error) {
        elapi_destroy_event_template(template);
        printf("Failed to log event! %d\n", error);
        return error;
    }

    error = elapi_dsp_msg(E_TARGET_DEBUG, dispatcher, template, "a", "b", "c", "d", E_EOARG);
    if (error) {
        elapi_destroy_event_template(template);
        printf("Failed to log event! %d\n", error);
        return error;
    }

    error = elapi_dsp_msg(E_TARGET_DEBUG, dispatcher, NULL, "a", "b", "c", "d", E_EOARG);
    if (error) {
        elapi_destroy_event_template(template);
        printf("Failed to log event! %d\n", error);
        return error;
    }

    elapi_destroy_event_template(template);

    elapi_internal_print_dispatcher(dispatcher);

    elapi_destroy_dispatcher(dispatcher);

    return error;
}


/* Main function of the unit test */

int main(int argc, char *argv[])
{
    int error = 0;

    printf("Start\n");
    if ((error = simple_event_test()) ||
        (error = complex_event_test())) {
        printf("Failed!\n");
    }
    else printf("Success!\n");
    /* Add other tests here ... */
    return error;
}
