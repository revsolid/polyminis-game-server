#!/bin/sh
aws gamelift upload-build --operating-system "AMAZON_LINUX" --name "Polyminis Server 1" --build-version "0.0.1" --build-root ../bin
