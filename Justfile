default:
    @just --list

build *args:
    @./build.sh {{args}}

run *args:
    @./run.sh {{args}}

clean:
    @rm -rf _*

test project *args:
    @./test.sh {{project}} {{args}}


full-test project: clean (test project)
    @just run {{project}}

alias b := build
alias r := run
alias c := clean
alias ft := full-test
