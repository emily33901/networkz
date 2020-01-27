#!/bin/sh

cd "$(dirname "$0")"

GOOS=js GOARCH=wasm go build -o main.wasm
