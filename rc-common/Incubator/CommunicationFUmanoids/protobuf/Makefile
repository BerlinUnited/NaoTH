CPP_PROTOFILES := $(patsubst %.proto,%.pb.cc,$(wildcard *.proto))
PYTHON_PROTOFILES := $(patsubst %.proto,../../../install/protobuf/%_pb2.py,$(wildcard *.proto))

all: $(CPP_PROTOFILES) 

python: $(PYTHON_PROTOFILES)

clean:
	rm -f $(wildcard *.pb.cc) $(wildcard *.pb.h) 

%.pb.cc: %.proto
	../../../tools/protobuf/bin/protoc --cpp_out=. $<

../../../install/protobuf/%_pb2.py: %.proto
	../../../tools/protobuf/bin/protoc --python_out=../../../install/protobuf $<

