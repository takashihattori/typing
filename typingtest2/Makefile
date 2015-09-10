CC = gcc
CFLAGS = -O -DUNIX -funsigned-char

SRCS = Intprt.c main.c Global.c Token.c Terminal.c Menu.c Test.c Variable.c Time.c ErrChk.c Examination.c
OBJS = Intprt.o main.o Global.o Token.o Terminal.o Menu.o Test.o Examination.o Variable.o Time.o ErrChk.o 
LIBS = -lldap

all : typing data

install:
	install -c -d -m 644 ja/*.txt $(DEST_DIR)
	install -c -d -m 644 en/*.txt $(DEST_DIR)
	install -c -m 755 typing $(DEST_DIR)

typing: $(OBJS) Makefile
	gcc $(CFLAGS) $(LIBS) -o typing $(OBJS)

depend:
	mkdep $(SRCS)

clean:
	@rm -f typing *.o core *~

data: 
	./translate.sh
