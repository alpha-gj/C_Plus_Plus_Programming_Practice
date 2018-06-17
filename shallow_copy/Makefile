DAEMON = shallow_copy
TARGETS = $(DAEMON)

SRCS = $(DAEMON).cpp 

CC = g++ -g

.PHONY: all clean install

all: $(TARGETS) 

clean:
	rm -f *.o *.d *~ $(TARGETS)

install: all


%.d: %.c
	$(CC) -c -MM $(CPPFLAGS) $< | sed -re 's,^.*:,$*.o $*.d:,g' > $@
%.d: %.cpp
	$(CC) -c -MM $(CPPFLAGS) $< | sed -re 's,^.*:,$*.o $*.d:,g' > $@
DEPENDS := $(SRCS:%.c=%.d)
DEPENDS := $(DEPENDS:%.cpp=%.d)

$(DAEMON): $(DAEMON).o 

