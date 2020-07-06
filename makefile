Target=random-matrix.exe small-world.exe self-avoid-walk.exe

All: $(Target)

CFLAGS=/source-charset:utf-8 /D_CRT_SECURE_NO_WARNINGS=1 /EHsc /O2 /nologo /GL /W4 /MD /arch:AVX2 /openmp

random-matrix.exe: random-matrix.cpp
	cl random-matrix.cpp $(CFLAGS) /Fe:random-matrix.exe

small-world.exe: small-world.cpp
	cl small-world.cpp $(CFLAGS) /Zi /Fe:small-world.exe

self-avoid-walk.exe: self-avoid-walk.cpp
	cl self-avoid-walk.cpp $(CFLAGS) /Fe:self-avoid-walk.exe

mepc.wasm: kpsolver.c uwraptype.c genhlist.c
	clang --target=wasm32 -nostdlib -O2 -Xlinker --no-entry -Xlinker --export-all -Xlinker --import-memory -o mepc.wasm kpsolver.c uwraptype.c genhlist.c

clean:
	del *.obj

cleanall:
	del *.obj
	del *.exe