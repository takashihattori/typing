export DEST_DIR = /pub/sfc/ipl/typing

all: 
	cd typingtest2; $(MAKE)
	cd typingadmin2; $(MAKE)

install:
	cd typingtest2; $(MAKE) install
	cd typingadmin2; $(MAKE) install

install-question:
	cd typingtest2; $(MAKE) install-question
