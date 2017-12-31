all: run

EXE =	cal_exp
OUT = $(subst in,out,$(wildcard *in.txt))

.PHONY: run clean

run: $(EXE) $(OUT)

clean:
	-rm $(OUT:%="%") $(EXE)

$(EXE): $(EXE).c
	gcc -o $(EXE) $(EXE).c

$(OUT): %.out.txt: %.in.txt $(EXE)
	./$(EXE) < $*.in.txt > $*.out.txt


