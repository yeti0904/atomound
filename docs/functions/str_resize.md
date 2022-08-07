# str_resize

`str_resize str(string) size(word/int)`

resize `str` to be `size` characters long and return the new string

## example
```
@main
	let string mystr = "hello"
	print mystr "(end)\n"
	mystr = str_resize mystr 10
	print mystr "(end)\n"
	exit 0
```

Output:
```
hello(end)
hello     (end)
```
