#!/bin/bash

set -xe

srcdir="$(pwd)/.."

sudo docker build \
        --tag "epoxyci" \
        --file "Dockerfile" .
sudo docker run --rm \
        --volume "${srcdir}:/root/epoxy" \
        --tty --interactive "epoxyci" bash
