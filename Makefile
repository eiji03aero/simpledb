MAIN_NAME = simpledb

.PHONY: build
build:
	@cd build \
		&& make

.PHONY: build-cmake
build-cmake:
	@cd build \
		&& cmake ..

.PHONY: test
test:
	@cd build \
		&& ctest

.PHONY: test-unit
test-unit:
	@cd build \
		&& $(MAKE) \
		&& ./$(TEST_NAME)

.PHONY: dev
dev:
	@cd build \
		&& $(MAKE) $(MAIN_NAME) \
		&& ./$(MAIN_NAME) mydb.db

.PHONY: watch
watch:
	@reflex \
		-r '(\.cpp|\.h)' \
		-s $(MAKE) $(COMMAND)

.PHONY: clean
clean:
	@rm -rf ./build/*
