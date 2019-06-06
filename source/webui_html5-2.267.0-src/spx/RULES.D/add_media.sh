#!/bin/sh

sed -i '/\/\/##MEDIA_TAGS/{r media.tag
d}' ./app/config.js
sed -i '/\/\/##MEDIA_SHIMS/{r media.shim
d}' ./app/config.js