# Compiler for the m19 language

See the manual for m19 [here](m19.pdf)

## Requirements
- Linux machine
- flex(tested on 2.6.4), byacc(tested on 1.9), yasm(tested on 1.3.0), GNU ld(version 2.31.1)

## Features
- This compilers compiles to 32 bit architectures
- The language is compatible with C
- The cdk tarball present in this repo is the skeleton of the compiler
- The rts tarball present in this repo provides libraries for linking

## How to Run

### Compile the compiler

Unpack the cdk and rts tarballs, and run "make install" in each one.
This will create a directory in the HOME folder, "compiladores/root"
where the libraries will be copied to. You can always change this by
editing the Makefiles top section (it has to be consistent with the 
m19 Makefile).

```
tar xf Libcdk14-201902132207.tar.bz2
tar xf Librts4-201602111900.tar.bz2
cd libcdk14-201902132207
make install
cd ../librts4-201602111900
make install
```

In m19/, run "make":
```
cd m19
make
```

The m19 binary will now be inside m19/

### Compile m19 programs

Let main.m19 be our m19 code.
First, generate assembly:
```
/path/to/m19 main.m19 
```
This will place a file named "main.asm" in the main.m19 location.
Now, use yasm to generate binary code (32 bit elf):
```
yasm -felf32 main.m19
```
It outputs "main.o". Now we just have to link it with ld:
```
ld -m elf_i386 -o main main.o -L/path/to/rts/lib -lrts
```
main is your compiled m19 program.
If you wish to use several .m19 files, you just need to link them all with the ld command:
```
ld -m elf_i386 -o main file1.o file2.o file3.o -L/path/to/rts/lib -lrts
```

### XML
If you wish to generate the xml tree of the program, do:
```
/path/to/m19 main.m19 --target xml
```
And it outputs "main.xml"
### Test

Run [./test.sh](m19/test.sh). Tweak the script variables as needed.
