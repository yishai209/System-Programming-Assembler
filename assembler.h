#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#define MAX_SYMBOL_SIZE 32 
#define MAX_LINE_SIZE 82 /*include /n*/
#define MAX_LABEL_SIZE 32 
#define shift_src 6  /* Shift for source operand */
#define shift_dest 2 /* Shift for destination operand */
#define rts 14 /* RTS action code */
#define stop 15 /* STOP action code */
#define DEFINITION_ADDR -1
#define OPCODE_SHIFT 6
#define SRC_ADDR_SHIFT 4
#define DEST_ADDR_SHIFT 2
#define ARE_MASK 0x3
#define RESERVED_WORDS_COUNT ((int)(sizeof(RESERVED_WORDS)/sizeof(RESERVED_WORDS[0])))
#define SRC 0
#define DEST 1


/* ===== Error message defines ===== */
#define ERR_MACRO_EMPTY_MSG           "Line %d: Macro name is empty"
#define ERR_MACRO_TOO_LONG_MSG         "Line %d: Macro name is too long (max %d characters)"
#define ERR_MACRO_FIRST_CHAR_MSG     "Line %d: First character of macro name must be a letter"
#define ERR_MACRO_INVALID_CHAR_MSG    "Line %d: Macro name contains invalid character"
#define ERR_MACRO_ALREADY_DEFINED_MSG  "Line %d: Macro already defined"
#define ERR_MACRO_RESERVED_WORD_MSG    "Line %d: Macro name cannot be a reserved word"
#define ERR_EXTRA_TEXT_AFTER_MCROEND_MSG  "Line %d: Extra text after 'mcroend' is not allowed"
#define ERR_LINE_TOO_LONG           "Line %d too long (max %d characters)."
#define MISSING_LABEL    "Line %d: Missing label"
#define INVALID_LABEL         "Line %d: Invalid label"
#define CANNOT_BE_RESERVED_WORD     "Line %d: Label cannot be a reserved word"
#define LABEL_TOO_LONG     "Line %d: Label is too long "
#define ERR_INVALID_OPERAND     "Line %d: unknown opcode %d.\n"
#define FULL_ERR_MESSAGE     "Line %d: \"%s\" expects %d operand%s, got %d.\n"
#define STRING_START_ERR     "Line %d: .string must start with a '\"'.\n"
#define STRING_END_ERR     "Line %d: .string missing closing '\"'.\n"
#define NON_PRINTABLE_CHAR_ERR "Line %d: .string contains non-printable character.\n"
#define EXTRA_CHAR_IN_LINE      "Line %d: extra characters after closing '\"' in .string.\n"
#define EMPLY_DATA_ERR     "Line %d: .data requires at least one number.\n"
#define MISSING_DATA      "Line %d: empty item between commas in .data.\n"
#define INVALIDE_DATA      "Line %d: invalid .data item \"%s\" (expect +/-digits, range -512..511).\n"
#define TRAILING_DATA_ERR    "Line %d: trailing comma in .data.\n"
#define FIRST_DIN_ERR     "Line %d: .mat invalid first dimension.\n"
#define SECOND_DIM_ERR     "Line %d: .mat invalid second dimension.\n"
#define NO_VALUE_MAT_ERR     "Line %d: .mat requires values after dimensions.\n" 
#define TRAILING_MAT_ERR    "Line %d: trailing comma (,) in .mat values.\n" /*problem*/
#define EMPTY_INSIDE_COMMA    "Line %d: empty item between commas in .mat.\n" 
#define INVALID_MAT_DATA      "Line %d: invalid .mat item \"%s\" (expect +/-digits, range -512..511).\n"
#define MAT_SIZE_DATA_MIS_MATCH_ERR    "Line %d: .mat values count (%d) does not match %d x %d.\n"
#define MAT_EMPTY_OPERAND_ERR       "Line %d : empty matrix operand.\n"
#define MAT_MISSING_FIRST_LB_ERR     "Line %d : matrix must be LABEL[rX][rY] (missing first '[').\n"
#define MAT_NO_LABEL_BEFORE_LB_ERR    "Line %d : matrix operand must start with a label before '['.\n"
#define MAT_MISSING_FIRST_RB_ERR   "Line %d : missing ']' for first index in matrix operand.\n"
#define MAT_FIRST_IDX_NEED_REG_ERR    "Line %d : first index must be a register r0..r7.\n"
#define MAT_FIRST_IDX_NEED_DIGIT_ERR  "Line %d : first index must be r0..r7.\n"
#define MAT_FIRST_IDX_RANGE_ERR  "Line %d : first index register out of range (r%d).\n"
#define MAT_FIRST_IDX_SINGLE_ERR  "Line %d : only a single register allowed inside first [].\n"
#define MAT_MISSING_SECOND_LB_ERR     "Line %d : matrix must be LABEL[rX][rY] (missing second '[').\n"
#define MAT_MISSING_SECOND_RB_ERR    "Line %d : missing ']' for second index of matrix operand.\n"
#define MAT_SECOND_IDX_NEED_REG_ERR   "Line %d : second index must be a register r0..r7.\n"
#define MAT_SECOND_IDX_NEED_DIGIT_ERR "Line %d : second index must be r0..r7.\n"
#define MAT_SECOND_IDX_RANGE_ERR    "Line %d : second index register out of range (r%d).\n"
#define MAT_SECOND_IDX_SINGLE_ERR "Line %d : only a single register allowed inside second [].\n"
#define MAT_EXTRA_AFTER_ERR   "Line %d : extra characters after matrix operand.\n"
#define ERR_DUP_LABEL_DEF   "Error: label \"%s\" defined more than once (addresses %d and %d).\n"
#define ERR_ENTRY_NOT_DEFINED   "Error: .entry label \"%s\" not defined.\n"
#define ERR_ENTRY_EXTERN_CONFLICT    "Error: label \"%s\" cannot be both .entry and .extern.\n"
#define ERR_EXTERN_DEFINED_LOCAL     "Error: .extern label \"%s\" is also defined locally (addr %d).\n"
#define ERR_UNDEFINED_SYMBOL    "Error: undefined symbol \"%s\" referenced at address %d.\n"
#define INVALID_NUM_RANGE_ERR "Line %d: invalid number range, expected -128 to 127.\n"
#define INVALID_NUM_FORMAT_ERR "Line %d: invalid number format, expected integer.\n"

typedef enum ErrorsEnum {
    VALID = 0,
    ERROR = 1
} Errors;

typedef struct {
    char name[MAX_LABEL_SIZE];
    int code;
} Action;

typedef struct Code_list{
    int addr; 
    unsigned short code_word; 
    char label_name[MAX_LABEL_SIZE];
    struct Code_list * next; /* Pointer to the next object code */
    
}Code_list;

typedef struct Data_list {
    int addr;
    unsigned short data_word;
    struct Data_list *next;
} Data_list;

    

typedef struct Content_List{
    char line[MAX_LINE_SIZE];/* Line of macro content */
    struct Content_List * next;/* Pointer to the next line */
}Content_List;


typedef struct Macro_List{
    char name [MAX_SYMBOL_SIZE];/* Name of the macro */
    struct Content_List * content_head;/* Head of macro content */
    struct Macro_List * next; /* Pointer to the next macro */
}Macro_List;



typedef struct LABEL_LIST{
    char label[MAX_LABEL_SIZE]; /* Label name */
    int addr;  /* Address of the label */
    char type[MAX_LABEL_SIZE]; /* Type of the label (e.g., data, code) */
    struct LABEL_LIST * next; /* Pointer to the next label */
} LABEL_LIST;

typedef struct Entry_list{
    char label[MAX_LABEL_SIZE]; /* Label name */
    int addr;  /* Address of the label */
    struct Entry_list * next; /* Pointer to the next label */
} Entry_list;


typedef struct Extern_list{
    char label[MAX_LABEL_SIZE]; /* Label name */
    int  addr;  /* Address of the label */ 
    struct Extern_list * next; /* Pointer to the next label */
} Extern_list;




typedef struct Assembler_Table{
    struct Macro_List * macro_head; /* Head of the macro list */
    struct LABEL_LIST * label_head; /* Head of the label list */
    struct Code_list * code_list_head; /* Head of the code list */
    struct Data_list * data_list_head; /* Head of the data list */
    struct Extern_list * extern_list_head;
    struct Entry_list * entry_list_head;
    char file_name[MAX_SYMBOL_SIZE];
    int IC;
    int DC;
}Assembler_Table;






int pre_proc(Assembler_Table ** assembler_table );
/* --- Helper declarations for preprocessor--- */

void add_to_content_list(char line[] , Content_List **head_content);
int find_if_macro(char * line,Macro_List ** head_macro, FILE * fp_am);
void add_to_macro_list( char * macro_name , Content_List * head_content , Macro_List ** head_macro);
void remove_spaces_and_tabs(char *line);

/*Declarations for preprocessor errors*/
Errors validate_macro_name(Assembler_Table ** assembler , int line_counter , char macro_name[]);
int find_macro(Macro_List * mc_head , char name[]);
int is_reserved_word(const char *name);
Errors validate_mcroend(char after_mcroend[] , int line_counter);
Errors check_line_too_long_and_flush(FILE *fp, const char *line, int line_no);





void first_pass(Assembler_Table **assembler_table,  char file_am []);
/* --- Helper declarations for first pass--- */

void add_label_list(LABEL_LIST ** label_head, char * label_name, int IC , char type []);
void add_to_code_list(Code_list **head, int addr, unsigned short code_word, char *label_name);
void add_to_data_list(Data_list **head, int addr, unsigned short data_word);
void convert_operand_to_code(Assembler_Table **assembler_table , char *operand, int *IC, int mode);
void get_label_type(char *label_end, char *type);
void parse_operands(char *line, char *src, char *dest);
void process_commands_words(Assembler_Table **assembler_table, char line[] ,Action  action_names[] , int word[],int *IC);
void make_first_word( Assembler_Table **assembler_table, int *word,char *mod_src,char *mod_dest ,int *IC);
int find_line_action(char *line, int *word, Action *action_names);
unsigned short get_word_from_word(int *word);
int get_addr_method(char *operand);
void handle_two_register_operands(Assembler_Table **assembler_table, char *src, char *dest, int *IC);
void process_data_words(Assembler_Table **assembler_table, char *line, int *IC, int *DC);
void add_extern_to_list(Extern_list **head, char *label, int addr);  
void add_entry_to_list(Entry_list **head, char *label, int addr);
int prosses_label(char *line, Assembler_Table **assembler_table, int *IC, int *DC,int *line_counter) ;
void get_label_type(char *label_end, char *type);

/*Declarations for first_pass errors*/
Errors examine_Label(char *label, int *line_counter );
Errors validate_data(const char *line_after_method, int line_no, Errors *final_error);
Errors validate_operand(const Action *actions,int opcode,const char *line_after_method,int line_no,Errors *final_error);
Errors validate_string_directive(const char *p, int line_no, Errors *final_error);
Errors validate_data_list(const char *p, int line_no, Errors *final_error);
Errors validate_mat_directive(const char *p, int line_no, Errors *final_error);
void trim_ends(char *s);
int is_int10(const char *tok, long *out);
int parse_dim(const char *p, int *out, const char **after_bracket);
int has_trailing_comma(const char *s);
int has_text(const char *s);
Errors validate_entry(const char *p, int line_counter, Errors *final_error);
Errors validate_extern(const char *p, int line_counter, Errors *final_error);
Errors handle_matrix_operand(char *operand, int line_no);
Errors handle_operand_erorrs(int opcode, char *src, char *dest, int line_no, Errors *final_error, int expected);
           





void second_pass(Assembler_Table *table,  char file_am []);
/* --- Helper declarations for second pass--- */

LABEL_LIST* find_label(LABEL_LIST *head,  char *label);
int is_entry_label(Entry_list *entry_head, char *label);
int is_extern_label(Extern_list *extern_head, char *label);
void update_code_for_entry(Code_list *node, int addr);
void update_code_for_extern(Code_list *node);
void add_extern_usage(Assembler_Table *table,  char *label, int addr);
void update_entry_addresses(Entry_list *entry_head, LABEL_LIST *label_head);

/*Declarations for second_pass errors*/
Errors check_duplicate_labels(LABEL_LIST *labels, Errors *final_error);
Errors check_entry_extern_conflicts(Entry_list *entries, Extern_list *externs, Errors *final_error);
Errors check_entry_defined(Entry_list *entries, LABEL_LIST *labels, Errors *final_error);
Errors check_extern_not_defined(Extern_list *externs, LABEL_LIST *labels, Errors *final_error);
Errors check_undefined_operands(Code_list *code, LABEL_LIST *labels, Extern_list *externs, Errors *final_error);
Errors run_second_pass_errors(Assembler_Table *table, Errors *final_error);
int name_equals(const char *a, const char *b);
int label_exists(LABEL_LIST *labels, const char *name, int *out_addr, const char **out_type);
int is_declared_extern(Extern_list *exts, const char *name);





void translation_unit(Assembler_Table *table);
/* --- Helper declarations for translation--- */

void int_to_base4_letters(int value, char *out, int width);
void write_assembler_base4(Assembler_Table *table, const char *filename);
void write_extern_file(Assembler_Table *table, const char *filename);
void write_entry_file(Assembler_Table *table, const char *filename);
void write_code_file(Assembler_Table *table, const char *filename);
void write_data_file(Assembler_Table *table, const char *filename);
void write_externs_base4(Assembler_Table *table, const char *filename);
void write_entries_base4(Assembler_Table *table, const char *filename);


/*free_table*/
void free_assembler_table(Assembler_Table **ptable);

