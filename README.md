
# Clox

A bytecode VM interpreter for the Lox programming language.

## 📌 Requirements
- [Premake5](https://premake.github.io/download/) 
- [Dart](https://dart.dev/get-dart)
- [Watchexec](https://github.com/watchexec/watchexec)
- GNU Make and some C/C++ compiler

## 🛠 Build
```sh
premake5 gmake2
make config=release
```

## 🚀 Run
```sh
./bin/release/clox  		# Launches the REPL
./bin/release/clox  [file] 	# Executes a script
```

## 🧪 Test
```sh
dart tool/bin/test.dart --interpreter bin/release/clox
```

## 🧱 Develop
Watches for changes in the source files, compile the program and run the tests.
```sh
chmod +x watch.sh
./watch.sh [chapter] # chapter is optional
```
