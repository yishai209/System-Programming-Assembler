#include "assembler.h"


/* translation_unit
 * input : assembler table (uses table->file_name to form output names)
 * does  : builds "<name>.ob", "<name>.ent", "<name>.ext" and writes:
 *         - .ob  : IC/DC header (base-4) + all code and data words (addr + 10-bit word)
 *         - .ent : entry labels (label + address in base-4)
 *         - .ext : extern usages (label + address in base-4, skipping decl stubs)
 * return: void
 */

void translation_unit(Assembler_Table *table) {
    char file_ob [MAX_LABEL_SIZE] , file_ent[MAX_LABEL_SIZE] , file_ext[MAX_LABEL_SIZE];
    strcpy(file_ob , table->file_name);
    strcpy(file_ent , table->file_name);
    strcpy(file_ext , table->file_name);

    strcat(file_ob , ".ob");
    strcat(file_ent, ".ent");
    strcat(file_ext, ".ext");

    write_assembler_base4(table, file_ob);
    write_entries_base4(table, file_ent);
    write_externs_base4(table, file_ext);
}


/* int_to_base4_letters
 * input : value (integer), out buffer, width
 * does  : converts value to base-4 using letters {a=0,b=1,c=2,d=3};
 *         writes exactly 'width' chars and a trailing '\0'
 * return: void
 */

void int_to_base4_letters(int value, char *out, int width) {
    char map[4] = { 'a', 'b', 'c', 'd' };
    int i;
    for (i = width - 1; i >= 0; i--) {
        out[i] = map[value % 4];
        value /= 4;
    }
    out[width] = '\0';
}

/* write_assembler_base4
 * input : assembler table, output filename (e.g., "<name>.ob")
 * does  : writes first line with IC and DC in base-4 (3 and 2 letters),
 *         then all code words and data words:
 *           - left column: 4-letter base-4 address
 *           - right col : 5-letter base-4 10-bit word (masked)
 * return: void
 */

void write_assembler_base4(Assembler_Table *table, const char *filename) {
    FILE *fp;
    Code_list *curr_code = table->code_list_head;
    Data_list *curr_data = table->data_list_head;
    char addr_str[5];
    char code_str[6];
    char data_str[6];
    char ic[5] , dc[5];
    fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening output file %s\n", filename);
        return;
    }
    int_to_base4_letters(table->IC , ic , 3);
    int_to_base4_letters(table->DC , dc , 2);
    fprintf(fp, "\t%s\t%s\n", ic, dc);    

    while (curr_code) {         int_to_base4_letters(curr_code->addr, addr_str, 4);
        int_to_base4_letters((int)(curr_code->code_word & 0x3FF), code_str, 5);
        fprintf(fp, "%s\t%s\n", addr_str, code_str);
        curr_code = curr_code->next;
    }

    while (curr_data) {         int_to_base4_letters(curr_data->addr, addr_str, 4);
        int_to_base4_letters((int)(curr_data->data_word & 0x3FF), data_str, 5);
        fprintf(fp, "%s\t%s\n", addr_str, data_str);
        curr_data = curr_data->next;
    }

    fclose(fp);
}


/* write_entries_base4
 * input : assembler table, output filename (e.g., "<name>.ent")
 * does  : for each entry node, writes:
 *           label <tab> 4-letter base-4 address
 * return: void
 */

void write_entries_base4(Assembler_Table *table, const char *filename) {
    FILE *fp;
    Entry_list *e = table->entry_list_head;
    char addr_str[5];

    fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening output file %s\n", filename);
        return;
    }

    while (e) {
        int_to_base4_letters(e->addr, addr_str, 4);
        fprintf(fp, "%s\t%s\n", e->label ,  addr_str);
        e = e->next;
    }

    fclose(fp);
}

/* write_externs_base4
 * input : assembler table, output filename (e.g., "<name>.ext")
 * does  : for each extern *usage* (addr != DEFINITION_ADDR), writes:
 *           label <tab> 4-letter base-4 address
 *         skips extern declarations with addr == DEFINITION_ADDR
 * return: void
 */

/* ps.ext: extern usages (addr, label); skip definition stubs (addr == DEFINITION_ADDR) */
void write_externs_base4(Assembler_Table *table, const char *filename) {
    FILE *fp;
    Extern_list *x = table->extern_list_head;
    char addr_str[5];

    fp = fopen(filename, "w");
    if (!fp) {
        printf("Error opening output file %s\n", filename);
        return;
    }

    while (x) {
       if (x->addr != DEFINITION_ADDR) {
            int_to_base4_letters(x->addr, addr_str, 4);
            fprintf(fp, "%s\t%s\n", x->label , addr_str);
        }
        x = x->next;
    }

    fclose(fp);
}

