; errors second pass

; entry not defined
.entry MISSING

; entry–extern conflict
.extern BOTH
.entry BOTH

; extern declared but also defined locally
.extern LOCAL
LOCAL: .data 5

; duplicate label definitions
DUP: .data 1
DUP: .string "x"

; undefined symbols referenced in code
MAIN: mov r1, UNDEF
      jmp UNDEF2

