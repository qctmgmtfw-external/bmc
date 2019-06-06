#!/bin/sh

sed -i '/\/\/##KVM_TAGS/{r kvm.tag
d}' ./app/config.js
sed -i '/\/\/##KVM_SHIMS/{r kvm.shim
d}' ./app/config.js