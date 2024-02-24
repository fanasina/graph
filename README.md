## graph
some graph functions.


### use
```
gcc graph10.c -o raf
./raf
french ? y/n
```
`n` = english!

`would you like to work on undirected graphs? y/n`

`y`

```
1. enter edges.
2. display the edges.
3. enter vertex.
4. display vertices
5. display the neighbours of a vertex
6. browse in depth (recursive).
7. browse in depth (iterative).
8. display graph matrix
9. to list the paths between 2 summits.
0. browse widthwise (iterative).
q. Quit (or s. Exit).
```
choice, `1` to enter edges: accept chain separate with these separator characters: space` ` or coma`,` or `|` . 

the link between vertices N1 and N2 can be any character string of length one or two other than the separator characters.
#### example
```1-2-*5-+8--8,7/4//6\3 10++11++12..13 15..8```

`2` to list edges

```
1 - 2 | 2 - 5 | 5 - 8 | 8 - 8 | 7 - 4 | 4 - 6 | 6 - 3 | 10 - 11 | 11 - 12 | 12 - 13 | 15 - 8 |
```

have fun!
