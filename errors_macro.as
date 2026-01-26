; macro errors tester 


;  ERR_MACRO_EMPTY_MSG
mcro
mcroend

;ERR_MACRO_FIRST_CHAR_MSG
mcro 1bad
body
mcroend

; ERR_MACRO_INVALID_CHAR_MSG
mcro good-name
content
mcroend

; ERR_MACRO_RESERVED_WORD_MSG
mcro mov
x
mcroend

; ERR_MACRO_TOO_LONG_MSG
mcro aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
line
mcroend

; A valid macro (so the duplicate check below can trigger)
mcro dup
ok
mcroend

; ERR_MACRO_ALREADY_DEFINED_MSG
mcro dup
again
mcroend

; ERR_EXTRA_TEXT_AFTER_MCROEND_MSG
mcro wat
line
mcroend junk

; ERR_LINE_TOO_LONG

XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


; ERR_MACRO_EMPTY_MSG 
mcro
mcroend

; ERR_MACRO_FIRST_CHAR_MSG 
mcro 1bad
line
mcroend

; ERR_MACRO_INVALID_CHAR_MSG 
mcro good-name
body
mcroend

; ERR_MACRO_RESERVED_WORD_MSG 
mcro mov
x
mcroend

; ERR_MACRO_TOO_LONG_MSG 
mcro aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
y
mcroend

; ERR_MACRO_ALREADY_DEFINED_MSG 
mcro dup
ok
mcroend
mcro dup
again
mcroend

; ERR_EXTRA_TEXT_AFTER_MCROEND_MSG 
mcro testm
t
mcroend junk
