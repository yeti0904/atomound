# set_char

`set_char str(string) idx(integer/word) newChar(string)`

set the `idx`th character of `str` to `newChar` and returns the new string

## example
```
@main
	let string str = "hello\n"
	print str
	str = set_char str 0 "H"
	print str
	exit 0
```

Output
```
hello
Hello
```
