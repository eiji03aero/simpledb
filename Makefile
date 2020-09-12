.PHONY: build-cmake
build-cmake:
	cd build && cmake ..

.PHONY: dev
dev:
	cd build && make && ./simpledb
