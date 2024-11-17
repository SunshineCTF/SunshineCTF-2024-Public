SOLUTION: `AABBBBBBBB%9$sCCCCCCCCCCCCCC1`
  - (`AA`) -> overflows the option character variable
    - had to make this 2 otherwise the `scanf` would overflow the null
    terminator into pronouns variable and would make it an empty print,
    did not want to confuse CTFers.
  - (`BBBBBBBB`) -> overflows pronouns variable to reach the username
  - (`%9$s`) -> string formatter exploit
  - (`CCCCCCCCCCCCCC`) -> overflows the username variable to reach `is_admin`
  - (`1`) -> any non-zero byte to flip it to true
