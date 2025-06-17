default:
    @just --list

build:
    @./build.sh

run *args: build
    @_bin/hello {{args}}

clean:
    @rm -rf _*

test:
    @./test.sh

full-test: clean build test run

alias b := build
alias r := run
alias c := clean
alias ft := full-test
