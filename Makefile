NAME = regctl

obj-m := $(NAME).o

PWD := $(shell pwd)
KDIR := /lib/modules/$(shell uname -r)/build

all: main
	$(MAKE) -C $(KDIR) M=$(PWD) modules

main: main.cpp
	g++ -o main main.cpp

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f main
insmod:
	sudo insmod $(NAME).ko
rmmod:
	sudo rmmod $(NAME)