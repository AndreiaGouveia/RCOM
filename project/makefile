# Project properties
PROGRAM = writer
PROGRAM_1 = receiver

#source options
SRCDIR =
C_FILES := $(wildcard $(SRCDIR)/*.c)

#build options
BUILDDIR = build
OBJS := writerDataPacket.c linkLayerWriter.c linkLayer.c writenoncanonical.c
OBJS_1 := linkLayerReceiver.c receiverDataPacket.c linkLayer.c noncanonical.c

#compiler options
CFLAGS = -Wall -Wextra -Werror -pedantic
LDFLAGS =
LDLIBS = -lm

all: $(PROGRAM) $(PROGRAM_1)

$(PROGRAM): .depend $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(PROGRAM) $(LDLIBS)

$(PROGRAM_1): .depend $(OBJS_1)
	$(CC) $(CFLAGS) $(OBJS_1) $(LDFLAGS) -o $(PROGRAM_1) $(LDLIBS)

# Dependency management

depend: .depend

.depend: cmd = gcc -MM -MF depend $(var); echo -n "$(BUILDDIR)/" >> .depend; cat depend >> .depend;
.depend:
	@echo "Generating dependencies..."
	@$(foreach var, $(C_FILES), $(cmd))
	@rm -f depend

-include .depend

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

%: %.c
	$(CC) $(CFLAGS) -o $@ $<


clean:
	rm -f .depend $(PROGRAM)
	rm -rf $(BUILDDIR)

$(BUILDDIR):
	mkdir $(BUILDDIR)

.PHONY: clean depend
