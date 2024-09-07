.PRECIOUS: %.bc
all: run.primes

run.%: %.bc
	opt -enable-new-pm=0 -load ../build/src/LeptoInst.so -LeptoInst -disable-output $<

%.bc: %.cpp
	clang++ -O3 -c -emit-llvm $<

clean:
	rm -f *.bc

.PHONY: all run.% clean


