
lz4tcl.dll: lz4tcl.c
	gcc -shared -o $@ $< -Wl,-dy,-ltcl,-dn,-llz4
	strip $@

.PHONY: clean
clean:
	rm -f lz4tcl.dll
