#include "assembler.h"


/*
 * first_pass
 * input : assembler table (IC/DC inside are updated), path to .am file
 * does  : scans the .am, builds code/data/labels/entry/extern lists, runs validations
 * return: void (prints errors and stops second pass if needed)
 */

void first_pass(Assembler_Table **assembler_table, char file_am[])
{
    char line[MAX_LINE_SIZE];

    FILE *fp_am;
    int IC = 100, DC = 0, line_counter = 0, label_length = 0; /*Initialize IC and DC as needed*/
    int word[4];
    Errors final_error = VALID; /* Initialize final error status */
    char *line_after_method;

    Action action_names[] = {
        {"mov", 0},
        {"cmp", 1},
        {"add", 2},
        {"sub", 3},
        {"lea", 4},
        {"clr", 5},
        {"not", 6},
        {"inc", 7},
        {"dec", 8},
        {"jmp", 9},
        {"bne", 10},
        {"jsr", 11},
        {"red", 12},
        {"prn", 13},
        {"rts", 14},
        {"stop", 15}};

    fp_am = fopen(file_am, "r");

    if (fp_am == NULL)
    {
        printf("Failed to open am file");
        exit(1);
    }


    while (fgets(line, sizeof(line), fp_am))
    {
        line_counter++;
        memset(word, 0, sizeof(word)); /*word[0] = opcode , word[1] method_src , word[2] method_dest , word[3] = A,R,E*/
        
        label_length = prosses_label(line, assembler_table, &IC, &DC, &line_counter);
        if (label_length == 1)
        {
            final_error = ERROR; /* If the label is invalid, set final error */
        }
        if(IC +DC > 255){
            printf("Exceed the maximum number of lines in a file\n");
            final_error = ERROR;
            break;
        }

        line_after_method = line + label_length; /* Move past the label */
        while (isspace(*line_after_method))
        {
            line_after_method++;
        }
        

        if (find_line_action(line_after_method, word, action_names) == 0)
        { /*found command*/
            validate_operand(action_names, word[0], line_after_method, line_counter, &final_error);

            process_commands_words(assembler_table, line_after_method, action_names, word, &IC);
        }
        else
        {
            validate_data(line_after_method, line_counter, &final_error);
            process_data_words(assembler_table, line_after_method, &IC, &DC);
        }

        label_length = 0;
    }

    (*assembler_table)->IC = IC - 100;
    (*assembler_table)->DC = DC;

    if (final_error == ERROR)
    {
        printf("First pass failed with errors.\n");
        return;
    }
    else
    {
        printf("First pass completed successfully.\n");
    }

    second_pass(*assembler_table, file_am); /* Call second pass to handle labels and addresses */

    fclose(fp_am);
}

/*
 * get_label_type
 * input : pointer right after label name (at ':' or space), out buffer 'type'
 * does  : writes "data" if next token is a directive ('.'), else "code"
 */

void get_label_type(char *label_end, char *type)
{

    while (*label_end == ':' || *label_end == ' ')
    {
        label_end++;
    }

    if (strncmp(label_end, ".", strlen(".")) == 0)
    {
        strcpy(type, "data");
    }
    else
    {
        strcpy(type, "code");
    }
}

/*
 * prosses_label
 * input : full line, assembler table, IC/DC pointers, line counter pointer
 * does  : if line starts with a label, validates it and adds to label list
 * return: number of chars that belong to the label (including ':'), or 0 if none,
 *         or 1 to signal an invalid label (so caller can flag error)
 */

int prosses_label(char *line, Assembler_Table **assembler_table, int *IC, int *DC, int *line_counter)
{
    char label_name[MAX_LINE_SIZE]; /*max line size for error checking */
    char type[MAX_LABEL_SIZE];
    char *label_end = strchr(line, ':'); /* Find the position of ':' in the line (label delimiter) */
    int label_length = 0;

    if (label_end != NULL)
    {
        label_length = label_end - line;
        strncpy(label_name, line, label_length);
        label_name[label_length] = '\0'; /* Null-terminate the string */
        if (examine_Label(label_name, line_counter) == ERROR)
        {
            return 1; /*If the label is invalid, return 1 */
        }

        get_label_type(label_end + 1, type); /* Get the type after the ':' */
        if (strcmp(type, "data") == 0)
        {
            add_label_list(&((*assembler_table)->label_head), label_name, *IC + *DC, type);
        }
        else
        {
            add_label_list(&((*assembler_table)->label_head), label_name, *IC, type);
        }

        label_length += 1;
    }
    return label_length; /* Return the length of the label including ':' */
}

/*
 * find_line_action
 * input : line (after label), out word[0] for opcode, action table
 * does  : matches mnemonic at start of line
 * return: 0 if found, 1 if not an action (probably a directive)
 */

int find_line_action(char *line, int *word, Action *action_names)
{
    int i = 0, j;

    char line_temp[MAX_LINE_SIZE];
    strcpy(line_temp, line);
    while (isspace(line[i]))
    {
        i++;
    }

    if (line == NULL || line[i] == '.')
    {
        return 1;
    }

    for (j = 0; j < 16; j++)
    {
        if (strncmp(line, action_names[j].name, strlen(action_names[j].name)) == 0)
        {
            word[0] = j; /* Store the action number */
            return 0;    /* */
        }
    }

    return 1;
}

/*
 * remove_newline
 * input : mutable string
 * does  : replaces first '\n' with '\0' if present
 */

void remove_newline(char *str)
{
    char *p = strchr(str, '\n');
    if (p)
        *p = '\0';
}

/*
 * parse_operands
 * input : line after mnemonic, out src/dest
 * does  : splits into up to two operands (uses strtok), trims newline
 */

void parse_operands(char *line_after_command, char *src, char *dest)
{
    char *token1 = strtok(line_after_command, " \t,");
    char *token2 = strtok(NULL, " \t\n");

    if (token1 && !token2)
    {
        /* Only one operand: destination */
        src[0] = '\0';
        strcpy(dest, token1);
        remove_newline(dest);
    }
    else
    {
        /* Two operands: source and destination */
        if (token1)
        {
            strcpy(src, token1);
            remove_newline(src);
        }
        else
            src[0] = '\0';
        if (token2)
        {
            strcpy(dest, token2);
            remove_newline(dest);
        }
        else
            dest[0] = '\0';
    }
}

/*
 * get_addr_method
 * input : operand text
 * does  : detects addressing method (0=#imm, 1=label, 2=mat [...], 3=register r0..r7)
 * return: method id
 */

int get_addr_method(char *operand)
{
    int i = 0;
    if (operand == NULL || strlen(operand) == 0)
    {
        return 0; /* empty */
    }
    while (isspace(operand[i]))
    {
        i++;
    }
    /* Immediate addressing: starts with '#' */
    if (operand[i] == '#')
    {
        return 0;
    }

    /* Register direct addressing: r0 to r7 */
    if (operand[i] == 'r' && isdigit((unsigned char)operand[i + 1]) &&
        (operand[i + 2] == '\0' || isspace((unsigned char)operand[i + 2]) || operand[i + 2] == ','))
    {
        int reg = operand[i + 1] - '0';
        if (reg >= 0 && reg <= 7)
        {
            return 3;
        }
    }

    /* mat addressing */
    if (strchr(operand, '[') != NULL && strchr(operand, ']') != NULL)
    {
        return 2;
    }

    /* Direct addressing: label */
    return 1;
}

/*
 * get_word_from_word
 * input : word[] = {opcode, src_method, dest_method, ARE/misc}
 * does  : packs into 16-bit instruction word (your bit layout)
 */

unsigned short get_word_from_word(int *word)
{
    unsigned short final_word = 0;
    final_word |= (word[0] & 0xF) << OPCODE_SHIFT;      /* Opcode */
    final_word |= (word[1] & 0x3) << SRC_ADDR_SHIFT;    /* Source addressing method */
    final_word |= (word[2] & 0x3) << DEST_ADDR_SHIFT;   /* Destination addressing method */
    final_word |= (word[3] & ARE_MASK);                 /* Additional bits (A, R, E) */
    return final_word;
}

/*
 * make_first_word
 * input : table, word[], src/dest strings, IC*
 * does  : computes and appends the first instruction word to code list, bumps IC
 */

void make_first_word(Assembler_Table **assembler_table, int *word, char *mod_src, char *mod_dest, int *IC)
{
    unsigned short final_word;

    word[1] = get_addr_method(mod_src);
    word[2] = get_addr_method(mod_dest);
    word[3] = 0;

    if (word[0] == rts || word[0] == stop)
    {                /* If the action is 'rts' or 'stop' */
        word[1] = 0; /* No source operand */
        word[2] = 0; /* No destination operand */
        word[3] = 0;
    }
    final_word = get_word_from_word(word);

    add_to_code_list(&((*assembler_table)->code_list_head), *IC, final_word, "");
    (*IC)++;
}

/*
 * handle_two_register_operands
 * input : table, src/dest (both registers), IC*
 * does  : packs two registers into single extra word, appends, bumps IC
 */

void handle_two_register_operands(Assembler_Table **assembler_table, char *src, char *dest, int *IC)
{
    int i = 0, j = 0;
    /* Skip leading whitespace for src*/
    while (src[i] && isspace((unsigned char)src[i]))
        i++;
    /* Find 'r' in src*/
    if (src[i] == 'r' && isdigit((unsigned char)src[i + 1]))
    {
        int src_num = src[i + 1] - '0';
        /* Skip leading whitespace for dest*/
        while (dest[j] && isspace((unsigned char)dest[j]))
            j++;
        /* Find 'r' in dest*/
        if (dest[j] == 'r' && isdigit((unsigned char)dest[j + 1]))
        {
            int dest_num = dest[j + 1] - '0';
            unsigned short reg_word = (src_num << shift_src) | (dest_num << shift_dest);

            add_to_code_list(&((*assembler_table)->code_list_head), *IC, reg_word, "");
            (*IC)++;
        }
    }
}

/*
 * process_commands_words
 * input : table, full line, action table, word[], IC*
 * does  : builds first word, then extra words for operands, adds to code list
 */

void process_commands_words(Assembler_Table **assembler_table, char line[], Action action_names[], int word[], int *IC)
{
    char line_after_command[MAX_LINE_SIZE];
    char mod_src[MAX_LABEL_SIZE], mod_dest[MAX_LABEL_SIZE];
    int i = 0;
    int src_is_reg, dest_is_reg;

    while (isspace(line[i]))
    {
        i++;
    }
    strcpy(line_after_command, line + strlen(action_names[word[0]].name) + i);
    parse_operands(line_after_command, mod_src, mod_dest);         /* Parse the operands from the line after the command*/
    make_first_word(assembler_table, word, mod_src, mod_dest, IC); /* create the first word */

    src_is_reg = get_addr_method(mod_src);
    dest_is_reg = get_addr_method(mod_dest);

    if ((src_is_reg == 3) && (dest_is_reg == 3))
    { /* Check if both operands are registers */
        handle_two_register_operands(assembler_table, mod_src, mod_dest, IC);
        return; /* Exit the function after handling both registers*/
    }

    convert_operand_to_code(assembler_table, mod_src, IC, SRC);  /* handel source operand if exist*/
    convert_operand_to_code(assembler_table, mod_dest, IC, DEST); /* handel destination operand if exist*/

    /*Maybe free at end mod_src , mod_dest*/
}

/*
 * convert_operand_to_code
 * input : table, single operand, IC*, mode (0=src, 1=dest)
 * does  : appends proper extra word(s) for this operand (mat/register/immediate/label), bumps IC
 */

void convert_operand_to_code(Assembler_Table **assembler_table, char *operand, int *IC, int mode)
{
    const char *registers[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
    char matrix_name[MAX_LABEL_SIZE];
    char reg1[4], reg2[4];
    int r1, r2, j, value, i = 0;
    unsigned short matrix_word, value_word, reg_num;

    /* Skip leading whitespace (spaces, tabs, etc.) */
    while (operand && isspace((unsigned char)operand[i]))
    {
        i++;
    }
    if (operand == NULL || strlen(operand + i) == 0)
    {
        return; /* No operand to process */
    }

    /* --- MATRIX --- */
    if (strchr(operand, '[') && strchr(operand, ']'))
    {
        memset(matrix_name, '\0', sizeof(matrix_name));
        memset(reg1, '\0', sizeof(reg1));
        memset(reg2, '\0', sizeof(reg2));
        r1 = 0;
        r2 = 0;

        sscanf(operand, "%[^[][%2[^]]][%2[^]]", matrix_name, reg1, reg2);
        for (j = 0; j < 8; j++)
        {
            if (strcmp(reg1, registers[j]) == 0)
                r1 = j;
            if (strcmp(reg2, registers[j]) == 0)
                r2 = j;
        }

        /* First: matrix address (label stored for second pass) */
        add_to_code_list(&((*assembler_table)->code_list_head), *IC, 0, matrix_name);
        (*IC)++;
        /* Second: registers packed into word */
        matrix_word = (unsigned short)((r1 & 0x7) << shift_src | (r2 & 0x7) << shift_dest);
        add_to_code_list(&((*assembler_table)->code_list_head), *IC, matrix_word, "");
        (*IC)++;
    }

    /* --- IMMEDIATE VALUE --- */
    else if (operand[i] == '#')
    {
        value = atoi(operand + i + 1); /* Convert string to integer */

        value_word = (unsigned short)((value & 0x3FF) << shift_dest);
        add_to_code_list(&((*assembler_table)->code_list_head), *IC, value_word, "");
        (*IC)++;
    }

    /* --- REGISTER DIRECT --- */
    else
    {
        if (get_addr_method(operand + i) == 3)
        {
            reg_num = operand[i + 1] - '0';

            if (mode == SRC)
                reg_num = (unsigned short)((operand[i + 1] & 0x7) << shift_src);
            else
            {
                reg_num = (unsigned short)((operand[i + 1] & 0x7) << shift_dest);
            };
            add_to_code_list(&((*assembler_table)->code_list_head), *IC, reg_num, "");
            (*IC)++;
        }

        /* --- DIRECT (LABEL or SYMBOL) --- */
        else
        {
            /* If it's a label, add it to the code list with address 0 for now */
            /* This will be resolved in the second pass */
            add_to_code_list(&((*assembler_table)->code_list_head), *IC, 0, operand); /* store label name */
            (*IC)++;
        }
    }
}

/*
 * process_data_words
 * input : table, line after directive, IC*, DC*
 * does  : handles .string / .data / .mat / .entry / .extern and appends to data/entry/extern lists
 */

void process_data_words(Assembler_Table **assembler_table, char *line_after_method, int *IC, int *DC)
{
    char *token;
    unsigned short data_word;
    int addr;

    /* Skip leading spaces */
    while (isspace((unsigned char)*line_after_method))
        line_after_method++;

    /* --- Handle .string --- */
    if (strncmp(line_after_method, ".string", 7) == 0)
    {
        char *str = strchr(line_after_method, '"');
        if (str != NULL)
        {
            str++; /* Skip first " */
            while (*str && *str != '"')
            {
                data_word = ((unsigned short)(*str)) & 0x3FF; /* Only 10 bits */
                addr = *IC + *DC;
                add_to_data_list(&((*assembler_table)->data_list_head), addr, data_word);
                (*DC)++;
                str++;
            }
            /* Add null terminator */
            data_word = 0;
            addr = *IC + *DC;
            add_to_data_list(&((*assembler_table)->data_list_head), addr, data_word);
            (*DC)++;
        }
    }

    /* --- Handle .data --- */
    else if (strncmp(line_after_method, ".data", 5) == 0)
    {
        token = strtok(line_after_method + 5, ", \t\n");
        while (token != NULL)
        {
            int value = atoi(token);
            data_word = ((unsigned short)(value & 0x3FF)); /* 10-bit */
            addr = *IC + *DC;
            add_to_data_list(&((*assembler_table)->data_list_head), addr, data_word);
            (*DC)++;
            token = strtok(NULL, ", \t\n");
        }
    }

    /* --- Handle .mat --- */
    else if (strncmp(line_after_method, ".mat", 4) == 0)
    {
        /* Skip matrix size e.g. [2][2] */
        char *data_start = strchr(line_after_method, ']');
        if (data_start != NULL)
        {
            data_start = strchr(data_start + 1, ']');
            if (data_start != NULL)
                data_start++;
        }
        token = strtok(data_start, ", \t\n");
        while (token != NULL)
        {
            int value = atoi(token);
            data_word = ((unsigned short)(value & 0x3FF)); /* 10-bit */
            addr = *IC + *DC;
            add_to_data_list(&((*assembler_table)->data_list_head), addr, data_word);
            (*DC)++;
            token = strtok(NULL, ", \t\n");
        }
    }
    else if (strncmp(line_after_method, ".entry", strlen(".entry")) == 0)
    {
        token = strtok(line_after_method + strlen(".entry"), "\t\n");
        if (token == NULL)
        {
            token = "empty_entry"; /* Default label if none provided */
        }
        add_entry_to_list(&(*assembler_table)->entry_list_head, token, 0);
    }
    else if (strncmp(line_after_method, ".extern", strlen(".extern")) == 0)
    {

        token = strtok(line_after_method + strlen(".extern"), "\t\n");
        if (token == NULL)
        {
            token = "empty_extern"; /* Default label if none provided */
        }
        add_extern_to_list(&(*assembler_table)->extern_list_head, token, DEFINITION_ADDR);
    }
}
