CC=gcc
CFLAGS=-Wall -Wextra -g
PROJECT_BINARY_OUTPUT=bin

.PHONY: all

all: main

## Build:
clean: ## Cleans output folder
	@rm -rf ${PROJECT_BINARY_OUTPUT}

main: clean ## Builds main
	@mkdir -p ${PROJECT_BINARY_OUTPUT}
	@$(CC) $(CFLAGS) -o ${PROJECT_BINARY_OUTPUT}/main -DDEBUG=0 main.c heap.c
	@./${PROJECT_BINARY_OUTPUT}/main

test: clean ## Builds simulations
	@mkdir -p ${PROJECT_BINARY_OUTPUT}
	@$(CC) $(CFLAGS) -o ${PROJECT_BINARY_OUTPUT}/test -DDEBUG=0 test.c heap.c
	@./${PROJECT_BINARY_OUTPUT}/test

sketch: clean ## Builds sketch
	@mkdir -p ${PROJECT_BINARY_OUTPUT}
	@$(CC) $(CFLAGS) -o ${PROJECT_BINARY_OUTPUT}/sketch sketch.c
	@./${PROJECT_BINARY_OUTPUT}/sketch

## Help:
help: ## Show this help.
	@echo ''
	@echo 'Usage:'
	@echo '  make <target>'
	@echo ''
	@echo 'Targets:'
	@awk 'BEGIN {FS = ":.*?## "} { \
		if (/^[a-zA-Z_-]+:.*?##.*$$/) {printf "    %-20s%s\n", $$1, $$2} \
		else if (/^## .*$$/) {printf "  %s\n", substr($$1,4)} \
		}' $(MAKEFILE_LIST)
