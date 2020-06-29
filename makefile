Target=random-matrix.exe small-world.exe

All: $(Target)

CFLAGS=/D_CRT_SECURE_NO_WARNINGS=1 /EHsc /O2 /nologo /GL /W4 /MD /arch:AVX2

random-matrix.exe: random-matrix.cpp
	cl random-matrix.cpp $(CFLAGS) /Fe:random-matrix.exe

small-world.exe: small-world.c
	cl small-world.c $(CFLAGS) /Fe:small-world.exe

mepc.wasm: kpsolver.c uwraptype.c genhlist.c
	clang --target=wasm32 -nostdlib -O2 -Xlinker --no-entry -Xlinker --export-all -Xlinker --import-memory -o mepc.wasm kpsolver.c uwraptype.c genhlist.c

clean:
	del *.obj

cleanall:
	del *.obj
	del *.exe