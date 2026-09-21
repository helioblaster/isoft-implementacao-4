.PHONY: all clean

all:
	$(MAKE) -C src/utils setitimer-helper
	$(MAKE) -C src/threads

clean:
	$(MAKE) -C src/threads clean
	$(MAKE) -C src/utils clean
