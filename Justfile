default:
    @just --list

build:
    @./build.sh

run *args: build
    @_bin/<my-exe> {{args}}

clean:
    @rm -rf _*

full-test: clean build run

alias b := build
alias r := run
alias c := clean
alias ft := full-test
