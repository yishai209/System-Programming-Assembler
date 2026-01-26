; errors_data

; STRING_START_ERR 
.string abc

; STRING_END_ERR 
.string "abc

; NON_PRINTABLE_CHAR_ERR 
.string "a	b"   ; includes a tab  

; EXTRA_CHAR_IN_LINE 
.string "abc" junk

; EMPLY_DATA_ERR 
.data

; MISSING_DATA 
.data 1,,2

; INVALIDE_DATA 
.data 1,+x,3

; TRAILING_DATA_ERR 
.data 1,2,3,

; FIRST_DIN_ERR 
.mat 2][2] 1,2,3,4

; SECOND_DIM_ERR 
.mat [2]2] 1,2,3,4

; NO_VALUE_MAT_ERR 
.mat [2][2]

; TRAILING_MAT_ERR 
.mat [1][2] 1,2,

; EMPTY_INSIDE_COMMA 
.mat [1][3] 1,,3

; INVALID_MAT_DATA 
.mat [1][2] 1,600

; MAT_SIZE_DATA_MIS_MATCH_ERR 
.mat [2][2] 1,2,3
