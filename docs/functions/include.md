# include

`include path(string)`

includes and executes the code belonging to the file `path`

## example
lib.atmo
```
goto end

@sayhello
	print "hello\n"
	return

@end
```

main.atmo
```
@main
	include "lib.atmo"
	sayhello
	exit 0
```

Output:
```
hello
```
