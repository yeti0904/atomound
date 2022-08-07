# unpass

```
unpass
```

pops and returns an argument from the pass stack, which is used to give functions parameters

## example
```
@putstr
	let string str = unpass
	print str "\n"

@main
	putstr "hello"
	exit 0
```

Output
```
hello
```
