set DST_DIR=protocol-buffer-src
protoc --cpp_out=%DST_DIR% MapProtocol.proto
protoc --cpp_out=%DST_DIR% Message.proto