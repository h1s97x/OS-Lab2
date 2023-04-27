srcs = lab2.c
objs = lab2.o
opts = -g -c
all: lab2
lab2: $(objs)
	gcc $(objs) -o lab2
lab2.o: $(srcs)
	gcc $(opts) $(srcs)
clean:
	rm lab2 *.o