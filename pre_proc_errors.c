#include "assembler.h"


/*
 * validate_mcroend
 * checks that nothing extra appears after "mcroend"
 * return: VALID / ERROR
 */

Errors validate_mcroend(char after_mcroend[] , int line_counter){
    if(after_mcroend[0] != '\n' && after_mcroend[0] != '\0'){
        printf(ERR_EXTRA_TEXT_AFTER_MCROEND_MSG "\n", line_counter);
        return ERROR;
    }
    return VALID;
}


/*
 * validate_macro_name
 * does basic checks for macro name rules. prints an error on fail.
 * return: VALID / ERROR
 */


Errors validate_macro_name(Assembler_Table ** assembler , int line_counter , char macro_name[]){
    int i = 0;
    printf("macro name: -%s- \n",macro_name);
    
    if(macro_name == NULL){
        printf(ERR_MACRO_EMPTY_MSG "\n", line_counter);
        return ERROR;
    }

    if(strlen(macro_name) > MAX_LABEL_SIZE){
        printf(ERR_MACRO_TOO_LONG_MSG "\n", line_counter, MAX_LABEL_SIZE);
        return ERROR;
    }

    if(!isalpha(macro_name[0])){
        printf(ERR_MACRO_FIRST_CHAR_MSG "\n", line_counter);
        return ERROR;
    }


    for(i = 1 ; i < strlen(macro_name) ; i++){
        if(!isalpha(macro_name[i]) && !isdigit(macro_name[i]) && macro_name[i] != '_'){
            printf(ERR_MACRO_INVALID_CHAR_MSG "\n", line_counter);
            return ERROR;
        }

    }
    
    if(find_macro((*assembler)->macro_head , macro_name) == 1){
        printf(ERR_MACRO_ALREADY_DEFINED_MSG "\n", line_counter);
        return ERROR;
    }

    if(is_reserved_word(macro_name) == 1){
        printf(ERR_MACRO_RESERVED_WORD_MSG "\n", line_counter);
        return ERROR;
    }


    return VALID;
}

/*
 * find_macro
 * returns 1 if a macro by this name exists, else 0
 */

int find_macro(Macro_List * mc_head , char name[]){

    while(mc_head != NULL){

        if(strcmp(mc_head->name , name) == 0){
            return 1;
        }

        mc_head =  mc_head->next;
    }

    return 0;
}



/*
 * is_reserved_word
 * returns 1 if name is reserved (op, directive, register, mcro/mcroend), else 0
 */

int is_reserved_word(const char *name)
{
    char *const RESERVED_WORDS[] = {
        /* actions */
        "mov","cmp","add","sub","not","clr","lea","inc",
        "dec","jmp","bne","red","prn","jsr","rts","stop",

        /* directives */
        ".data",".string",".mat",".entry",".extern",

        /* registers */
        "r0","r1","r2","r3","r4","r5","r6","r7",

        /* macro keywords */
        "mcro","mcroend"
    };

    int i;
    for (i = 0; i < RESERVED_WORDS_COUNT; i++) {
        if (strcmp(name, RESERVED_WORDS[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * check_line_too_long_and_flush
 * detects an overlong source line, prints error with line number,
 * and flushes the rest of that line from fp so the next read is clean.
 * return: VALID if ok, ERROR if it flushed an overlong line
 */

Errors check_line_too_long_and_flush(FILE *fp, const char *line, int line_no)
{
    size_t len;
    int ch;

    if (line == NULL) return ERROR;

    len = strlen(line);

    /* If fgets() did not read a newline and we’re not at EOF, the line was longer than the buffer */
    if (len > 0 && line[len - 1] != '\n' && !feof(fp)) {
    
        printf(ERR_LINE_TOO_LONG "\n", line_no, MAX_LINE_SIZE - 2); 
    
        /* Flush the remainder of this line */
        do {
            ch = fgetc(fp);
        } while (ch != '\n' && ch != EOF);

        return ERROR;
    }

    return VALID;
}


