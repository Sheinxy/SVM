all: svm

svm: tools.o cpu.o op.o syscalls.o svm.o

clean:
	$(RM) *.d
	$(RM) *.o
	$(RM) svm

clear:
	$(RM) *.d
	$(RM) *.o