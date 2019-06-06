var MediaSocket = function(host, port, protocol, useSSL) {
    var _parent = this;

    this.host = host;
    this.port = port || 9667;
    this.protocol = protocol;

    this.pos = 0;

    this.inBuffer = new DataStream();
    this.outBuffer = new DataStream();

    this.nwBuffer = new DataStream();

    this.useSSL = useSSL;
};

MediaSocket.prototype = {

    connect: function() {
        var ws_proto;

        if (this.useSSL) {
            ws_proto = "wss://";
        } else {
            ws_proto = "ws://";
        }

        if (this.host.indexOf("[") != -1 && /Trident\/|MSIE /.test(window.navigator.userAgent)) {
            this.host = this.host.split(":").join("-");
            this.host = this.host.replace(/[\[\]]+/g, '');
            this.host += ".ipv6-literal.net";
        }

        if(this.port == 9999) {
            this.sock = new WebSocket(ws_proto + this.host + ":" + this.port + "/", ["binary", "base64"]);
        }
        else {
            this.sock = new WebSocket(ws_proto + this.host + "/cd-server", ["binary", "base64"]);
        }

        this.sock.binaryType = "arraybuffer";

        var _parent = this;

        this.sock.onopen = function(e) {
            //console.log("WebSocket connection open", this.protocol);
            if (this.protocol) {
                _parent.wsmode = this.protocol;
            } else {
                _parent.wsmode = "base64";
            }
            _parent.onopen && _parent.onopen(e);
        };

        this.sock.onmessage = function(e) {

            _parent.pauseRead = true;

            if (_parent.pos == _parent.nwBuffer.byteLength) {
                delete _parent.nwBuffer;
                _parent.nwBuffer = new DataStream();
                _parent.pos = 0;
            }

            var u8data = null;

            if (_parent.wsmode == "binary") {
                u8data = new Uint8Array(e.data);
            } else {
                u8data = Base64Binary.decode(e.data);
            }

            _parent.nwBuffer.writeUint8Array(u8data, DataStream.LITTLE_ENDIAN);
            _parent.pauseRead = false;

            if (!_parent.startReading) {
                _parent.startReading = true;
                /*setTimeout(function() {
                    _parent.startRead.call(_parent);
                }, 50);*/
            }

            /*var _p = _parent.nwBuffer.position;
            _parent.nwBuffer.seek(0);
            //console.log("new incoming packets sized ", _parent.nwBuffer.byteLength, _parent.nwBuffer.readUint8Array());
            _parent.nwBuffer.seek(_p);*/

            _parent.onmessage && _parent.onmessage(e);
        };

        this.sock.onclose = function(e) {
            console.log("Media WebSocket closed message", e);
            _parent.onclose && _parent.onclose(e);
        };

        this.sock.onerror = function(e) {
            console.log("Media WebSocket error message");
            _parent.onerror && _parent.onerror(e);
        };
    },

    close: function() {
        this.sock.close();
    },

    setBuffer: function(inBufferSize, outBufferSize, inBufferOrder, outBufferOrder) {
        this.inBuffer = new DataStream(inBufferSize, 0, inBufferOrder);
        this.outBuffer = new DataStream(outBufferSize, 0, outBufferOrder);
    },

    sendPacket: function(packet) {

        var packetsSent = window.packetsSent || 0;

        //console.log("sending packet to server...");
        try {
            packet.writePacket(this.outBuffer);

            packetsSent += this.outBuffer.byteLength;

            window.packetsSent = packetsSent;

            document.getElementById("sentcount").innerHTML = Math.ceil(packetsSent / 1024);
            this.outBuffer.seek(0);
            //console.log("outBuffer", this.outBuffer.readUint8Array());
            this.outBuffer.seek(0);
            if (this.wsmode == "binary") {
                this.sock.send(this.outBuffer.buffer);
            } else {
                //this.sock.send(base64ArrayBuffer(this.outBuffer.buffer));
                this.sock.send(Base64Binary.encode2(this.outBuffer.buffer));
            }
        } catch (e) {
            console.log(e);
        }
    },

    receivePacket: function(doTrim) {
        var packet = null,
            ibpos = null,
            dataPosition = 0,
            dataPacketLength = 0;

        if (doTrim === undefined) doTrim = true;

        //console.log("checking if inbuffer has some bytes left from previous transactions");
        if (this.inBuffer.position !== 0) {
            ibpos = this.inBuffer.position;
            /*this.inBuffer.seek(0);
            //console.log("inbuffer", this.inBuffer.readUint8Array());*/
            try {
                this.inBuffer.seek(0);
                packet = this.protocol.getPacket(this.inBuffer);
            } catch (e) {
                if (e.message.indexOf("buffer under flow") != -1) {
                    packet = null;
                    this.inBuffer.seek(ibpos);
                } else {
                    throw new Error("Terrible thing happened while media-socket receivePacket", e.message);
                }
            }

        }

        var retries = 0;

        while (packet === null) {
            retries++;

            if (retries > 50) throw new Error("Too many retires for receivePacket");

            //console.log("Not enough to make a packet. Reading from network buffer", this.pauseRead);
            if (!this.pauseRead) {
                if (this.nwBuffer.byteLength < this.pos + IUSBHeader.HEADER_LEN) {
                    //network buffer less than expected size so continue
                    //console.log("network buffer less than expected header size so continue", this.nwBuffer.byteLength, this.pos + IUSBHeader.HEADER_LEN);
                    continue;
                }
                var readHeaderBuffer = new Uint8Array(this.nwBuffer.buffer.slice(this.pos, this.pos + IUSBHeader.HEADER_LEN));

                this.inBuffer.writeUint8Array(readHeaderBuffer);

                this.inBuffer.seek(12);

                dataPacketLength = this.inBuffer.readInt32(DataStream.LITTLE_ENDIAN) & 0xffffffff;
                dataPosition = this.pos + IUSBHeader.HEADER_LEN;
                if (this.nwBuffer.byteLength < dataPosition + dataPacketLength) {
                    //network buffer less than expected size so continue
                    //console.log("network buffer less than expected data size so continue", this.nwBuffer.byteLength, dataPosition + dataPacketLength);
                    continue;
                }
                var dataBuffer = new Uint8Array(this.nwBuffer.buffer.slice(dataPosition, dataPosition + dataPacketLength));

                this.inBuffer.seek(IUSBHeader.HEADER_LEN);

                this.inBuffer.writeUint8Array(dataBuffer);

                ibpos = this.inBuffer.position;

                this.inBuffer.seek(0);

                try {
                    packet = this.protocol.getPacket(this.inBuffer);
                } catch (e) {
                    if (e.message.indexOf("buffer under flow") != -1) {
                        packet = null;
                        this.inBuffer.seek(ibpos);
                    } else {
                        throw new Error("Terrible thing happened while media-socket receivePacket", e.message);
                    }
                }

            }
        }

        this.inBuffer.seek(0);
        /*
        //console.log("inbuffer", this.inBuffer.readUint8Array());
        this.inBuffer.seek(0);*/

        if (doTrim) {
            this.pos = dataPosition + dataPacketLength; //next time a data receives in this.nwbuffer it will clear the older bytes
            //console.log("Trimming nwBuffer by ", this.pos, " bytes");
        }
        //console.log("Packet read successfully");

        return packet;
    },

    startRead: function() {
        /*//console.log(this.nwBuffer.byteLength);

        var _parent = this;
        if (!this.pauseRead) {
            var ihab = this.nwBuffer.buffer.slice(this.pos, this.pos + 8);

            var newBuffer = new DataStream(ihab);
        }*/
    }

};

if (typeof define == "function" && define.amd) {
    define(function() {
        return MediaSocket;

    });
}