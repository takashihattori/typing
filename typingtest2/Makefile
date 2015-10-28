CC = gcc
CFLAGS = -O -DUNIX -funsigned-char
# CFLAGS = -g -O0 -DDEBUG -DUNIX -funsigned-char

SRCS = Intprt.c main.c Global.c Token.c Terminal.c Menu.c Test.c Variable.c Time.c ErrChk.c Examination.c
OBJS = Intprt.o main.o Global.o Token.o Terminal.o Menu.o Test.o Examination.o Variable.o Time.o ErrChk.o 
LIBS = -lldap

all : typing data

install:
	install -c -m 755 typing $(DEST_DIR)/typing-mac

typing: $(OBJS) Makefile
	gcc $(CFLAGS) $(LIBS) -o typing $(OBJS)

depend:
	mkdep $(SRCS)

clean:
	@rm -f typing *.o core *~

question: 
	./question.sh

install-question:
	install -c -g ipl -m 664 ja_JP.eucJP/*.txt $(DEST_DIR)/ja_JP.eucJP
	install -c -g ipl -m 664 ja_JP.UTF-8/*.txt $(DEST_DIR)/ja_JP.UTF-8
	install -c -g ipl -m 664 en/*.txt $(DEST_DIR)/en
