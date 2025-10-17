.PHONY: help build test clean

help: ## Show available commands
	@grep -E '^[a-zA-Z0-9_%/-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

build: ## Build executables
	$(MAKE) -C src

test: build ## Build and run Unity tests
	$(MAKE) -C test test

clean: ## Clear build environment
	$(MAKE) -C src clean
	$(MAKE) -C test clean