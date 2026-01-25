#include "assembler.h"


int main(int argc , char * argv[]){

    int i;
    Assembler_Table * assembler_table;
    

    for(i = 1 ; i < argc ; i++){
        assembler_table = malloc(sizeof(Assembler_Table));
        if(assembler_table == NULL){
             exit(1); 
        }/*replace with safe malloc*/

        if(strlen(argv[i]) > MAX_LABEL_SIZE){
            puts("File name is too long");
            return 1;
        }

        strcpy(assembler_table->file_name , argv[i]);
        assembler_table->macro_head = NULL; /* Initialize macro list head to NULL */
        assembler_table->label_head = NULL; /* Initialize label list head to NULL */
        assembler_table->code_list_head = NULL; /* Initialize code list head to NULL */
        assembler_table->data_list_head = NULL; /* Initialize data list head to NULL */
        assembler_table->entry_list_head = NULL; /* Initialize entry list head to NULL */
        assembler_table->extern_list_head = NULL; /* Initialize extern list head to NULL */
        assembler_table->IC = 0;
        assembler_table->DC = 0;
        
        pre_proc(&assembler_table);
    
        /*free assembler table*/
        free_assembler_table(&assembler_table);
       

        
    }

    return 0;
}
