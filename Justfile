default:
    @just --list

build project:
    @./build.sh {{project}}

run project *args:
    @./run.sh {{project}} {{args}}

clean:
    @rm -rf _*

test project *args:
    @./test.sh {{project}} {{args}}


full-test project: clean (test project) (run project)

alias b := build
alias r := run
alias c := clean
alias ft := full-test
