#!/bin/bash

cd "$(dirname "$0")"

goexec 'http.ListenAndServe(`:8080`, http.FileServer(http.Dir(`.`)))'