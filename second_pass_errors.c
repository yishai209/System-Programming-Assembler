#include "assembler.h"


/* run_second_pass_errors
 * input : assembler table, out param final_error
 * does  : execute all second-pass semantic checks and print errors found
 */

Errors run_second_pass_errors(Assembler_Table *table, Errors *final_error)
{
    Errors tmp = VALID;

    if (table == NULL) return VALID;

    if (check_duplicate_labels(table->label_head, final_error) == ERROR) tmp = ERROR;
    if (check_entry_defined(table->entry_list_head, table->label_head, final_error) == ERROR) tmp = ERROR;
    if (check_entry_extern_conflicts(table->entry_list_head, table->extern_list_head, final_error) == ERROR) tmp = ERROR;
    if (check_extern_not_defined(table->extern_list_head, table->label_head, final_error) == ERROR) tmp = ERROR;
    if (check_undefined_operands(table->code_list_head, table->label_head, table->extern_list_head, final_error) == ERROR) tmp = ERROR;

    return tmp;
}



/* ---------- local helpers ---------- */

/* name_equals
 * input : two C strings
 * does  : return 1 if both non-NULL and equal, else 0
 */

int name_equals(const char *a, const char *b)
{
    return (a != NULL && b != NULL && strcmp(a, b) == 0);
}

/* label_exists
 * input : label list, name, optional out addr/type
 * does  : search by name and optionally return address/type when found
 */

int label_exists(LABEL_LIST *labels, const char *name, int *out_addr, const char **out_type)
{
    LABEL_LIST *p = labels;
    while (p != NULL) {
        if (name_equals(p->label, name)) {
            if (out_addr) *out_addr = p->addr;
            if (out_type) *out_type = p->type;
            return 1;
        }
        p = p->next;
    }
    return 0;
}

/* is_declared_extern
 * input : extern list, name
 * does  : return 1 if name appears as declaration (addr == DEFINITION_ADDR), else 0
 */

/* true if name appears in extern list as a declaration (addr == DEFINITION_ADDR) */
int is_declared_extern(Extern_list *exts, const char *name)
{
    Extern_list *p = exts;
    while (p != NULL) {
        if (name_equals(p->label, name) && p->addr == DEFINITION_ADDR) return 1;
        p = p->next;
    }
    return 0;
}

/* check_duplicate_labels
 * input : head of label list, out param final_error
 * does  : report labels defined more than once (by same name)
 */

Errors check_duplicate_labels(LABEL_LIST *labels, Errors *final_error)
{
    LABEL_LIST *a;
    LABEL_LIST *b;

    for (a = labels; a != NULL; a = a->next) {
        for (b = a->next; b != NULL; b = b->next) {
            if (name_equals(a->label, b->label)) {
                /* defined twice (even if types differ) */
                printf(ERR_DUP_LABEL_DEF "\n" , a->label, a->addr, b->addr);
                *final_error = ERROR;
            }
        }
    }
    return (*final_error == ERROR) ? ERROR : VALID;
}

/* check_entry_extern_conflicts
 * input : entry list, extern list, out param final_error
 * does  : flag names declared both .entry and .extern
 */

Errors check_entry_extern_conflicts(Entry_list *entries, Extern_list *externs, Errors *final_error)
{
    Entry_list *e = entries;
    while (e != NULL) {
        Extern_list *x = externs;
        while (x != NULL) {
            if (name_equals(e->label, x->label) && x->addr == DEFINITION_ADDR) {
                printf(ERR_ENTRY_EXTERN_CONFLICT "\n", e->label);
                *final_error = ERROR;
            }
            x = x->next;
        }
        e = e->next;
    }
    return (*final_error == ERROR) ? ERROR : VALID;
}

/* check_entry_defined
 * input : entry list, label list, out param final_error
 * does  : ensure every .entry symbol is defined in label list
 */

Errors check_entry_defined(Entry_list *entries, LABEL_LIST *labels, Errors *final_error)
{
    Entry_list *e = entries;
    while (e != NULL) {
        int addr_dummy = 0;
        if (!label_exists(labels, e->label, &addr_dummy, (const char **)0)) {
            printf(ERR_ENTRY_NOT_DEFINED "\n", e->label);
            *final_error = ERROR;
        }
        e = e->next;
    }
    return (*final_error == ERROR) ? ERROR : VALID;
}

/* check_extern_not_defined
 * input : extern list, label list, out param final_error
 * does  : flag extern symbols that are also defined locally
 */

Errors check_extern_not_defined(Extern_list *externs, LABEL_LIST *labels, Errors *final_error)
{
    Extern_list *x = externs;
    while (x != NULL) {
        if (x->addr == DEFINITION_ADDR) {
            int local_addr = 0;
            if (label_exists(labels, x->label, &local_addr, (const char **)0)) {
                printf(ERR_EXTERN_DEFINED_LOCAL "\n", x->label, local_addr);
                *final_error = ERROR;
            }
        }
        x = x->next;
    }
    return (*final_error == ERROR) ? ERROR : VALID;
}

/* check_undefined_operands
 * input : code list, label list, extern list, out param final_error
 * does  : report code nodes referencing symbols neither defined nor declared extern
 */

Errors check_undefined_operands(Code_list *code, LABEL_LIST *labels, Extern_list *externs, Errors *final_error)
{
    Code_list *c = code;
    while (c != NULL) {
        if (c->label_name[0] != '\0') {
            int addr_dummy = 0;
            if (!label_exists(labels, c->label_name, &addr_dummy, (const char **)0) &&
                !is_declared_extern(externs, c->label_name)) {
                printf(ERR_UNDEFINED_SYMBOL "\n" , c->label_name, c->addr);
                *final_error = ERROR;
            }
        }
        c = c->next;
    }
    return (*final_error == ERROR) ? ERROR : VALID;
}




