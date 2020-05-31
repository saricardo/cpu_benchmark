.PHONY = all clean

MKDIR_P = mkdir -p

CC = aarch64-linux-gnu-gcc

SRCPATH := ../../src
INCPATH := ../../inc
BUILDPATH := ../../build
ROOTDIR := ../..

SRCS := $(wildcard $(SRCPATH)/*.c)
OBJS := $(SRCS:%.c=%.o)
BIN := $(wildcard $(BUILDPATH)/*)


CFLAGS = -Wall -Idir
LINKERFLAG = -lm

all: gen_arm64

gen_arm64: $(OBJS)
	$(MKDIR_P) $(BUILDPATH)
	@echo "creating arm64 object..."
	${CC} ${LINKERFLAG} $^ -o $(BUILDPATH)/$@
	rm $(OBJS)

%.o: $(SRCPATH)/%.c $(INCPATH)/*.h
	@echo "Creating Object..."
	${CC} -c $(CFLAGS) $^ 

clean:
	@echo "Cleaning up..."
	rm $(OBJS) $(BINS)

