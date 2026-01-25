#include "assembler.h"

/*
 * pre_proc
 * input:  assembler table (file_name must be set, e.g. "ps")
 * does:   runs the pre-processor. writes .am file, or deletes it on errors.
 * return: 0 on ok, ERROR on failure.
 */

int pre_proc(Assembler_Table **assembler_table)
{
    char file_as[MAX_SYMBOL_SIZE], file_am[MAX_SYMBOL_SIZE], line[MAX_LINE_SIZE], macro_name_copy[MAX_SYMBOL_SIZE];
    FILE *fp_as, *fp_am;
    int line_count = 0;
    Content_List *head_content;
    Errors errors_in_line = VALID;
    Errors final_error = VALID;
    memset(file_as, '\0', MAX_SYMBOL_SIZE);
    memset(file_am, '\0', MAX_SYMBOL_SIZE);

    strcpy(file_as, (*assembler_table)->file_name);
    strcat(file_as, ".as");

    strcpy(file_am, (*assembler_table)->file_name);
    strcat(file_am, ".am");

    fp_as = fopen(file_as, "r");

    if ( fp_as == NULL)
    {
        printf("Error in open file\n");
        exit(1);
    }

    fp_am = fopen(file_am, "w");

    if (fp_am == NULL)
    {
        printf("Error in open file\n");
        exit(1);
    }

    while (fgets(line, MAX_LINE_SIZE, fp_as) != NULL)
    {
        line_count++;

        /* detect & handle overlong line; skip processing this iteration */
        if (check_line_too_long_and_flush(fp_as, line, line_count) == ERROR)
        {
            final_error = ERROR;
            continue; /* buffer is clean, move to next source line */
        }

        errors_in_line = VALID;
        /*trim line white spaces*/
        remove_spaces_and_tabs(line);
        if (line[0] == ';' || line[0] == '\n')
        {
            continue;
        }

        if (strncmp(line, "mcro", strlen("mcro")) == 0)
        {
            /*if line is macro*/
            char *macro_name = strtok(line + strlen("mcro"), " \t\n");
            if ((errors_in_line = validate_macro_name(assembler_table, line_count, macro_name)) == ERROR)
            {
                final_error = ERROR;
            }

            if (macro_name == NULL)
            {
                /*error*/
            }

            /* Make a copy of macro_name before strtok or any further modification */
            if (errors_in_line == VALID)
            {
                strncpy(macro_name_copy, macro_name, MAX_SYMBOL_SIZE - 1);
                macro_name_copy[MAX_SYMBOL_SIZE - 1] = '\0';
            }

            head_content = NULL;
            while (fgets(line, MAX_LINE_SIZE, fp_as) != NULL)
            {
                line_count++;
                remove_spaces_and_tabs(line);
                if (strncmp(line, "mcroend", strlen("mcroend")) == 0)
                {
                    errors_in_line = validate_mcroend(line + strlen("mcroend"), line_count);
                    if (errors_in_line == ERROR)
                    {
                        final_error = ERROR;
                    }
                    break;
                }
                if (errors_in_line == VALID)
                {
                    add_to_content_list(line, &head_content);
                }
            }
            if (errors_in_line == VALID)
            {
                add_to_macro_list(macro_name_copy, head_content, &(*assembler_table)->macro_head);
            }

            macro_name = NULL;
        }
        else if (find_if_macro(line, &(*assembler_table)->macro_head, fp_am) != 1)
        {
            /*fprintf  content*/

            continue;
        }
        else
        {
            /*if not macro so write to am file*/
            fprintf(fp_am, "%s", line);
        }

        /*if mcro so while(that end in macro end)*/
    }

    fclose(fp_as);
    fclose(fp_am);

    if (final_error == ERROR)
    {
        if (remove(file_am) != 0)
        {
            perror("Error deleting file");
        }
        return ERROR;
    }

    first_pass(assembler_table, file_am);

    return 0;
}

/*
 * add_to_content_list
 * input:  a line and the head of a macro content list
 * does:   pushes the line to the end of the list (as-is)
 */

void add_to_content_list(char line[], Content_List **head_content)
{
    Content_List *new_node = (malloc(sizeof(Content_List)));
    Content_List *tmp = NULL;

    if (new_node == NULL)
    {
        printf("Error in Malloc\n");
        exit(1);
    }
    strcpy(new_node->line, line);
    new_node->next = NULL;

    if (*head_content == NULL)
    {
        *head_content = new_node;
        return;
    }

    tmp = *head_content;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

/*
 * add_to_macro_list
 * input:  macro name, its content list head, and the macros head
 * does:   creates a macro node and appends it to the macro list
 */

void add_to_macro_list(char *macro_name, Content_List *head_content, Macro_List **head_macro)
{
    Macro_List *new_node = (malloc(sizeof(Macro_List)));
    Macro_List *tmp = NULL;
    if (new_node == NULL)
    {
        printf("Error in Malloc\n");
        exit(1);
    }

    strcpy(new_node->name, macro_name);

    new_node->content_head = head_content;
    new_node->next = NULL;
    if (*head_macro == NULL)
    {
        *head_macro = new_node;
        return;
    }
    tmp = *head_macro;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = new_node;
}

/*
 * find_if_macro
 * input:  a line, macro list head, and the open .am FILE*
 * does:   if the line is a macro call, writes its content to .am and returns 0
 * return: 0 if macro expanded, 1 otherwise
 */

int find_if_macro(char *line, Macro_List **head_macro, FILE *fp_am)
{
    Macro_List *tmp;
    if (*head_macro == NULL)
    {
        return 1;
    }

    tmp = *head_macro;

    while (tmp != NULL)
    {
        if (strncmp(line, tmp->name, strlen(tmp->name)) == 0)
        {
            Content_List *content = tmp->content_head;
            while (content != NULL)
            {
                fprintf(fp_am, "%s", content->line);
                content = content->next;
            }
            return 0;
        }
        tmp = tmp->next;
    }

    return 1;
}

/*
 * remove_spaces_and_tabs
 * input:  a line (mutable)
 * does:   removes spaces/tabs outside quotes, keeps text inside "..."
 */
void remove_spaces_and_tabs(char *str)
{
    char *src = str;
    char *dst = str;
    int in_quotes = 0; /* Flag to track if we are inside "..." to make sure it won't remove spaces inside quotes */

    while (*src != '\0')
    {
        if (*src == '"')
        {
            in_quotes = !in_quotes; /* Toggle state when seeing a quote */
            *dst++ = *src++;
        }
        else if (!in_quotes && (*src == ' ' || *src == '\t'))
        {
            /* Skip space/tab when outside quotes */
            src++;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst = '\0'; /* Null-terminate the result */
}
