obj-m += vvaltz.o
vvaltz-objs := main.o proc.o policy.o hooks.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean