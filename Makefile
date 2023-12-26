all: calculate calculate.7.gz

# Which compiler
CC = g++

# Where are include files kept
INCLUDE = .

# Where to install
INSTDIR = /usr/local/bin
MANDIR = /usr/share/man/man7

# Options for development
#CFLAGS = -g -Wall -ansi

# Options for release
CFLAGS = -O2 -ansi

debug: calculate.cpp eval.cpp
	$(CC) -g -o calculate calculate.cpp eval.cpp

calculate: calculate.o eval.o
	$(CC) -o calculate calculate.o eval.o

calculate.o: calculate.cpp eval.cpp
	$(CC) -I$(INCLUDE) $(CFLAGS) -c calculate.cpp eval.cpp

calculate.7.gz: calculate.7.man
	gzip -c calculate.7.man > calculate.7.gz

clean:
	-rm -f *.o
	-rm -f calculate
	-rm -f calculate.7.gz

install: calculate calculate.7.gz
	@if [ -d $(INSTDIR) ]; \
	then \
		install -m 755 calculate $(INSTDIR);\
		(cd $(INSTDIR) && ln -sf calculate ans);\
		echo "Installed calculate and ans in $(INSTDIR)";\
	else \
		echo "Error: $(INSTDIR) does not exist";\
		exit 1;\
	fi
	@if [ -d $(MANDIR) ]; \
	then \
		install -m 644 calculate.7.gz $(MANDIR);\
		echo "Installed man page in $(MANDIR)";\
	else \
		echo "Error: $(MANDIR) does not exist";\
	fi

uninstall:
	@if [ -d $(INSTDIR) ]; \
	then \
		rm -f $(INSTDIR)/calculate;\
		rm -f $(INSTDIR)/ans;\
		echo "Removed calculate and ans from $(INSTDIR)";\
	else \
		echo "Error: $(INSTDIR) does not exist";\
	fi
	@if [ -d $(MANDIR) ]; \
	then \
		rm -f $(MANDIR)/calculate.7.gz;\
		echo "Removed man page from $(MANDIR)";\
	else \
		echo "Error: $(MANDIR) does not exist";\
	fi
