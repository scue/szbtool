CC := gcc
RM := rm -f
OBJS := getinfo.o fileoperate.o szbtool.o

szbtool: $(OBJS)
	$(CC) 	$^ -o $@

szbtool.o: szbtool.c szb.h libget.h libfile.h 
getinfo.o: getinfo.c szb.h
fileoperate.o: fileoperate.c szb.h

clean:
	$(RM) 	szbtool $(OBJS)
