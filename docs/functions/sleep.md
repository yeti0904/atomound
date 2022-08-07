# sleep

`sleep time(integer/float)`

sleeps for `time` seconds

## example
```
@main
	let integer i = 1
	@:loop
		print i "\n"
		is_equal i 10
		goto_if :done
	@:done
		exit 0
```

Output:
```
1
2
3
4
5
6
7
8
9
10
```
