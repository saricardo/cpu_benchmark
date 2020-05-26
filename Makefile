.PHONY = all clean

MKDIR_P = mkdir -p

CC = gcc

CCT = aarch64-linux-gnu-gcc

SRCPATH := src
INCPATH := inc
BUILDPATH := build
ROOTDIR := .

SRCS := $(wildcard $(SRCPATH)/*.c)
OBJS := $(SRCS:%.c=%.o)
BIN := $(wildcard $(BUILDPATH)/*)


CFLAGS = -Wall -Idir
LINKERFLAG = -lm

all: gen_amd64

gen_amd64: $(OBJS)
	$(MKDIR_P) $(BUILDPATH)
	@echo "creating amd64 object..."
	${CC} ${LINKERFLAG} $^ -o $(BUILDPATH)/$@

%.o: $(SRCPATH)/%.c $(INCPATH)/*.h
	@echo "Creating Object..."
	${CC} -c $(CFLAGS) $^ 

clean:
	@echo "Cleaning up..."
	rm $(OBJS) $(BINS)

