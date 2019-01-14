BINARY		 = firmware
OBJS		+= main.o

include ../common.mk

check: $(SRC)
	$(CHECKER) $(CHECKERFLAGS) $(SRC)

gen-docs: $(HDR) $(SRC) 
	$(DOXYGEN) $(DOXYGENFLAGS)
