
# Clox

A bytecode VM interpreter for the Lox programming language.

# 🛠 Build
```sh
premake5 gmake2
make config=release
```

# 🧪 Test
```sh
dart tool/bin/test.dart --interpreter bin/release/clox
```

# 🚀 Run
```sh
./bin/release/clox  		# Launches the REPL
./bin/release/clox  [file] 	# Executes a script
```

# 🧱 Develop
```sh
chmod +x watch.sh
./watch.sh [chapter] # Watches the changes in the source files, compile them and run the tests.
```
