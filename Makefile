# Makefile to run zsh under x86_64 architecture

.PHONY: run setup stop-arch

run:
	@arch -x86_64 /bin/zsh
	@uname -m

setup:
	@brew install python
	@softwareupdate --install-rosetta --agree-to-license

stop-arch:
	@if [ "$$(uname -m)" = "x86_64" ];
	then
		exit 0; \
	fi
