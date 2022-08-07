# goto_if

`goto_if place(word)`

jump to `place` if the last return value is `true` or non-zero, `place` is usually provided using a label identifier

## example
```
@main
	let integer myint = 5
	is_equal myint 5
	goto_if :equal
	exit 0
	@:equal
		print "they are equal\n"
		exit 0
```

Output
```
they are equal
```
