<!-- Required extensions: pymdownx.betterem, pymdownx.tilde, pymdownx.emoji, pymdownx.tasklist, pymdownx.superfences -->
# multren
Linux console utility to rename multiple files


```text
multren {p | x | s | r} {TARGET} {PREFIX | NEWEXT | SUFFIX | STRING}
EXAMPLES:
multren x .dat txt ---> change extension
multren x myfile txt ---> change|add extension
multren x myfile '' ---> remove extension
multren p .dat 'new-pre-' ---> add prefix
multren p myfile 'new-pre-' ---> add prefix
multren p .dat 5 ---> add prefix 005_ 010_ 015_ ...
multren s .dat '-new-suf' ---> add suffix
multren s myfile '-new-suf' ---> add suffix
multren r 'test' 'production' ---> string replacement
multren r 'test' '' ---> string deletion
```
