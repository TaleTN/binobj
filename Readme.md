# Binary to object Windows dev tool

Converts a binary file to a machine-independent Windows object (OBJ) file,
so you can link the binary file with your own code.

## How to build & run

1. Open the Command Prompt for VS (Visual Studio).

2. Run the Program Maintenance Utility:

    `nmake`

3. Convert your binary file to an object file:

    `binobj.exe MyArray 16 my_data.bin my_data.obj`

    Replace `MyArray` with the symbol name you want to use for your data,
    and replace `16` with the alignment (in bytes) that is appropriate for
    your data.

4. Add this line to your own source code to declare the data as external:

    `extern "C" const unsigned char MyArray[1024*1024];`

    Replace `unsigned char` with the POD type of your binary data, and
    replace `1024*1024` with the number of elements that are needed to hold
    your data. If you don't need to know the size at compile time, then you
    can also omit the number of elements.

5. Link your own code against the object file, e.g.:

    `cl my_program.cpp my_data.obj`

## Alternate solution for other platforms

This tool is Windows only, so it won't work on other platforms, like macOS
or Linux. However, if your platform comes with the GNU Binary Utilities,
**and** if it includes objcopy, then you could do the following:

1. Create a C source file (my_data.c) that contains a single line:

    `const unsigned char MyArray[1024*1024] = { 0 };`

    Replace `unsigned char` with the POD type of your binary data, and
    replace `1024*1024` with the number of elements that are needed to hold
    your data.

2. Compile this C file to create an object file containing zeros, and then
   use objcopy to replace the zeros with the actual binary data:

    `gcc -fvisibility=hidden -c -o my_data.o my_data.c`
    `objcopy --update-section .rodata=my_data.bin my_data.o`

3. Add this line to your own source code to declare the data as external:

    `extern "C" const unsigned char MyArray[1024*1024];`

    If you don't need to know the size at compile time, then you can also
    omit the number of elements here.

4. Link your own code against the object file, e.g.:

    `g++ my_program.cpp my_data.o`

Note that Apple's Xcode apparently doesn't include objcopy, but there is yet
another way:

1. Create an assembly source file (my_data.s) that contains the following
   lines:

    `.static_const`
    `.align 4`
    `.globl _MyArray`
    `_MyArray: .incbin "my_data.bin"`

    Replace `.align 4` with the alignment that is appropriate for your data.
    Note that this alignment is in 2^n bytes, so `.align 4` actually means
    2^4 = 16 bytes.

2. Assemble this assembly file to create an object file:

    `as -o my_data.o my_data.s`

3. Add this line to your own source code to declare the data as external:

    `extern "C" const unsigned char MyArray[1024*1024];`

    If you don't need to know the size at compile time, then you can also
    omit the number of elements here.

4. Link your own code against the object file:

    `g++ my_program.cpp my_data.o`

5. Create a symbol file (my_symbols.exp) containing the following line:

    `_MyArray`

    Note that this file can also hold multiple lines/symbols, in case you
    have multiple binary files.

6. Strip the symbols from your binary code:

    `strip -R my_symbols.exp -i a.out`

Note that you can also add the assembly file to your Xcode project, and then
set *Build Settings* > *Additional Strip Flags* to `-R my_symbols.exp -i`,
so you can build using Xcode.

## License

Copyright &copy; 2021 Theo Niessink &lt;theo@taletn.com&gt;  
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
