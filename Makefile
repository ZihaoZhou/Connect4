CC := gcc
CXXFLAGS=-Wall -pedantic -std=c11

# Folders
SRCDIR := src
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj

connect4: init $(addprefix $(OBJDIR)/,connect4_functions.o connect4.o)
	$(CC) $(LDFLAGS) -o $(BUILDDIR)/$@ $(filter-out init,$^)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CXXFLAGS) -c -o $@ $<

.PHONY: init
init:
	@mkdir -p $(OBJDIR)

.PHONY: clean
clean:
	@rm -rf $(BUILDDIR)


