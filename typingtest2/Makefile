CC = gcc
CFLAGS = -O  -DUNIX -funsigned-char
DEST_DIR = /usr/local/lib/typing

SRCS = Intprt.c main.c Global.c Token.c Terminal.c Menu.c Test.c Variable.c Time.c ErrChk.c Examination.c
OBJS = Intprt.o main.o Global.o Token.o Terminal.o Menu.o Test.o Examination.o Variable.o Time.o ErrChk.o 

all : typing

install:
	for f in *.txt ;\
	  do install -c -m 644 $$f $(DEST_DIR); \
	done
	install -c -m 755 typing $(DEST_DIR)

typing: $(OBJS)
	gcc -funsigned-char -lldap -o typing $(OBJS)

depend:
	mkdep $(SRCS)

clean:
	@rm -f typing *.o core *~

