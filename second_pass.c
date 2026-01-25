#include "assembler.h"
#include <stdio.h>

/* second_pass
 * does  : resolve labels in code_list, set A/R/E bits, record extern usages,
 *         copy real addresses to entry list, run second-pass error checks,
 *         and then write output files (translation_unit).
 */

/* The main second pass function  */
void second_pass(Assembler_Table *table,  char file_am []) {
    Code_list *node = table->code_list_head;
    LABEL_LIST *label_info ;
    Errors final_error = VALID;

    while (node) {
        /* If label_name is not empty, needs to be resolved */
        if (strlen(node->label_name) > 0) {
            label_info= find_label(table->label_head, node->label_name);
            if (label_info) {
                /* First, check if it's an entry label */
                if (is_entry_label(table->entry_list_head, node->label_name)) {
                    /* If it's an entry label, update the code word with the address AND the entry list */
                    update_code_for_entry(node, label_info->addr);
                } else {
                    /* Normal label: resolve address, set R=2 for "relocatable" */
                    node->code_word = ((unsigned short)(label_info->addr & 0xFF) << 2) | 0x2;
                }
            } else if (is_extern_label(table->extern_list_head, node->label_name)) {
                /* Use 0 as the address for external reference */
                update_code_for_extern(node);
                add_extern_usage(table, node->label_name, node->addr);
            }
        }
        node = node->next;
    }
    /* Update entry addresses from label list */
    update_entry_addresses((table->entry_list_head), (table->label_head));
    

run_second_pass_errors(table, &final_error);

    if (final_error == ERROR) {
        printf("second pass failed with errors.\n");
        return;
    }
    
    
    /* Write the code list to a file in base 4 uniqe ny the course  */
    translation_unit(table);
    printf("Second pass completed. Output written to\n");

}



/* find_label
 * input : head of label list, label name
 * does  : search by name
 * return: pointer to label node or NULL
 */

LABEL_LIST* find_label(LABEL_LIST *head,  char *label) {
    while (head) {
        
        if (strcmp(head->label, label) == 0) {  
            return head;
        }
        head = head->next;
    }
    return NULL;
}

/* is_entry_label
 * input : entry list head, label name
 * return: 1 if label is in entry list, else 0
 */

int is_entry_label(Entry_list *entry_head, char *label) {
  
    while (entry_head) {
        if (strcmp(entry_head->label, label) == 0){  /* If label is found, update its address and return true */
        return 1; 
        } 
        
        
        entry_head = entry_head->next;}
    
    return 0;
}

/* is_extern_label
 * input : extern list head, label name
 * return: 1 if label is in extern list, else 0
 */

int is_extern_label(Extern_list *extern_head,char *label) {
    while (extern_head) {
        if (strcmp(extern_head->label, label) == 0) 
        return 1;
        extern_head = extern_head->next;
    }
    return 0;
}

/* update_code_for_entry
 * input : code node, resolved address
 * does  : put address in first bits and set A/R/E = 10b (relocatable)
 */
void update_code_for_entry(Code_list *node, int addr) {
    node->code_word = ((unsigned short)(addr & 0xFF) << 2) | 0x2; /* last 2 bits = 10 (entry) */
}

/* update_code_for_extern
 * input : code node
 * does  : set A/R/E = 01b (external ref), address field = 0
 */

void update_code_for_extern(Code_list *node) {
    node->code_word = 1; /* last 2 bits = 01 (external) */
}

/* add_extern_usage
 * input : table, extern label, use-site address
 * does  : append a usage record (label + address) to extern list
 */

void add_extern_usage(Assembler_Table *table,  char *label, int addr) {
    add_extern_to_list(&table->extern_list_head, label, addr);} /*this time its with an addr and not -1 */


/* update_entry_addresses
 * input : entry list head, label list head
 * does  : copy real addresses from labels into entry nodes
 */
void update_entry_addresses(Entry_list *entry_head, LABEL_LIST *label_head) {
    Entry_list *entry = entry_head;
    while (entry) {
        LABEL_LIST *label_info = find_label(label_head, entry->label);
         if (label_info) {
            entry->addr = label_info->addr;
        }
        entry = entry->next;
    }
}
