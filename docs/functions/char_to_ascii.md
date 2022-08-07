# char_to_ascii

```
char_to_ascii ch(string)
```

returns an `integer` of the ASCII value of the first character in `ch`

if `ch` is empty, it will return 0

## example
```
@main
	let integer ch = char_to_ascii "A"
	print ch "\n"
	exit 0
```

Output:
```
65
```
