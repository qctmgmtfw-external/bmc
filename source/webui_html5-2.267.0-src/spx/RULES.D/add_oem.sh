#!/bin/sh

sed -i '/\/\/##OEM_TAGS/{r oem.tag
d}' ./app/config.js
sed -i '/\/\/##OEM_SHIMS/{r oem.shim
d}' ./app/config.js