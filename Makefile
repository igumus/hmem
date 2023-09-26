CC=gcc
CFLAGS=-Wall -Wextra 
PROJECT_BINARY=main
PROJECT_BINARY_OUTPUT=bin

.PHONY: all

all: run

## Build:
clean: ## Cleans output folder
	@rm -rf ${PROJECT_BINARY_OUTPUT}

main: clean ## Builds project
	@mkdir -p ${PROJECT_BINARY_OUTPUT}
	@$(CC) $(CFLAGS) -o ${PROJECT_BINARY_OUTPUT}/${PROJECT_BINARY} main.c

run: main ## Runs project
	@./${PROJECT_BINARY_OUTPUT}/${PROJECT_BINARY}

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
