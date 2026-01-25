#include "assembler.h"


/*
 * examine_Label
 * input : label text, pointer to line counter
 * does  : validates label (non-empty, starts with letter, <= MAX_LABEL_SIZE-1,
 *         alnum only, not reserved)
 * return: VALID / ERROR (prints error with line number on failure)
 */

Errors examine_Label(char *label, int *line_counter)
{
    size_t len;
    int i;

    if (label == NULL || label[0] == '\0')
    {
        printf(MISSING_LABEL "\n", *line_counter);
        return ERROR;
    }

    if (!isalpha((unsigned char)label[0]))
    {
        printf(INVALID_LABEL "\n", *line_counter);
        return ERROR;
    }

    len = strlen(label);
    /* if MAX_LABEL_SIZE is the buffer size, max valid length is MAX_LABEL_SIZE-1 */
    if (len > (size_t)(MAX_LABEL_SIZE - 1))
    {
        printf(LABEL_TOO_LONG "\n", *line_counter);
        return ERROR;
    }

    for (i = 0; i < (int)len; i++)
    {
        if (!isalnum((unsigned char)label[i]))
        {
            printf(INVALID_LABEL "\n", *line_counter);
            return ERROR;
        }
    }

    if (is_reserved_word(label))
    {
        printf(CANNOT_BE_RESERVED_WORD "\n", *line_counter);
        return ERROR;
    }

    return VALID;
}

/*
 * check_if_legal_operand_for_command
 * input : opcode (0..15), method_type (0=#,1=label,2=mat,3=reg), mode_op (0=src,1=dest), line_no
 * does  : enforces which addressing kinds are allowed per opcode & operand side
 * return: VALID / ERROR (prints error on failure)
 */

Errors check_if_legal_operand_for_command(int opcode, int method_type, int mode_op, int line_no)
{
    if (mode_op == SRC)
    {
        switch (opcode)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            return VALID;
        case 4:
            if (method_type != 2 && method_type != 1)
            {
                printf("Ilegal type of operand");
                return ERROR;
            }
            break;
        }
    }

    if (mode_op == DEST)
    {
        switch (opcode)
        {
        case 1:
        case 13:
            return VALID;
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            if (method_type == 0)
            {
                printf("Ilegal type of operand");
                return ERROR;
            }
        }
    }

    return VALID;
}

/*
 * handle_number_oeprand_error
 * input : numeric text (without leading '#'), line_no
 * does  : checks sign and digits only; verifies signed 8-bit range [-128..127]
 * return: VALID / ERROR
 */

Errors handle_number_oeprand_error(char * operand, int line_no){
    int num_op,i=0;
    
    if(operand[0] == '+' || operand[0] == '-'){
        i++;
    }

    while(i < strlen(operand)){
        if(!isdigit(operand[i])){
            printf(INVALID_NUM_FORMAT_ERR "\n", line_no);
            return ERROR;
        }
        i++;
    }

     num_op = atoi(operand);
    if(num_op < -128 || num_op > 127){ /* Check if the number is within the valid range for a 8-bit signed integer*/
        printf(INVALID_NUM_RANGE_ERR "\n",line_no);
        return ERROR;
    }

    return VALID;
}

/*
 * handle_each_operand
 * input : opcode, operand text, line_no, mode_op (0=src,1=dest), final_error*
 * does  : detects addressing method, runs specific validators for that kind
 * return: VALID / ERROR (updates *final_error on error)
 */

Errors handle_each_operand(int opcode, char *operand, int line_no, int mode_op, Errors *final_error)
{
    int method_type = get_addr_method(operand);
    if (check_if_legal_operand_for_command(opcode, method_type, mode_op, line_no) == ERROR)
    {
        *final_error = ERROR;
        return ERROR;
    }
    switch (method_type)
    {
    case 0:
        if (handle_number_oeprand_error(operand + 1, line_no) == ERROR)
        {
            *final_error = ERROR;
            return ERROR;
        }
        break;
    case 1:
        if (examine_Label(operand, &line_no) == ERROR)
        {
            *final_error = ERROR;
            return ERROR;
        }
        break;
    case 2:
        if (handle_matrix_operand(operand,line_no) == ERROR)
        {
            *final_error = ERROR;
            return ERROR;
        }
        break;
    case 3:
        break;
    }

    return VALID;
}

/*
 * handle_matrix_operand
 * input : operand text like LABEL[rX][rY], line_no
 * does  : validates structure and that both indices are registers r0..r7 only
 * return: VALID / ERROR
 */

Errors handle_matrix_operand(char *operand, int line_no)
{
    const char *text;
    const char *open1, *close1, *open2, *close2;
    const char *scan;
    int reg_num;

    if (operand == NULL) {
        printf(MAT_EMPTY_OPERAND_ERR "\n", line_no);
        return ERROR;
    }

    /* skip leading spaces */
    text = operand;
    while (*text && isspace((unsigned char)*text)) text++;

    /* find first [ ... ] */
    open1 = strchr(text, '[');
    if (open1 == NULL) {
        printf(MAT_MISSING_FIRST_LB_ERR "\n", line_no);
        return ERROR;
    }
    if (open1 == text) {
        printf(MAT_NO_LABEL_BEFORE_LB_ERR "\n", line_no);
        return ERROR;
    }
    close1 = strchr(open1 + 1, ']');
    if (close1 == NULL) {
        printf(MAT_MISSING_FIRST_RB_ERR "\n", line_no);
        return ERROR;
    }

    /* validate first bracket content: r0..r7 only, no extras */
    scan = open1 + 1;
    while (scan < close1 && isspace((unsigned char)*scan)) scan++;
    if (scan >= close1 || *scan != 'r') {
        printf(MAT_FIRST_IDX_NEED_REG_ERR "\n", line_no);
        return ERROR;
    }
    scan++;
    if (scan >= close1 || !isdigit((unsigned char)*scan)) {
        printf(MAT_FIRST_IDX_NEED_DIGIT_ERR "\n", line_no);
        return ERROR;
    }
    reg_num = *scan - '0';
    if (reg_num < 0 || reg_num > 7) {
        printf(MAT_FIRST_IDX_RANGE_ERR "\n", line_no, reg_num);
        return ERROR;
    }
    scan++;
    while (scan < close1 && isspace((unsigned char)*scan)) scan++;
    if (scan != close1) {
        printf(MAT_FIRST_IDX_SINGLE_ERR "\n", line_no);
        return ERROR;
    }

    /* find second [ ... ] */
    open2 = strchr(close1 + 1, '[');
    if (open2 == NULL) {
        printf(MAT_MISSING_SECOND_LB_ERR "\n", line_no);
        return ERROR;
    }
    close2 = strchr(open2 + 1, ']');
    if (close2 == NULL) {
        printf(MAT_MISSING_SECOND_RB_ERR "\n", line_no);
        return ERROR;
    }

    /* validate second bracket content: r0..r7 only, no extras */
    scan = open2 + 1;
    while (scan < close2 && isspace((unsigned char)*scan)) scan++;
    if (scan >= close2 || *scan != 'r') {
        printf(MAT_SECOND_IDX_NEED_REG_ERR "\n", line_no);
        return ERROR;
    }
    scan++;
    if (scan >= close2 || !isdigit((unsigned char)*scan)) {
        printf(MAT_SECOND_IDX_NEED_DIGIT_ERR "\n", line_no);
        return ERROR;
    }
    reg_num = *scan - '0';
    if (reg_num < 0 || reg_num > 7) {
        printf(MAT_SECOND_IDX_RANGE_ERR "\n", line_no, reg_num);
        return ERROR;
    }
    scan++;
    while (scan < close2 && isspace((unsigned char)*scan)) scan++;
    if (scan != close2) {
        printf(MAT_SECOND_IDX_SINGLE_ERR "\n", line_no);
        return ERROR;
    }

    /* ensure nothing extra after second ']' (except whitespace) */
    scan = close2 + 1;
    while (*scan && isspace((unsigned char)*scan)) scan++;
    if (*scan != '\0') {
        printf(MAT_EXTRA_AFTER_ERR "\n", line_no);
        return ERROR;
    }

    return VALID;
}



/*
 * handle_operand_erorrs
 * input : opcode, src operand, dest operand, line_no, final_error*, expected operand count (0/1/2)
 * does  : routes to handle_each_operand for each required operand
 * return: VALID / ERROR
 */


Errors handle_operand_erorrs(int opcode, char *operand_src, char *operand_dest, int line_no, Errors *final_error, int excepted_operand_count)
{
    int error = VALID;
    switch (excepted_operand_count)
    {
    case 0:
        break;
    case 1:
        return handle_each_operand(opcode, operand_dest, line_no, 0, final_error);
        break;
    case 2:
        error = handle_each_operand(opcode, operand_src, line_no, 0, final_error);
        if (error == ERROR)
        {
            *final_error = ERROR;
            return ERROR;
        }
        error = handle_each_operand(opcode, operand_dest, line_no, 1, final_error);
        if (error == ERROR)
        {
            *final_error = ERROR;
            return ERROR;
        }
        break;
    }

    return error;
}

/*
 * validate_operand
 * input : action table, opcode, line_after_method (mnemonic + operands), line_no, final_error*
 * does  : checks arity by opcode group (0–4 => 2, 5–13 => 1, 14–15 => 0) and validates operands
 * return: VALID / ERROR
 */

Errors validate_operand(const Action *actions, int opcode, const char *line_after_method, int line_no, Errors *final_error)
{
    char buf[MAX_LINE_SIZE];
    char src[MAX_LABEL_SIZE];
    char dest[MAX_LABEL_SIZE];
    const char *mn = actions[opcode].name;
    const char *p = line_after_method;
    int got, expected;

    /* move p to after mnemonic and following spaces */
    p += strlen(mn);
    while (*p == ' ' || *p == '\t')
        p++;

    /* work on a local copy; parse_operands uses strtok */
    strncpy(buf, p, (size_t)(MAX_LINE_SIZE - 1));
    buf[MAX_LINE_SIZE - 1] = '\0';

    src[0] = '\0';
    dest[0] = '\0';
    parse_operands(buf, src, dest);

    got = has_text(src) + has_text(dest);

    /* groups: 0–4 => 2 ops, 5–13 => 1 op, 14–15 => 0 ops */
    if (opcode >= 0 && opcode <= 4)
    {

        if (strlen(src) > MAX_LABEL_SIZE || strlen(dest) > MAX_LABEL_SIZE)
        {
            /*too long*/
        }

        expected = 2;
    }
    else if (opcode >= 5 && opcode <= 13)
    {
        if (strlen(src) > MAX_LABEL_SIZE)
        {
            /*too long*/
        }
        expected = 1;
    }
    else if (opcode == 14 || opcode == 15)
    {
        expected = 0;
    }
    else
    {
        /* unknown opcode */
        printf(ERR_INVALID_OPERAND "\n", line_no, opcode);
        *final_error = ERROR;
        return ERROR;
    }

    if (got != expected)
    {
        /* swap printf with your #define macro if you have one */
        printf(FULL_ERR_MESSAGE "\n", line_no, mn, expected, (expected == 1 ? "" : "s"), got);
        *final_error = ERROR;
        return ERROR;
    }

    return handle_operand_erorrs(opcode, src, dest, line_no, final_error, expected);
}

/*
 * validate_data
 * input : line_after_method (starts at directive), line_no, final_error*
 * does  : dispatches to the proper directive validator
 * return: VALID / ERROR
 */

Errors validate_data(const char *line_after_method, int line_no, Errors *final_error)
{
    char buf[MAX_LINE_SIZE];
    const char *p = line_after_method;

    /* writable copy for scans */
    strncpy(buf, p, (size_t)(MAX_LINE_SIZE - 1));
    buf[MAX_LINE_SIZE - 1] = '\0';

    p = buf;
    while (*p == ' ' || *p == '\t')
        p++;

    if (strncmp(p, ".string", 7) == 0)
        return validate_string_directive(p, line_no, final_error);
    else if (strncmp(p, ".data", 5) == 0)
        return validate_data_list(p, line_no, final_error);
    else if (strncmp(p, ".mat", 4) == 0)
        return validate_mat_directive(p, line_no, final_error);
    else if (strncmp(p, ".entry", 6) == 0)
        return validate_entry(p, line_no, final_error);
    else if (strncmp(p, ".extern", 7) == 0)
        return validate_extern(p, line_no, final_error);
    else
    {
        printf("Line %d: Undefined Instruction\n", line_no);
    }
    return VALID;
}

/*
 * validate_string_directive
 * input : pointer at ".string", line_no, final_error*
 * does  : checks quotes, printable chars only, no extra text after closing quote
 * return: VALID / ERROR
 */


Errors validate_string_directive(const char *p, int line_no, Errors *final_error)
{
    const char *q = p + 7; /* after ".string" */
    const char *close, *r;

    while (*q == ' ' || *q == '\t')
        q++;

    if (*q != '\"')
    {
        printf(STRING_START_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }
    q++; /* after opening quote */

    close = strchr(q, '\"');
    if (close == NULL)
    {
        printf(STRING_END_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    for (r = q; r < close; r++)
    {
        if (!isprint((unsigned char)*r))
        {
            printf(NON_PRINTABLE_CHAR_ERR "\n", line_no);
            *final_error = ERROR;
            return ERROR;
        }
    }

    close++;
    while (*close != '\0' && isspace((unsigned char)*close))
        close++;
    if (*close != '\0')
    {
        printf(EXTRA_CHAR_IN_LINE "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    return VALID;
}


/*
 * validate_data_list
 * input : pointer at ".data", line_no, final_error*
 * does  : validates comma-separated signed 10-bit ints (-512..511), comma rules
 * return: VALID / ERROR
 */

Errors validate_data_list(const char *p, int line_no, Errors *final_error)
{
    char list[MAX_LINE_SIZE];
    char tmp[MAX_LINE_SIZE];
    char *tok;

    p += 5; /* after ".data" */
    while (*p == ' ' || *p == '\t')
        p++;

    strncpy(list, p, (size_t)(MAX_LINE_SIZE - 1));
    list[MAX_LINE_SIZE - 1] = '\0';

    /* quick empty check */
    strncpy(tmp, list, (size_t)(MAX_LINE_SIZE - 1));
    tmp[MAX_LINE_SIZE - 1] = '\0';
    trim_ends(tmp);
    if (tmp[0] == '\0')
    {
        printf(EMPLY_DATA_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    if (has_trailing_comma(list))
    {
        printf(TRAILING_DATA_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    tok = strtok(list, ",");
    while (tok != NULL)
    {
        long v;
        trim_ends(tok);
        if (tok[0] == '\0')
        {
            printf(MISSING_DATA "\n", line_no);
            *final_error = ERROR;
            return ERROR;
        }
        if (!is_int10(tok, &v))
        {
            printf(INVALIDE_DATA "\n", line_no, tok);
            *final_error = ERROR;
            return ERROR;
        }
        tok = strtok(NULL, ",");
    }
    return VALID;
}

/*
 * validate_mat_directive
 * input : pointer at ".mat", line_no, final_error*
 * does  : parses [rows][cols], validates signed 10-bit values list size = rows*cols
 * return: VALID / ERROR
 */

Errors validate_mat_directive(const char *p, int line_no, Errors *final_error)
{
    char values[MAX_LINE_SIZE];
    const char *after_rows;
    const char *after_cols;
    const char *vals;
    char *tok;
    int rows, cols, count = 0;

    p += 4; /* after ".mat" */
    while (*p == ' ' || *p == '\t')
        p++;

    /* parse [rows] */
    if (!parse_dim(p, &rows, &after_rows))
    {
        printf(FIRST_DIN_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    /* skip spaces to [cols] */
    p = after_rows;
    while (*p == ' ' || *p == '\t')
        p++;

    /* parse [cols] */
    if (!parse_dim(p, &cols, &after_cols))
    {
        printf(SECOND_DIM_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    /* move to values after second ']' */
    vals = after_cols;
    while (*vals == ' ' || *vals == '\t')
        vals++;

    if (*vals == '\0')
    {
        printf(NO_VALUE_MAT_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    strncpy(values, vals, (size_t)(MAX_LINE_SIZE - 1));
    values[MAX_LINE_SIZE - 1] = '\0';

    if (has_trailing_comma(values))
    {
        printf(TRAILING_MAT_ERR "\n", line_no);
        *final_error = ERROR;
        return ERROR;
    }

    tok = strtok(values, ",");
    while (tok != NULL)
    {
        long v;
        trim_ends(tok);
        if (tok[0] == '\0')
        {
            printf(EMPTY_INSIDE_COMMA "\n", line_no);
            *final_error = ERROR;
            return ERROR;
        }
        if (!is_int10(tok, &v))
        {
            printf(INVALID_MAT_DATA "\n", line_no, tok);
            *final_error = ERROR;
            return ERROR;
        }
        count++;
        tok = strtok(NULL, ",");
    }

    if (count != rows * cols)
    {
        printf(MAT_SIZE_DATA_MIS_MATCH_ERR "\n", line_no, count, rows, cols);
        *final_error = ERROR;
        return ERROR;
    }

    return VALID;
} 





/* --- .entry / .extern label validators --- */

/*
 * validate_entry
 * input : pointer at ".entry", line_no, final_error*
 * does  : extracts label and validates label syntax
 * return: VALID / ERROR
 */


Errors validate_entry(const char *p, int line_counter, Errors *final_error)
{
    char tmp[MAX_LINE_SIZE];
    char *tok;
    char entry_label[MAX_LABEL_SIZE];

    /* p points at ".entry..." */
    strncpy(tmp, p + 6, (size_t)(MAX_LINE_SIZE - 1));
    tmp[MAX_LINE_SIZE - 1] = '\0';

    tok = strtok(tmp, " \t\r\n");
    if (tok == NULL)
    {
        printf(MISSING_LABEL "\n", line_counter);
        *final_error = ERROR;
        return ERROR;
    }

    strncpy(entry_label, tok, (size_t)(MAX_LABEL_SIZE - 1));
    entry_label[MAX_LABEL_SIZE - 1] = '\0';
    trim_ends(entry_label);

    if (examine_Label(entry_label, &line_counter) != VALID)
    {
        printf(".entry label \n");
        *final_error = ERROR;
        return ERROR;
    }
    return VALID;
}

/*
 * validate_extern
 * input : pointer at ".extern", line_no, final_error*
 * does  : extracts label and validates label syntax
 * return: VALID / ERROR
 */

Errors validate_extern(const char *p, int line_counter, Errors *final_error)
{
    char tmp[MAX_LINE_SIZE];
    char *tok;
    char extern_label[MAX_LABEL_SIZE];

    /* p points at ".extern..." */
    strncpy(tmp, p + 7, (size_t)(MAX_LINE_SIZE - 1));
    tmp[MAX_LINE_SIZE - 1] = '\0';

    tok = strtok(tmp, " \t\r\n");
    if (tok == NULL)
    {
        printf(MISSING_LABEL "\n", line_counter);
        *final_error = ERROR;
        return ERROR;
    }

    strncpy(extern_label, tok, (size_t)(MAX_LABEL_SIZE - 1));
    extern_label[MAX_LABEL_SIZE - 1] = '\0';
    trim_ends(extern_label);

    if (examine_Label(extern_label, &line_counter) != VALID)
    {
        printf(".extern label \n");
        *final_error = ERROR;
        return ERROR;
    }
    return VALID;
}

/* ---------- helpers ---------- */

/*
 * has_text
 * input : string
 * does  : returns 1 if non-empty, else 0
 */
int has_text(const char *s)
{
    return (s != NULL && s[0] != '\0');
}

/*
 * trim_ends
 * input : mutable string
 * does  : trims leading/trailing spaces, tabs, and line endings
 */

void trim_ends(char *s)
{
    char *p, *q;
    if (s == NULL)
        return;
    p = s;
    while (*p == ' ' || *p == '\t')
        p++;
    if (p != s)
        memmove(s, p, (size_t)(strlen(p) + 1));
    q = s + strlen(s);
    while (q > s && (q[-1] == ' ' || q[-1] == '\t' || q[-1] == '\n' || q[-1] == '\r'))
        q--;
    *q = '\0';
}

/*
 * is_int10
 * input : numeric text, out* value
 * does  : parses signed integer; accepts whitespace and +/-; checks range [-512..511]
 * return: 1 ok, 0 bad format/range
 */

int is_int10(const char *tok, long *out)
{
    const char *p = tok;
    char *endp;
    long v;

    while (*p != '\0' && isspace((unsigned char)*p))
        p++;
    if (*p == '\0')
        return 0;

    v = strtol(p, &endp, 10);
    if (endp == p)
        return 0;

    while (*endp != '\0' && isspace((unsigned char)*endp))
        endp++;
    if (*endp != '\0')
        return 0;

    if (v < -512 || v > 511)
        return 0;
    if (out)
        *out = v;
    return 1;
}

/*
 * parse_dim
 * input : pointer at '[', outputs dimension value and pointer after closing ']'
 * does  : parses positive integer inside brackets
 * return: 1 ok, 0 invalid
 */

int parse_dim(const char *p, int *out, const char **after_bracket)
{
    const char *q = p;
    long v = 0;
    char *endp;

    if (*q != '[')
        return 0;
    q++; /* after '[' */
    while (*q != '\0' && isspace((unsigned char)*q))
        q++;

    v = strtol(q, &endp, 10);
    if (endp == q)
        return 0; /* no digits */
    if (v <= 0 || v > 32767)
        return 0; /* invalid range */
    q = endp;

    while (*q != '\0' && isspace((unsigned char)*q))
        q++;
    if (*q != ']')
        return 0;

    if (out)
        *out = (int)v;
    if (after_bracket)
        *after_bracket = q + 1;
    return 1;
}

/*
 * has_trailing_comma
 * input : string
 * does  : after trimming, returns 1 if last non-space char is ',', else 0
 */
int has_trailing_comma(const char *s)
{
    size_t L = strlen(s);
    while (L > 0 && (s[L - 1] == ' ' || s[L - 1] == '\t' || s[L - 1] == '\n' || s[L - 1] == '\r'))
        L--;
    return (L > 0 && s[L - 1] == ',');
}

