#include <stdlib.h>   /* free */
#include "assembler.h"

/* ---------- helpers (internal) ---------- */
 void free_content_list(Content_List *head)
{
    Content_List *cur;
    Content_List *next;
    cur = head;
    while (cur) {
        next = cur->next;
        /* cur->line is an in-struct array; nothing extra to free */
        free(cur);
        cur = next;
    }
}

 void free_macro_list(Macro_List *head)
{
    Macro_List *cur;
    Macro_List *next;
    cur = head;
    while (cur) {
        next = cur->next;
        /* free the per-macro content list first */
        free_content_list(cur->content_head);
        /* cur->name is an in-struct array */
        free(cur);
        cur = next;
    }
}

void free_label_list(LABEL_LIST *head)
{
    LABEL_LIST *cur;
    LABEL_LIST *next;
    cur = head;
    while (cur) {
        next = cur->next;
        /* label and type are in-struct arrays */
        free(cur);
        cur = next;
    }
}

void free_code_list(Code_list *head)
{
    Code_list *cur;
    Code_list *next;
    cur = head;
    while (cur) {
        next = cur->next;
        /* label_name is an in-struct array */
        free(cur);
        cur = next;
    }
}

void free_data_list(Data_list *head)
{
    Data_list *cur;
    Data_list *next;
    cur = head;
    while (cur) {
        next = cur->next;
        free(cur);
        cur = next;
    }
}

void free_extern_list(Extern_list *head)
{
    Extern_list *cur;
    Extern_list *next;
    cur = head;
    while (cur) {
        next = cur->next;
        /* label is an in-struct array */
        free(cur);
        cur = next;
    }
}
void free_entry_list(Entry_list *head)
{
    Entry_list *cur;
    Entry_list *next;
    cur = head;
    while (cur) {
        next = cur->next;
        /* label is an in-struct array */
        free(cur);
        cur = next;
    }
}

/* single method for free */

void free_assembler_table(Assembler_Table **ptable)
{
    Assembler_Table *t;

    if (ptable == NULL || *ptable == NULL)
        return;

    t = *ptable;

    /* free each list */
    free_macro_list(t->macro_head);
    free_label_list(t->label_head);
    free_code_list(t->code_list_head);
    free_data_list(t->data_list_head);
    free_extern_list(t->extern_list_head);
    free_entry_list(t->entry_list_head);

    /* finally free the table struct and null the caller's pointer */
    free(t);
    *ptable = NULL;
}
