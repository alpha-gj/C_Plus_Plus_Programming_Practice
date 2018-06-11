DAEMON = queue_pro_cus
TARGETS = $(DAEMON)

SRCS = $(TARGETS).cpp

CC = g++ -g -lpthread

LDLIBS = -lpthread 

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

$(DAEMON): $(TARGETS).o
