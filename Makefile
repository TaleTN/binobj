# Copyright (C) 2021 Theo Niessink <theo@taletn.com>
# This work is free. You can redistribute it and/or modify it under the
# terms of the Do What The Fuck You Want To Public License, Version 2,
# as published by Sam Hocevar. See http://www.wtfpl.net/ for more details.

CFLAGS = /O2 /GS- /D NDEBUG /W3 /D _CRT_SECURE_NO_WARNINGS /nologo

all : binobj.exe

binobj.exe : binobj.c
	$(CC) $(CFLAGS) $**

clean :
	@for %i in (binobj.obj binobj.exe) do @if exist %i del %i | echo del %i
