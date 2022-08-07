# return

`return ret(any type)`

returns 1 value to the caller

## example
```
@myfunc
	print "hello world\n"
	return // return nothing

@myfunc2
	print "hello world\n"
	return 5

@main
	myfunc
	let integer res = myfunc2
	print res "\n"
```

Output:
```
hello world
hello world
5
```
