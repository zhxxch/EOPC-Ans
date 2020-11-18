Target=random-matrix.exe small-world.exe self-avoid-walk.exe

All: $(Target)

CXXFLAGS=/source-charset:utf-8 /D_CRT_SECURE_NO_WARNINGS=1 /EHsc /O2 /nologo /GL /W4 /MD /arch:AVX2 /openmp /std:c++14

random-matrix.exe: $*.cpp
	cl $*.cpp $(CXXFLAGS) /Fe:$@

small-world.exe: $*.cpp
	cl $*.cpp $(CXXFLAGS) /Zi /Fe:$@

self-avoid-walk.exe: $*.cpp
	cl $*.cpp $(CXXFLAGS) /Fe:$@

speck-perf.exe: $*.cpp
	cl $*.cpp $(CXXFLAGS) /Fe:$@ /FAs

mepc.wasm: kpsolver.c uwraptype.c genhlist.c
	clang --target=wasm32 -nostdlib -O2 -Xlinker --no-entry -Xlinker --export-all -Xlinker --import-memory -o mepc.wasm kpsolver.c uwraptype.c genhlist.c

clean:
	del *.obj
	del *.exe
	del *.pdb