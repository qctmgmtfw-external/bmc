#!/bin/sh

if ! type curl &> /dev/null; then
	echo "curl is not found. Installing now..."
	sudo apt-get install curl	
fi 

if ! type npm &> /dev/null; then
	echo "Node.js is not found. Installing now... (http://nodejs.org)"
	curl -sL https://deb.nodesource.com/setup | sudo bash -
	sudo apt-get install nodejs
	if [ "$?" != "0" ]; then
		LATEST_VERSION="v0.10.30"
		curl -O http://nodejs.org/dist/$LATEST_VERSION/node-${LATEST_VERSION}.tar.gz
		tar xfz node-${LATEST_VERSION}.tar.gz
		cd node-${LATEST_VERSION}
		./configure --prefix=/usr
		make
		sudo make install
	fi
fi

if ! type grunt &> /dev/null; then
	echo "Grunt/Bower/coveralls are required and one or more is not present. Installing them now..."
	sudo npm i -gq grunt-cli bower coveralls
	echo "Done"
fi