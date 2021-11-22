BINARY TO OBJECT WINDOWS DEV TOOL

Converts a binary file to a machine-independent Windows object (OBJ) file,
so you can link the binary file with your own code.

HOW TO BUILD & RUN

1. Open the Command Prompt for VS (Visual Studio).

2. Run the Program Maintenance Utility:

   nmake

3. Convert your binary file to an object file:

   binobj.exe MyArray 16 my_data.bin my_data.obj

   Replace "MyArray" with the symbol name you want to use for your data, and
   replace "16" with the alignment (in bytes) that is appropriate for your
   data.

4. Add this line to your own source code to declare the data as external:

   extern "C" const unsigned char MyArray[1024*1024];

   Replace "unsigned char" with the POD type of your binary data, and
   replace "1024*1024" with the number of elements that are needed to hold
   your data. If you don't need to know the size at compile time, then you
   can also omit the number of elements.

5. Link your own code against the object file, e.g.:

   cl my_program.cpp my_data.obj

ALTERNATE SOLUTION FOR OTHER PLATFORMS

AFAIK the object file format used on other platforms isn't as well-defined
as on Windows, so I don't have a cross-platform version of this tool.

However, if your platform comes with the GNU Binary Utilities, *and* if it
includes objcopy, then you could do the following:

1. Create a C source file (my_data.c) that contains a single line:

   const unsigned char MyArray[1024*1024] = { 0 };

   Replace "unsigned char" with the POD type of your binary data, and
   replace "1024*1024" with the number of elements that are needed to hold
   your data.

2. Compile this C file to create an object file containing zeros, and then
   use objcopy to replace the zeros with the actual binary data:

   gcc -fvisibility=hidden -c -o my_data.o my_data.c
   objcopy --update-section .rodata=my_data.bin my_data.o

3. Add this line to your own source code to declare the data as external:

   extern "C" const unsigned char MyArray[1024*1024];

   If you don't need to know the size at compile time, then you can also
   omit the number of elements here.

4. Link your own code against the object file, e.g.:

   g++ my_program.cpp my_data.o

Note that Apple's Xcode apparently doesn't include objcopy, so for macOS I
currently have no good solution (other than to convert your binary data to C
source code).

LICENSE

Copyright (C) 2021 Theo Niessink <theo@taletn.com>
This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.
