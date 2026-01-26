; errors_code


; INVALID_LABEL 
1bad: mov r0,r1

; LABEL_TOO_LONG 
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa: mov r0,r1   ; 33 'a'

; CANNOT_BE_RESERVED_WORD 
mov: r1,r2

; FULL_ERR_MESSAGE 
stop r1

; FULL_ERR_MESSAGE 
not r1, r2

; INVALID_NUM_FORMAT_ERR 
prn #+1a

; INVALID_NUM_RANGE_ERR 
prn #200

; Ilegal type of operand 
lea #5, r1

; Ilegal type of operand 
mov r1, #5

; MAT_NO_LABEL_BEFORE_LB_ERR 
mov [r0][r1], r2

; MAT_FIRST_IDX_NEED_REG_ERR 
mov M1[x3][r0], r2

; MAT_FIRST_IDX_NEED_DIGIT_ERR 
mov M1[rx][r0], r2

; MAT_FIRST_IDX_RANGE_ERR
mov M1[r8][r0], r2

; MAT_FIRST_IDX_SINGLE_ERR 
mov M1[r0 r1][r2], r3

; MAT_MISSING_SECOND_LB_ERR 
mov M1[r0]r1], r2

; MAT_MISSING_SECOND_RB_ERR 
mov M1[r0][r1, r2

; MAT_SECOND_IDX_NEED_REG_ERR 
mov M1[r0][x3], r2

; MAT_SECOND_IDX_NEED_DIGIT_ERR 
mov M1[r0][rx], r2

; MAT_SECOND_IDX_RANGE_ERR 
mov M1[r0][r9], r2

; MAT_SECOND_IDX_SINGLE_ERR 
mov M1[r0][r1 r2], r3

; MAT_EXTRA_AFTER_ERR 
mov M1[r0][r1]junk, r2

; Undefined Instruction
unknownop r1,r2

