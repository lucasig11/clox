
# Clox

A bytecode VM interpreter for the Lox programming language.

## ๐ Requirements
- [Premake5](https://premake.github.io/download/) 
- GNU Make and some C/C++ compiler  

*Testing/Developing only*
- [Dart](https://dart.dev/get-dart) 
- [Watchexec](https://github.com/watchexec/watchexec) 

## ๐  Build
```sh
premake5 gmake2
make config=release
```

## ๐ Run
```sh
./bin/release/clox  		# Launches the REPL
./bin/release/clox  [file] 	# Executes a script
```

## ๐งช Test
```sh
./tool/bin/run-tests
```

## ๐งฑ Develop
Watches for changes in the source files, compile the program and run the tests.
```sh
sh tool/bin/watch.sh [chapter] # chapter is optional
```
